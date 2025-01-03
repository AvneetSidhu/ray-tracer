#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include <iostream>
#include <thread>
#include "image_buffer.h"
#include <mutex> 

std::mutex cout_mutex; // Global mutex for serializing std::cout access

class camera {
    public:
        double aspect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;
        double vfov = 90;

        point3 lookfrom = point3(0,0,0);
        point3 lookat = point3(0,0,-1);
        vec3 vup = vec3(0,1,0);

        double defocus_angle = 0;
        double focus_dist = 10;

        void render(const hittable& world, int samples_per_pixel, int max_depth) {
            initialize();
            // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
            image_buffer image(image_height, image_width);
            std::vector<std::thread> threads;
            int start_row = 0;
            int end_row;

            std::cout << "Number of threads: " << num_threads << '\n';

            // std::cout << "image height: " << image_height << " image width: " << image_width << '\n';

            double pss = 1.0 / samples_per_pixel;

            for (int i = 0; i < num_threads; ++i) {
                start_row = i * rows_per_thread;
                end_row = start_row + rows_per_thread;
                if (i == num_threads - 1) {
                    end_row += remainder; // Last thread takes the remaining rows
                }

                // std::cout << "start " << start_row << " end " << end_row << " assigned to thread" << i + 1 << '\n' << std::flush;

                if (start_row < end_row) {
                    try {
                        threads.push_back(std::thread([this, start_row, end_row, &image, &world, samples_per_pixel, max_depth, pss]() {
                            try {
                                render_chunk(start_row, end_row, image, world, samples_per_pixel, max_depth, pss);
                            } catch (const std::exception& e) {
                                std::cerr << "Exception in thread " << std::this_thread::get_id() << ": " << e.what() << '\n';
                            }
                        }));

                        // std::cout << "Thread " << i + 1 << " added to the vector.\n";
                    } catch (const std::exception& e) {
                        std::cerr << "Error creating thread " << i << ": " << e.what() << '\n';
                        return; // Exit the function if thread creation fails
                    }

                } else {
                    std::cerr << "Invalid range for thread " << i << ": start_row = " << start_row << ", end_row = " << end_row << '\n';
                    return;
                }
            }

            // std::cout << "time to execute threads \n";

            for (auto& t : threads) {
                t.join();
            }

            image.write_to_ppm();

            std::clog << "\rDone.                          \n";
        }
    
    
    private:
        int image_height;
        point3 center;
        point3 pixel00_loc;
        vec3   pixel_delta_u; 
        vec3   pixel_delta_v;
        vec3 u, v, w;
        vec3 defocus_disk_u;
        vec3 defocus_disk_v;
        unsigned int num_threads;
        int rows_per_thread;
        int remainder;
        double pixel_samples_scale;

        void initialize() {
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            // auto focal_length = (lookfrom - lookat).length();
            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta / 2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (double (image_width) / image_height);

            pixel_samples_scale = 1.0 / samples_per_pixel;

            num_threads = std::thread::hardware_concurrency();

            rows_per_thread = image_height / num_threads;
            remainder = image_height % rows_per_thread; 

            std::cout << "Using " << num_threads << " threads \n";

            // std::cout << "pixel sample scale is " << pixel_samples_scale << '\n';
            center = lookfrom;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            //vectors to map 3d plane to 2d
            auto viewport_u = viewport_width * u; //map eye coordinates to viewport 
            auto viewport_v = viewport_height * -v; //map eye coordinates to viewport (invert y axis)

            pixel_delta_u = viewport_u / image_width; // inter-pixel distance u
            pixel_delta_v = viewport_v / image_height; // inter-pixel distance v 

            auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2; // vector for the upper left corner of the viewport
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); // vector for the (0,0) pixel of the viewport centre

            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }
        
        ray get_ray(int i, int j) {

            auto offset = sample_square();

            auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        vec3 sample_square() const {
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        point3 defocus_disk_sample() {
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) {
            if (depth <= 0) {
                return color(0,0,0);
            }
            hit_record rec;

            if (world.hit(r, interval(0.001, infinity), rec)) {
                ray scattered;
                color attenuation;
                if (rec.mat -> scatter(r, rec, attenuation, scattered))
                    return attenuation * ray_color(scattered, depth - 1, world);
                return color(0,0,0);
            }

            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            return (1.0 - a)*color(1.0,1.0,1.0) + a*color(0.5,0.7,1.0);
        }

        void print_message(const std::string& message ) {
            std::lock_guard<std::mutex> lock (cout_mutex);
            std::cout << "Thread " << "is printing this message:" << message << '\n' << std::endl;
        }

        void render_chunk(int start, int end, image_buffer& img, const hittable& world, int samples_per_pixel, int max_depth, double pixel_samples_scale) {
            // std::cout << "This is a thread with start and end of: " << start << ' ' << end << '\n';
            for (int j = start; j < end; j++) {
                // print_message("working on row" + std::to_string(j));
                for (int i = 0; i < image_width; i++) {
                    color pixel_color(0,0,0);
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i,j);
                        // std::cout << ray_color(r, max_depth, world) << "\n";
                        pixel_color += ray_color(r,max_depth, world);
                    }
                    // std::cout << pixel_samples_scale << '\n';
                    // if (j == 50 && i == 1) std::cout << "50 and 1 hit \n";
                    img.set_pixel(j, i, write_color(pixel_samples_scale * pixel_color));
                }
            }
        }
};

#endif
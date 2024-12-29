#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include <mutex> 
#include "vec3.h"
#include "color.h"
#include <vector>
#include <fstream> 
#include <stdexcept>
#include <sstream>  // For std::ostringstream

// represents points on a ray using the function P(t) = origin + tb, where P(t) is the point in a ray , t is a scalar representing position on the ray and b is the direction vector
// direction vector is dir = target - origin
class image_buffer {
    public: 
        int image_height; 
        int image_width;

        std::vector<std::vector<color>>buffer;
        std::mutex buffer_mutex;

        image_buffer(int height, int width) {
            image_height = height;
            image_width = width;
            std::cout << "created image with height: " << height << " and width " << width << '\n';
            // Resize the buffer to match the given dimensions
            buffer.resize(image_height, std::vector<color>(image_width));

            // Initialize the buffer with black pixels (0.0, 0.0, 0.0)
            for (int i = 0; i < image_height; ++i) {
                for (int j = 0; j < image_width; ++j) {
                    buffer[i][j] = color(0.0, 0.0, 0.0); // Initialize each pixel to black
                }
            }
        }

        void set_pixel(int i, int j, const color& c) {
            if (i < 0 || i >= image_height || j < 0 || j >= image_width) {
                std::ostringstream oss;
                oss << "pixel coordinates out of range: (" << i << ", " << j << ") "
                    << "Image dimensions: (" << image_width << " x " << image_height << ")";
                throw std::out_of_range(oss.str());
            }
            // std::cout << "Setting pixel (" << i << ", " << j << ") to color: " 
            //         << c.e[0] << " " << c.e[1] << " " << c.e[2] << std::endl;
            std::lock_guard<std::mutex> lock(buffer_mutex);
            buffer[i][j] = c;
        }

        void write_to_ppm() {
            std::ofstream out("image.ppm");

            // Write PPM header
            out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            // Loop through the buffer and write pixel data
            for (int j = 0; j < image_height; j++) {
                for (int i = 0; i < image_width; i++) {

                    color& pixel = buffer[j][i];

                    
                    int r = pixel.e[0]; 
                    int g = pixel.e[1];
                    int b = pixel.e[2];

                    out << r << ' ' << g << ' ' << b << '\n';
                }
            }

            // Close the file
            out.close();
            std::cout << "Image written to image.ppm\n";
        }


};

#endif
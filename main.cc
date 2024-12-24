#include <iostream>
#include <cmath>
#include "vec3.h"
#include "color.h"
#include "ray.h"

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a)*color(1.0,1.0,1.0) + a*color(0.5,0.7,1.0);
}

int main() {

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double (image_width) / image_height);
    auto camera_centre = point3(0,0,0);

    //vectors to map 3d plane to 2d
    auto viewport_u = vec3(viewport_width, 0, 0); //map eye coordinates to viewport 
    auto viewport_v = vec3(0,-viewport_height, 0); //map eye coordinates to viewport (invert y axis)

    auto pixel_delta_u = viewport_u / image_width; // inter-pixel distance u
    auto pixel_delta_v = viewport_v / image_height; // inter-pixel distance v 

    auto viewport_upper_left = camera_centre - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2; // vector for the upper left corner of the viewport
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); // vector for the (0,0) pixel of the viewport centre

    //render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_width; j ++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++ ) {
            auto pixel_centre = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v); 
            auto ray_direction = pixel_centre - camera_centre;
            ray r(camera_centre, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                          \n";
}

#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include "vec3.h"
// represents points on a ray using the function P(t) = origin + tb, where P(t) is the point in a ray , t is a scalar representing position on the ray and b is the direction vector
// direction vector is dir = target - origin
class image_buffer {
    public: 
        static const int image_width = 800;
        static const int image_height = 600;

        std::array<std::array<color, image_width>, image_height> buffer;

        image_buffer () {
            for (int i = 0; i < image_height; ++i) {
                for (int j = 0; j < image_width; ++j) {
                    buffer[i][j] = color(0.0,0.0,0.0);
                }
            }
        }

        void set_pixel(int i, int j, const color& c) {
            if (i < 0 || i >= image_height || j < 0 || j >= image_width) {
                throw std::out_of_range("pixel coordinates out of range");
            }
            buffer[i][j] = c;
        }

        bool write_to_ppm(){
            return true;
        }

};

#endif
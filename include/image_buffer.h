#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include "vec3.h"
#include "color.h"
#include <vector>
// represents points on a ray using the function P(t) = origin + tb, where P(t) is the point in a ray , t is a scalar representing position on the ray and b is the direction vector
// direction vector is dir = target - origin
class image_buffer {
    public: 
        int image_height; 
        int image_width;

        std::vector<std::vector<color>>buffer;

        image_buffer (int height, int width) {
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
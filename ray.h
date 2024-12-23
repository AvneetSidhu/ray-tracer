ifndef RAY_H
define RAY_H

#include "vec3.h"
// represents points on a ray using the function P(t) = origin + tb, where P(t) is the point in a ray , t is a scalar representing position on the ray and b is the direction vector
// direction vector is dir = target - origin
class ray {
    public: 
        ray(){}

        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        point3 at(double t) const {
            return orig + t* dir;
        }

        private:
        point3 orig;
        vec3 dir;
}
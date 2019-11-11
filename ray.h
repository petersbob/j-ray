#ifndef RAY_H
#define RAY_H

#include "vector.h"

class ray {
    public:
        ray() {}
        ray(const Vector3& a, const Vector3& b, float ti = 0.0) { A = a; B = b; _time = ti;}
        Vector3 origin() const { return A; }
        Vector3 direction() const { return B; }
        float time() const { return _time; };
        Vector3 point_at_parameter(float t) const { return A + t*B; }

        Vector3 A;
        Vector3 B;
        float _time;

};

#endif
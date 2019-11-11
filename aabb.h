#ifndef AABBH
#define AABBH

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class AABB {
    public:
        AABB() {}
        AABB(const Vector3& a, const Vector3& b) { _min = a; _max = b; }
        Vector3 min() const {return _min; }
        Vector3 max() const {return _max; }

        bool hit(const Ray& r, float tmin, float tmax) const {
            for (int a = 0; a < 3; a++) {
                float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
                                (_max[a] - r.origin()[a]) / r.direction()[a]);
                float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
                                (_max[a] - r.origin()[a]) / r.direction()[a]);
                tmin = ffmax(t0, tmin);
                tmax = ffmin(t1, tmax);
                if (tmax <= tmin) return false;
            }
            return true;
        }

    Vector3 _min;
    Vector3 _max;
};

AABB surrounding_box(AABB box0, AABB box1) {
    Vector3 small( fmin(box0.min().x(), box1.min().x()),
                fmin(box0.min().y(), box1.min().y()),
                fmin(box0.min().z(), box1.min().z()));

    Vector3 big( fmax(box0.max().x(), box1.max().x()),
                fmax(box0.max().y(), box1.max().y()),
                fmax(box0.max().z(), box1.max().z()));

    return AABB(small,big);
}

/*
    Hit method variation that is quicker on many compilers
*/
// inline bool AABB::hit(const Ray& r, float tmin, float tmax) const {
//     for (int a = 0; a < 3; a++) {
//         float invD = 1.0f / r.direction()[a];
//         float t0 = (min()[a] - r.origin()[a]) * invD;
//         float t1 = (max()[a] - r.origin()[a]) * invD;
//         if (invD < 0.0f) std::swap(t0, t1);
//         tmin = t0 > tmin ? t0 : tmin;
//         tmax = t1 < tmax ? t1 : tmax;
//         if (tmax <= tmin) return false;
//     }
//     return true;
// }

#endif
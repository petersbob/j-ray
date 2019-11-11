#ifndef SHAPEH
#define SHAPEH

#include "hitable.h"

class Shape: public hitable {
    public:
        Shape() {}
        Shape(hitable *p)  :  ptr(p) {}
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;

        hitable *ptr;

};

bool Shape::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    return ptr->hit(r, t_min, t_max, rec);
}

bool Shape::bounding_box(float t0, float t1, aabb& box) const {
    return ptr->bounding_box(t0, t1, box);
}

#endif
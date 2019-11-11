#ifndef BOXH
#define BOXH

#include "hitable.h"
#include "material.h"

class Box: public Hitable {
    public:
        Box() {}
        Box(const Vector3& p0, const Vector3& p1, Material *ptr);
        virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            box = AABB(pmin, pmax);
            return true;
        }
        Vector3 pmin, pmax;
        Hitable *listPtr;
};

Box::Box(const Vector3& p0, const Vector3& p1, Material *ptr) {
    pmin = p0;
    pmax = p1;
    Hitable **list = new Hitable*[6];
    list[0] = new XYRect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
    list[1] = new FlipNormals(new XYRect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    list[2] = new XZRect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
    list[3] = new FlipNormals(new XZRect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    list[4] = new YZRect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
    list[5] = new FlipNormals(new YZRect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    listPtr = new HitableList(list, 6);
}

bool Box::hit(const Ray& r, float t0, float t1, HitRecord& rec) const {
    return listPtr->hit(r, t0, t1, rec);
}

#endif
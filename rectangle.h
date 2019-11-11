#ifndef RECTANGLEH
#define RECTANGLEH

#include "hitable.h"
#include "hitableList.h"
#include "material.h"

class XYRect: public Hitable {
    public:
        XYRect() {}
        XYRect(float _x0, float _x1, float _y0, float _y1, float _k, Material *mat) : 
        x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};
        virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            box = AABB(Vector3(x0, y0, k-0.0001), Vector3(x1, y1, k+0.0001));
            return true;
        }
        Material *mp;
        float x0, x1, y0, y1, k;
};

class XZRect: public Hitable {
    public:
        XZRect() {}
        XZRect(float _x0, float _x1, float _z0, float _z1, float _k, Material *mat) : 
        x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
        virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            box = AABB(Vector3(x0, z0, k-0.0001), Vector3(x1, z1, k+0.0001));
            return true;
        }
        Material *mp;
        float x0, x1, z0, z1, k;
};

class YZRect: public Hitable {
    public:
        YZRect() {}
        YZRect(float _y0, float _y1, float _z0, float _z1, float _k, Material *mat) : 
        y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
        virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            box = AABB(Vector3(y0, y0, k-0.0001), Vector3(z1, z1, k+0.0001));
            return true;
        }
        Material *mp;
        float y0, y1, z0, z1, k;
};

bool XYRect::hit(const Ray& r, float t0, float t1, HitRecord& rec) const {
    float t = (k-r.origin().z()) / r.direction().z();
    if (t < t0 || t > t1)
        return false;
    float x = r.origin().x() + t*r.direction().x();
    float y = r.origin().y() + t*r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1){
        return false;
    }

    rec.u = (x-x0)/(x1-x0);
    rec.v = (y-y0)/(y1-y0);
    rec.t = t;
    rec.matPtr = mp;
    rec.p = r.pointAtParameter(t);
    rec.normal = Vector3(0,0,1);
    return true;
}

bool XZRect::hit(const Ray& r, float t0, float t1, HitRecord& rec) const {
    float t = (k-r.origin().y()) / r.direction().y();
    if (t < t0 || t > t1)
        return false;
    float x = r.origin().x() + t*r.direction().x();
    float z = r.origin().z() + t*r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1) 
        return false;
    rec.u = (x-x0)/(x1-x0);
    rec.v = (z-z0)/(z1-z0); 
    rec.t = t;
    rec.matPtr = mp;
    rec.p = r.pointAtParameter(t);
    rec.normal = Vector3(0, 1, 0);
    return true;
}

bool YZRect::hit(const Ray& r, float t0, float t1, HitRecord& rec) const {
    float t = (k-r.origin().x()) / r.direction().x();
    if (t < t0 || t > t1)
        return false;
    float y = r.origin().y() + t*r.direction().y();
    float z = r.origin().z() + t*r.direction().z();
    if (y < y0 || y > y1 || z < z0 || z > z1) 
        return false;
    rec.u = (y-y0)/(y1-y0);
    rec.v = (z-z0)/(z1-z0); 
    rec.t = t;
    rec.matPtr = mp;
    rec.p = r.pointAtParameter(t);
    rec.normal = Vector3(1, 0, 0);
    return true;
}

#endif
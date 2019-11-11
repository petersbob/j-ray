#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "material.h"

void getSphereUV(const Vector3& p, float& u, float& v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1-(phi + M_PI) / (2*M_PI);
    v = (theta + M_PI/2) / M_PI;
}

class Sphere: public Hitable {
    public:
        Sphere() {}
        Sphere(Vector3 cen, float r, Material *m) : center(cen), radius(r), matPtr(m) {};
        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const;
        Vector3 center;
        float radius;
        Material *matPtr;
};

bool Sphere::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    Vector3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(b*b-a*c)) / a;
        if (temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.matPtr = matPtr;
            getSphereUV((rec.p-center)/radius, rec.u, rec.v);
            return true;
        }
        temp = (-b + sqrt(b*b-a*c))/a;
        if (temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.matPtr = matPtr;
            getSphereUV(rec.p, rec.u, rec.v);
            return true;
        }
    }
    return false;
};

bool Sphere::boundingBox(float t0, float t1, AABB& box) const {
    box = AABB(center  - Vector3(radius, radius, radius), center + Vector3(radius, radius, radius));
    return true;
}

class movingSphere: public Hitable {
    public:
        movingSphere() {}
        movingSphere(Vector3 cen0, Vector3 cen1, float t0, float t1, float r, Material *m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), matPtr(m) {};
        virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
        Vector3 center(float time) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const;
        Vector3 center0, center1;
        float time0, time1;
        float radius;
        Material *matPtr;
};

Vector3 movingSphere::center(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool movingSphere::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    Vector3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.matPtr = matPtr;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.matPtr = matPtr;
            return true;
        }
    }
    return false;
}

bool movingSphere::boundingBox(float t0, float t1, AABB& box) const {
    AABB box0(center(t0) - Vector3(radius,radius,radius), center(t0) + Vector3(radius, radius, radius));
    AABB box1(center(t1) - Vector3(radius,radius,radius), center(t1) + Vector3(radius, radius, radius));

    box = surroundingBox(box0, box1);
    return true;
}

#endif
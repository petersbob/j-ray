#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "material.h"

void get_sphere_uv(const Vector3& p, float& u, float& v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1-(phi + M_PI) / (2*M_PI);
    v = (theta + M_PI/2) / M_PI;
}

class Sphere: public Hitable {
    public:
        Sphere() {}
        Sphere(Vector3 cen, float r, Material *m) : center(cen), radius(r), mat_ptr(m) {};
        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
        virtual bool bounding_box(float t0, float t1, AABB& box) const;
        Vector3 center;
        float radius;
        Material *mat_ptr;
};

bool Sphere::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
    Vector3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(b*b-a*c)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
            return true;
        }
        temp = (-b + sqrt(b*b-a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.p, rec.u, rec.v);
            return true;
        }
    }
    return false;
};

bool Sphere::bounding_box(float t0, float t1, AABB& box) const {
    box = AABB(center  - Vector3(radius, radius, radius), center + Vector3(radius, radius, radius));
    return true;
}

class moving_sphere: public Hitable {
    public:
        moving_sphere() {}
        moving_sphere(Vector3 cen0, Vector3 cen1, float t0, float t1, float r, Material *m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {};
        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;
        Vector3 center(float time) const;
        virtual bool bounding_box(float t0, float t1, AABB& box) const;
        Vector3 center0, center1;
        float time0, time1;
        float radius;
        Material *mat_ptr;
};

Vector3 moving_sphere::center(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
    Vector3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool moving_sphere::bounding_box(float t0, float t1, AABB& box) const {
    AABB box0(center(t0) - Vector3(radius,radius,radius), center(t0) + Vector3(radius, radius, radius));
    AABB box1(center(t1) - Vector3(radius,radius,radius), center(t1) + Vector3(radius, radius, radius));

    box = surrounding_box(box0, box1);
    return true;
}

#endif
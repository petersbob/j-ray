#ifndef HITABLEH
#define HITABLEH

#include "ray.h"
#include "aabb.h"
#include "float.h"

class Material;

struct HitRecord {
    float t;
    Vector3 p;
    Vector3 normal;
    Material *mat_ptr;
    float u, v;
};

class Hitable {
    public:
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, AABB& box) const = 0;
};

int box_x_compare (const void * a, const void *b) {
    AABB box_left, box_right;
    Hitable *ah = *(Hitable**)a;
    Hitable *bh = *(Hitable**)a;
    if (!ah->bounding_box(0,0, box_left) || !bh->bounding_box(0,0, box_right))
        std::cerr << "no bounding box in bvh_onde constructor\n";
    if (box_left.min().x() - box_right.min().x() < 0.0 )
        return -1;
    else 
        return 1;
}

int box_y_compare (const void * a, const void *b) {
    AABB box_left, box_right;
    Hitable *ah = *(Hitable**)a;
    Hitable *bh = *(Hitable**)a;
    if (!ah->bounding_box(0,0, box_left) || !bh->bounding_box(0,0, box_right))
        std::cerr << "no bounding box in bvh_onde constructor\n";
    if (box_left.min().y() - box_right.min().y() < 0.0 )
        return -1;
    else 
        return 1;
}

int box_z_compare (const void * a, const void *b) {
    AABB box_left, box_right;
    Hitable *ah = *(Hitable**)a;
    Hitable *bh = *(Hitable**)a;
    if (!ah->bounding_box(0,0, box_left) || !bh->bounding_box(0,0, box_right))
        std::cerr << "no bounding box in bvh_onde constructor\n";
    if (box_left.min().z() - box_right.min().z() < 0.0 )
        return -1;
    else 
        return 1;
}

class BVHNode : public Hitable {
    public:
        BVHNode() {}
        BVHNode(Hitable **l, int n , float time0, float time1);
        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
        virtual bool bounding_box(float t0, float t1, AABB& box) const;
        Hitable *left;
        Hitable *right;
        AABB box;
};

bool BVHNode::bounding_box(float t0, float t1, AABB& b) const {
    b = box;
    return true;
}

BVHNode::BVHNode(Hitable **l, int n, float time0, float time1) {
    int axis = int(3*drand48());
    if (axis == 0)
        qsort(l, n, sizeof(Hitable *), box_x_compare);
    else if (axis == 1)
        qsort(l, n, sizeof(Hitable *), box_y_compare);
    else
        qsort(l, n, sizeof(Hitable *), box_z_compare);
    
    if (n == 1) {
        left = right = l[0];
    } else if (n == 2) {
        left = l[0];
        right = l[1];
    } else {
        left = new BVHNode(l, n/2, time0, time1);
        right = new BVHNode(l + n/2, n-n/2, time0, time1);
    }
    AABB box_left, box_right;
    if(!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0,time1, box_right))
        std::cerr << "no bounding box in BVHNode constructor\n";
    box = surrounding_box(box_left, box_right);
}

bool BVHNode::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
    if (box.hit(r, t_min, t_max)) {
        HitRecord left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        } else if (hit_left) {
            rec = left_rec;
            return true;
        } else if (hit_right) {
            rec = right_rec;
            return true;
        } else
            return false;
    } else
        return false;
}

class FlipNormals : public Hitable {
    public:
        FlipNormals(Hitable *p) : ptr(p) {}
        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
            if (ptr->hit(r, t_min, t_max, rec)) {
                rec.normal = -rec.normal;
                return true;
            } else {
                return false;
            }
        }
        virtual bool bounding_box(float t0, float t1, AABB& box) const {
            return ptr->bounding_box(t0, t1, box);
        }

        Hitable *ptr;
};

class Translate : public Hitable {
    public:
        Translate(Hitable *p, const Vector3& displacement) : ptr(p), offset(displacement) {}
        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;
        virtual bool bounding_box(float t0, float t1, AABB& box) const;
        Hitable *ptr;
        Vector3 offset;
};

bool Translate::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
    Ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (ptr->hit(moved_r, t_min, t_max, rec)) {
        rec.p += offset;
        return true;
    } else {
        return false;
    }
}

bool Translate::bounding_box(float t0, float t1, AABB& box) const {
    if (ptr->bounding_box(t0, t1, box)) {
        box = AABB(box.min() + offset, box.max() + offset);
        return true;
    } else {
        return false;
    }
}

class RotateY : public Hitable {
    public:
        RotateY(Hitable *p, float angle);
        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;
        virtual bool bounding_box(float t0, float t1, AABB& box) const {
            box = bBox; return hasBox;
        }
        Hitable *ptr;
        float sin_theta;
        float cos_theta;
        bool hasBox;
        AABB bBox;
};

RotateY::RotateY(Hitable *p, float angle) : ptr(p) {
    float radians = (M_PI / 180) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasBox = ptr->bounding_box(0, 1, bBox);
    Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i=0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k=0; k < 2; k++) {
                float x = i*bBox.max().x() + (1-i)*bBox.min().x();
                float y = j*bBox.max().y() + (1-j)*bBox.min().y();
                float z = k*bBox.max().z() + (1-k)*bBox.min().z();
                float newx = cos_theta*x + sin_theta*z;
                float newz = -sin_theta*x + cos_theta*z;
                Vector3 tester(newx, y, newz);
                for (int c = 0; c < 3; c++) {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
        }
    }
    bBox = AABB(min, max);
}

bool RotateY::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
    Vector3 origin = r.origin();
    Vector3 direction = r.direction();
    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
    Ray rotated_r(origin, direction, r.time());

    if (ptr->hit(rotated_r, t_min, t_max, rec)) {
        Vector3 p = rec.p;
        Vector3 normal = rec.normal;
        p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];
        normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    } else {
        return false;
    }
}

#endif
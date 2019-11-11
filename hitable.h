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
    Material *matPtr;
    float u, v;
};

class Hitable {
    public:
    virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;
    virtual bool boundingBox(float t0, float t1, AABB& box) const = 0;
};

int boxXCompare (const void * a, const void *b) {
    AABB boxLeft, boxRight;
    Hitable *ah = *(Hitable**)a;
    Hitable *bh = *(Hitable**)a;
    if (!ah->boundingBox(0,0, boxLeft) || !bh->boundingBox(0,0, boxRight))
        std::cerr << "no bounding box in bvh_onde constructor\n";
    if (boxLeft.min().x() - boxRight.min().x() < 0.0 )
        return -1;
    else 
        return 1;
}

int boxYCompare (const void * a, const void *b) {
    AABB boxLeft, boxRight;
    Hitable *ah = *(Hitable**)a;
    Hitable *bh = *(Hitable**)a;
    if (!ah->boundingBox(0,0, boxLeft) || !bh->boundingBox(0,0, boxRight))
        std::cerr << "no bounding box in bvh_onde constructor\n";
    if (boxLeft.min().y() - boxRight.min().y() < 0.0 )
        return -1;
    else 
        return 1;
}

int boxZCompare (const void * a, const void *b) {
    AABB boxLeft, boxRight;
    Hitable *ah = *(Hitable**)a;
    Hitable *bh = *(Hitable**)a;
    if (!ah->boundingBox(0,0, boxLeft) || !bh->boundingBox(0,0, boxRight))
        std::cerr << "no bounding box in bvh_onde constructor\n";
    if (boxLeft.min().z() - boxRight.min().z() < 0.0 )
        return -1;
    else 
        return 1;
}

class BVHNode : public Hitable {
    public:
        BVHNode() {}
        BVHNode(Hitable **l, int n , float time0, float time1);
        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const;
        Hitable *left;
        Hitable *right;
        AABB box;
};

bool BVHNode::boundingBox(float t0, float t1, AABB& b) const {
    b = box;
    return true;
}

BVHNode::BVHNode(Hitable **l, int n, float time0, float time1) {
    int axis = int(3*drand48());
    if (axis == 0)
        qsort(l, n, sizeof(Hitable *), boxXCompare);
    else if (axis == 1)
        qsort(l, n, sizeof(Hitable *), boxYCompare);
    else
        qsort(l, n, sizeof(Hitable *), boxZCompare);
    
    if (n == 1) {
        left = right = l[0];
    } else if (n == 2) {
        left = l[0];
        right = l[1];
    } else {
        left = new BVHNode(l, n/2, time0, time1);
        right = new BVHNode(l + n/2, n-n/2, time0, time1);
    }
    AABB boxLeft, boxRight;
    if(!left->boundingBox(time0, time1, boxLeft) || !right->boundingBox(time0,time1, boxRight))
        std::cerr << "no bounding box in BVHNode constructor\n";
    box = surroundingBox(boxLeft, boxRight);
}

bool BVHNode::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    if (box.hit(r, tMin, tMax)) {
        HitRecord leftRec, rightRec;
        bool hitLeft = left->hit(r, tMin, tMax, leftRec);
        bool hitRight = right->hit(r, tMin, tMax, rightRec);
        if (hitLeft && hitRight) {
            if (leftRec.t < rightRec.t)
                rec = leftRec;
            else
                rec = rightRec;
            return true;
        } else if (hitLeft) {
            rec = leftRec;
            return true;
        } else if (hitRight) {
            rec = rightRec;
            return true;
        } else
            return false;
    } else
        return false;
}

class FlipNormals : public Hitable {
    public:
        FlipNormals(Hitable *p) : ptr(p) {}
        virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
            if (ptr->hit(r, tMin, tMax, rec)) {
                rec.normal = -rec.normal;
                return true;
            } else {
                return false;
            }
        }
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            return ptr->boundingBox(t0, t1, box);
        }

        Hitable *ptr;
};

class Translate : public Hitable {
    public:
        Translate(Hitable *p, const Vector3& displacement) : ptr(p), offset(displacement) {}
        virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const;
        Hitable *ptr;
        Vector3 offset;
};

bool Translate::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    Ray movedR(r.origin() - offset, r.direction(), r.time());
    if (ptr->hit(movedR, tMin, tMax, rec)) {
        rec.p += offset;
        return true;
    } else {
        return false;
    }
}

bool Translate::boundingBox(float t0, float t1, AABB& box) const {
    if (ptr->boundingBox(t0, t1, box)) {
        box = AABB(box.min() + offset, box.max() + offset);
        return true;
    } else {
        return false;
    }
}

class RotateY : public Hitable {
    public:
        RotateY(Hitable *p, float angle);
        virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            box = bBox; return hasBox;
        }
        Hitable *ptr;
        float sinTheta;
        float cosTheta;
        bool hasBox;
        AABB bBox;
};

RotateY::RotateY(Hitable *p, float angle) : ptr(p) {
    float radians = (M_PI / 180) * angle;
    sinTheta = sin(radians);
    cosTheta = cos(radians);
    hasBox = ptr->boundingBox(0, 1, bBox);
    Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i=0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k=0; k < 2; k++) {
                float x = i*bBox.max().x() + (1-i)*bBox.min().x();
                float y = j*bBox.max().y() + (1-j)*bBox.min().y();
                float z = k*bBox.max().z() + (1-k)*bBox.min().z();
                float newx = cosTheta*x + sinTheta*z;
                float newz = -sinTheta*x + cosTheta*z;
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

bool RotateY::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    Vector3 origin = r.origin();
    Vector3 direction = r.direction();
    origin[0] = cosTheta*r.origin()[0] - sinTheta*r.origin()[2];
    origin[2] = sinTheta*r.origin()[0] + cosTheta*r.origin()[2];
    direction[0] = cosTheta*r.direction()[0] - sinTheta*r.direction()[2];
    direction[2] = sinTheta*r.direction()[0] + cosTheta*r.direction()[2];
    Ray rotatedR(origin, direction, r.time());

    if (ptr->hit(rotatedR, tMin, tMax, rec)) {
        Vector3 p = rec.p;
        Vector3 normal = rec.normal;
        p[0] = cosTheta*rec.p[0] + sinTheta*rec.p[2];
        p[2] = -sinTheta*rec.p[0] + cosTheta*rec.p[2];
        normal[0] = cosTheta*rec.normal[0] + sinTheta*rec.normal[2];
        normal[2] = -sinTheta*rec.normal[0] + cosTheta*rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    } else {
        return false;
    }
}

#endif
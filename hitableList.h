#ifndef HITABLELIST
#define HITABLELIST

#include "hitable.h"

class HitableList: public Hitable {
    public:
        HitableList() {}
        HitableList(Hitable **l, int n) {list = l; listSize = n; }
        virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const;
        Hitable **list;
        int listSize;
};

bool HitableList::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    HitRecord tempRec;
    bool hitAnything = false;
    double closestSoFar = tMax;
    for (int i = 0; i < listSize; i++) {
        if (list[i]->hit(r,tMin, closestSoFar, tempRec)) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }
    return hitAnything;
}

bool HitableList::boundingBox(float t0, float t1, AABB& box) const {
    if (listSize < 1 ) return false;

    AABB tempBox;
    bool firstTrue = list[0]->boundingBox(t0, t1, tempBox);
    if (!firstTrue)
        return false;
    else
        box = tempBox;

    for (int i = 1; i < listSize; i++) {
        if (list[0]->boundingBox(t0, t1, tempBox)) {
            box = surroundingBox(box, tempBox);
        } else
            return false;
    }
    return true;
}

#endif
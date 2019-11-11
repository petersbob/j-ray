#ifndef CMEDH
#define CMEDH

class ConstantMedium : public Hitable {
    public:
        ConstantMedium(Hitable *b, float d, Texture *a) : boundary(b), density(d) {
            phaseFunction = new Isotropic(a);
        }
        virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& box) const {
            return boundary->boundingBox(t0, t1, box);
        }

        Hitable *boundary;
        float density;
        Material *phaseFunction;
};

bool ConstantMedium::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const {
    bool db = (drand48() < 0.00001);
    db = false;

    HitRecord rec1, rec2;
    if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1)) {
        if (boundary->hit(r, rec1.t+0.0001, FLT_MAX, rec2)) {
            if (db) std::cerr << "t0 t1" << rec1.t << " " << rec2.t << std::endl;
            if (rec1.t < tMin)
                rec1.t = tMin;
            if (rec2.t > tMax)
                rec2.t = tMax;
            if (rec1.t >= rec2.t)
                return false;
            if (rec1.t < 0)
                rec1.t = 0;
            float distanceInsideBoundary = (rec2.t - rec1.t)*r.direction().length();
            float hitDistance = -(1/density)*log(drand48());
            if (hitDistance < distanceInsideBoundary) {
                if (db) std::cerr << "hitDistance = " << hitDistance << std::endl;
                rec.t = rec1.t + hitDistance / r.direction().length();
                if (db) std::cerr << "rec.t = " << rec.t << std::endl;
                rec.p = r.pointAtParameter(rec.t);
                if (db) std::cerr << "rec.p = " << rec.p << std::endl;
                rec.normal = Vector3(1,0,0); // arbitrary
                rec.matPtr = phaseFunction;
                return true;
            }
        }
    }
    return false;
}

#endif
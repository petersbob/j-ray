#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "hitable.h"
#include "texture.h"

float randomFloat() {
    // returns a random float [0, 1)
    return ((float) rand() / (RAND_MAX));
}

Vector3 randomInUnitSphere() {
    Vector3 p;
    do {
        p = 2.0*Vector3(randomFloat(),randomFloat(),randomFloat()) - Vector3(1,1,1);
    } while (p.squaredLength() >= 1.0);
    return p;
}

Vector3 reflect(const Vector3& v, const Vector3& n) {
    return v-2*dot(v,n)*n;
};

bool refract(const Vector3& v, const Vector3& n, float niOverNT, Vector3& refracted) {
    Vector3 uv = unitVector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - niOverNT*niOverNT*(1-dt*dt);
    if (discriminant > 0) {
        refracted = niOverNT*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    } else {
        return false;
    }
}

float schlick(float cosine, float refIDX) {
    float r0 = (1-refIDX) / (1+refIDX);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1-cosine),5);
}

class Material {
    public:
        virtual bool scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const = 0;
        virtual Vector3 emitted(float u, float v, const Vector3& p) const { return Vector3(0,0,0); }
};

class Lambertian : public Material {
    public:
        Lambertian(Texture *a) : albedo(a) {}
        virtual bool scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const {
            Vector3 target = rec.p + rec.normal + randomInUnitSphere();
            scattered = Ray(rec.p, target-rec.p, rIn.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

        Texture *albedo;
};

class Metal : public Material {
    public:
        Metal(const Vector3 a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1;}
        virtual bool scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const {
            Vector3 reflected = reflect(unitVector(rIn.direction()), rec.normal);
            scattered = Ray(rec.p, reflected + fuzz*randomInUnitSphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

        Vector3 albedo;
        float fuzz;
};

class Dielectric : public Material {
public:
        Dielectric(float ri) : refIDX(ri) {}
        virtual bool scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const {
            Vector3 outwardNormal;
            Vector3 reflected = reflect(rIn.direction(), rec.normal);
            float niOverNT;
            attenuation = Vector3(1.0,1.0,1.0);
            Vector3 refracted;
            float reflectProb;
            float cosine;
            if (dot(rIn.direction(), rec.normal) > 0) { // there is less than 180 between them
                outwardNormal = -rec.normal;
                niOverNT = refIDX;
                cosine = refIDX * dot(rIn.direction(), rec.normal) / rIn.direction().length();
            } else { // more than 180 degrees between them
                outwardNormal = rec.normal;
                niOverNT = 1.0 / refIDX;
                cosine = -dot(rIn.direction(), rec.normal) / rIn.direction().length();
            }
            if (refract(rIn.direction(), outwardNormal, niOverNT, refracted)) {
                reflectProb = schlick(cosine, refIDX);
            } else {
                scattered = Ray(rec.p, reflected);
                reflectProb = 1.0;
            }
            if (randomFloat() < reflectProb) {
                scattered = Ray(rec.p, reflected);
            } else {
                scattered = Ray(rec.p, refracted);
            }
            return true;
        }

        float refIDX;
};

class DiffuseLight : public Material {
    public:
        DiffuseLight(Texture *a) : emit(a) {}
        virtual bool scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const { return false; }
        virtual Vector3 emitted(float u, float v, const Vector3& p) const {
            return emit->value(u, v, p);
        }
        Texture *emit;
};

class Isotropic : public Material {
    public:
        Isotropic(Texture *a) : albedo(a) {}
        virtual bool scatter(const Ray& rIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const {
            scattered = Ray(rec.p, randomInUnitSphere());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
        Texture *albedo;
};

#endif
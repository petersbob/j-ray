#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

Vector3 randomInUnitDisk() {
    Vector3 p;
    do {
        p = 2.0*Vector3(randomFloat(), randomFloat(), 0) - Vector3(1,1,0);
    } while (dot(p,p) >= 1.0);
    return p;
}

class Camera {
    public:
        Camera(Vector3 lookfrom, Vector3 lookat, Vector3 vup, float vfov, float aspect, float aperture, float focusDist, float t0, float t1) { // vfov is top to bottom in degrees
            time0 = t0;
            time1 = t1;
            lensRadius = aperture / 2;
            float theta = vfov*M_PI/180;
            float halfHeight = tan(theta/2);
            float halfWidth = aspect * halfHeight;
            origin = lookfrom;
            w = unitVector(lookfrom - lookat);
            u = unitVector(cross(vup, w));
            v = cross(w, u);
            lowerLeftCorner = origin - halfWidth*focusDist*u  - halfHeight*focusDist*v - focusDist*w;
            horizontal = 2*halfWidth*focusDist*u;
            vertical = 2*halfHeight*focusDist*v;
        }
        Ray getRay(float s, float t) {
            Vector3 rd = lensRadius*randomInUnitDisk();
            Vector3 offset = u * rd.x() + v * rd.y();
            float time = time0 + (drand48()* (time1-time0));
            return Ray(origin + offset, lowerLeftCorner+s*horizontal + t*vertical - origin - offset, time);
        }

        Vector3 origin;
        Vector3 lowerLeftCorner;
        Vector3 horizontal;
        Vector3 vertical;
        Vector3 u, v, w;
        float time0, time1;
        float lensRadius;
};

#endif
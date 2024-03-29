#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "ray.h"
#include "sphere.h"
#include "rectangle.h"
#include "box.h"
#include "hitableList.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "parallel.h"
#include "constantMedium.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Options {
    std::string fileName = "image.jpg";
    int nSamples = 1;
    int xResolution = 600;
    int yResolution = 300;
};

Vector3 color(const Ray& r, Hitable *world, int depth) {
    HitRecord rec;
    if (world->hit(r, 0.001,FLT_MAX, rec)) {
        Ray scatteredRay;
        Vector3 attenuation = Vector3(0.5,0.5,0.5);
        Vector3 emitted = rec.matPtr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.matPtr->scatter(r, rec, attenuation, scatteredRay)) {
            return emitted + attenuation*color(scatteredRay, world, depth+1);
        } else {
            return emitted;
        }
    } else {
        return Vector3(0,0,0);
    }
}

Hitable *randomScene(unsigned char **texData) {
    Vector3 colors[6] = {
            Vector3(0.37,0.62,0.58),
            Vector3(0.24,0.21,0.22),
            Vector3(0.45,0.21,0.20),
            Vector3(0.71,0.38,0.22),
            Vector3(0.69,0.63,0.64),
            Vector3(0.89,0.85,0.82),
    };

    int n = 500;
    Hitable **list = new Hitable*[n+1];
    list[0] =  new Sphere(Vector3(0,-1000,0), 1000, new DiffuseLight(new ConstantTexture(Vector3(1.1,1.1,1.1))));

    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float chooseMat = drand48();
            Vector3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
            Vector3 color;
            color = colors[ int(drand48()*5) ];

            if ((center-Vector3(4,0.2,0)).length() > 0.9) { 
                if (chooseMat < 0.3) {  // diffuse
                    list[i++] = new Sphere(center, 0.2, new Lambertian(new ConstantTexture(color)));
                }
                else if (chooseMat < 0.6) { // metal
                    list[i++] = new Sphere(center, 0.2, new Metal(Vector3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())),  0.5*drand48()));
                }
                else {  // glass
                    list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new Sphere(Vector3(0, 1, 0), 1.0, new Dielectric(1.5));

    int nx, ny, nn;
    *texData = stbi_load("textures/earth.jpg", &nx, &ny, &nn, 0);
    if (texData == NULL) {
        std::cout << "Error: texture could not be loaded!" << std::endl;
        return NULL;
    }

    Material *mat = new Lambertian(new ImageTexture(*texData, nx, ny));
    list[i++] = new Sphere(Vector3(4, 1, 0), 1.0, mat);

    list[i++] = new Sphere(Vector3(-4, 1, 0), 1.0, new Metal(colors[4], 0.0));
    return new BVHNode(list,i,0.0, 1.0);
}

Hitable *cornellBox() {
    Hitable **list = new Hitable*[6];
    int i = 0;
    Material *white = new Lambertian(new ConstantTexture(Vector3(0.73, 0.73, 0.73)));

    list[i++] = new FlipNormals(new YZRect(0, 700, 0, 700, 700, white));
    list[i++] = new YZRect(0, 700, 0, 700, -700, white);
    list[i++] = new FlipNormals(new XZRect(-700, 700, -700, 700, 700, white));
    list[i++] = new XZRect(-700, 700, -700, 700, 0, white);
    list[i++] = new FlipNormals(new XYRect(-700, 700, 0, 700, 700, white));

    return new HitableList(list,i);
}

Hitable *final() {
    Hitable **list = new Hitable*[500];
    int count = 0;
    Material *red = new Lambertian( new ConstantTexture(Vector3(0.65, 0.05, 0.05)) );
    Material *white = new Lambertian( new ConstantTexture(Vector3(0.73, 0.73, 0.73)) );
    Material *green = new Lambertian( new ConstantTexture(Vector3(0.12, 0.45, 0.15)) );
    Material *light = new DiffuseLight( new ConstantTexture(Vector3(15, 15, 15)) );

    list[count++] = cornellBox();

    list[count++] = new Sphere(Vector3(0,0,0), 50, red);
    
    // for (int i=0; i < 6; i++) {
    //     for (int j = 0; j < 6; j++) {
    //         float x = (drand48()*900)-450;
    //         float y = drand48()*700;  
    //         float z = (drand48()*900)-450;

    //         list[count++] = new Sphere(Vector3(x,y,z), 50, white);
    //     }
    // }

    // for (int i=0; i < 28; i++) {

    //     list[count++] = new Box(
    //         Vector3(650-(50*i),0,400-drand48()*100),
    //         Vector3(700-(50*i),100+drand48()*200,700),
    //         green
    //     );

    // }

    //list[count++] = new ConstantMedium(cornellBox(), 0.01, new ConstantTexture(Vector3(1.0, 1.0, 1.0)));

    list[count++] = new XZRect(-200, 200, 0, 200, 554, light);

    return new HitableList(list, count);
}

int main(int argc, char *argv[]) {
    Options options;

    for (int i=1; i<argc;i++) {
        std::string argString = argv[i];
        if (argString.substr(0,11) == "--fileName="){
            options.fileName = argString.substr(11,argString.length());
        } else if (argString.substr(0,11) == "--nSamples=") {
            options.nSamples = stoi(argString.substr(11,argString.length()));
        } else if (argString.substr(0,14) == "--xResolution=") {
            options.xResolution = stoi(argString.substr(14,argString.length()));
        } else if (argString.substr(0,14) == "--yResolution=") {
            options.yResolution = stoi(argString.substr(14,argString.length()));
        } else {
            std::cout << "Error: parameter \"" << argString << "\" unknown!" << std::endl;
            return 0;
        }
    }

    srand(time(0));

    std::cout<< "Samples: " << options.nSamples << std::endl;
    std::cout<< "Resolution " << options.xResolution << " " << options.yResolution << std::endl;
    std::cout<< "Creating image " << options.fileName << "..." << std::endl;

    unsigned char *texData;
    //Hitable *world = randomScene(&texData);
    Hitable *world = final();
    // if (texData == NULL || world == NULL) {
    //     std::cout << "Error: creating scene has failed" << std::endl;
    //     return 0;
    // }

    Vector3 lookfrom(0,278,-800);
    Vector3 lookat(0,278,0);
    float distToFocus = 10;
    float aperture = 0.0;

    Camera cam(lookfrom, lookat, Vector3(0,1,0), 40, float(options.xResolution)/float(options.yResolution), aperture, distToFocus, 0, 1);

    char* image;
    image = new char[options.xResolution*options.yResolution*3];

    parallelForEach(0, options.yResolution, [=,&image,&cam](int j){
        for (int i=0; i < options.xResolution; i++) {
                Vector3 col(0,0,0);
                for (int s=0; s < options.nSamples; s++) {
                    float u = float(i + randomFloat()) / float(options.xResolution);
                    float v = float(j + randomFloat()) / float(options.yResolution);
                    Ray r = cam.getRay(u, v);
                    Vector3 p = r.pointAtParameter(2.0);
                    col += color(r, world, 0);
                }
                col /= float(options.nSamples);
                col = Vector3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

                image[(j*options.xResolution*3) + (i*3)] = char(255.99*col[0]);
                image[(j*options.xResolution*3) + (i*3+1)] = int(255.99*col[1]);
                image[(j*options.xResolution*3) + (i*3+2)] = int(255.99*col[2]);
            }
    });

    // stbi_image_free(texData);

    int fileNameSize = options.fileName.size();
    char cFileName[fileNameSize+1];
    options.fileName.copy(cFileName, fileNameSize + 1);
    cFileName[fileNameSize] = '\0';
    
    stbi_flip_vertically_on_write(1);
    int success = stbi_write_jpg(cFileName, options.xResolution, options.yResolution, 3, image, 100);
    if (!success) {
        std::cout << "Error: writing to file failed!" << std::endl;
    }
}
#ifndef TEXTUREH
#define TEXTUREH

inline float trilinearInterp(float c[2][2][2], float u, float v, float w) {
    float accum = 0;
    for (int i=0; i< 2; i++)
        for (int j=0; j< 2; j++)
            for (int k=0; k< 2; k++)
                accum += (i*u + (1-i)*(1-u))*
                        (j*v + (1-j)*(1-v))*
                        (k*w + (1-k)*(1-w))*c[i][j][k];

    return accum;
}

class Perlin {
    public:
    // TODO: implement perlin noise
        float noise(const Vector3& p) const {
            float u = p.x() - floor(p.x());
            float v = p.y() - floor(p.y());
            float w = p.z() - floor(p.z());
            u = u*u*(3-2*u);
            v = v*v*(3-2*v);
            w = w*w*(3-2*w);
            int i = floor(p.x());
            int j = floor(p.y());
            int k = floor(p.z());
            float c[2][2][2];
            for (int di=0; di < 2; di++)
                for (int dj=0; dj < 2; dj++)
                    for (int dk=0; dk < 2; dk++)
                        c[di][dj][dk] = ranfloat[permX[(i+di)&255] ^ permX[(j+dj)&255] ^ permX[(k+dk)&255]];
            return trilinearInterp(c, u, v, w);
        }
        static float *ranfloat;
        static int *permX;
        static int *permY;
        static int *permZ;
};

static float *perlinGenerate() {
    float *p = new float[256];
    for (int i=0; i < 256; ++i) {
        p[i] = drand48();
    }
    return p;
}

void permute(int *p, int n){
    for (int i=n-1; i > 0; i--) {
        int target = int(drand48()*(i+1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
    return;
}

static int* perlinGeneratePerm() {
    int * p = new int[256];
    for (int i=0; i < 256; i++) {
        p[i] = i;
    }
    permute(p, 256);
    return p;
}

class Texture {
    public:
        virtual Vector3 value(float u, float v, const Vector3& p) const = 0;
};

class ConstantTexture : public Texture {
    public:
        ConstantTexture() { }
        ConstantTexture(Vector3 c) : color(c) { }
        virtual Vector3 value(float u, float v, const Vector3& p) const {
            return color;
        }
        Vector3 color;
};

class CheckerTexture : public Texture {
    public:
        CheckerTexture() { }
        CheckerTexture(Texture *t0, Texture *t1) : even(t0), odd(t1) { }
        virtual Vector3 value(float u, float v, const Vector3& p) const {
            float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
            if (sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }

        Texture *odd;
        Texture *even;
};

class NoiseTexture : public Texture {
    public:
        NoiseTexture() {}
        NoiseTexture(float sc) : scale(sc) {}
        virtual Vector3 value(float u, float v, const Vector3& p) const {
            return Vector3(1,1,1)*noise.noise(p*scale);
        }
        Perlin noise;
        float scale;
};

class ImageTexture : public Texture {
    public:
        ImageTexture() {}
        ImageTexture(unsigned char *pixels, int A, int B) : data(pixels), nx(A), ny(B) {}
        virtual Vector3 value(float u, float v, const Vector3& p) const;
        unsigned char *data;
        int nx, ny;
};

Vector3 ImageTexture::value(float u, float v, const Vector3& p) const {
    int i = (u)*nx;
    int j = (1-v)*ny-0.001;
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i > nx-1) i = nx-1;
    if (j > ny-1) j = ny-1;
    float r = int(data[3*i + 3*nx*j] ) / 255.0;
    float g = int(data[3*i + 3*nx*j + 1] ) / 255.0;
    float b = int(data[3*i + 3*nx*j + 2] ) / 255.0;
    return Vector3(r, g, b);
}

float * Perlin::ranfloat = perlinGenerate();
int *Perlin::permX = perlinGeneratePerm();
int *Perlin::permY = perlinGeneratePerm();
int *Perlin::permZ = perlinGeneratePerm();

#endif
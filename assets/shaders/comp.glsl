#version 460

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform uint rndUniform;
uniform vec3 up;
uniform vec3 right;
uniform vec3 front;
uniform vec3 origin;

// ---- Helper Functions
uint seed = rndUniform;
vec2 id = gl_GlobalInvocationID.xy;

float randFloat(){
    seed++;
    
    uint m = uint(id.x) * 1664525u + uint(id.y) * 1013904223u + seed;
    m ^= (m >> 16);
    m *= 0x85ebca6bu;
    m ^= (m >> 13);
    m *= 0xc2b2ae35u;
    m ^= (m >> 16);
    return fract(float(m) / float(0xffffffffu));
}

vec2 randVec2() {
    return vec2(randFloat(), randFloat());
}

vec3 randVec3() {
    return vec3(randFloat(), randFloat(), randFloat());
}

vec3 randUnitVec() {
    while (true) {
        vec3 point = randVec3() * 2.0 - 1.0;
        float distSquare = dot(point, point);

        if (0 < distSquare) {
            return normalize(point);
        }
    }
}

// ---- Ray Structures
struct Ray {
    vec3 origin;
    vec3 direction;
};

const int MAT_LAMBERTIAN = 0;
const int MAT_METAL = 1;
const int MAT_DIELECTRIC = 2;

struct Material {
    int type;
    vec3 albedo;
    float fuzz;
    float refractiveIdx;
};

Material Lambertian(vec3 a) {
    Material mat;
    mat.type = MAT_LAMBERTIAN;
    mat.albedo = a;
    return mat;
}

Material Metal(vec3 a, float f) {
    Material mat;
    mat.type = MAT_METAL;
    mat.albedo = a;
    mat.fuzz = f;
    return mat;
}

Material Dielectric(float r) {
    Material mat;
    mat.type = MAT_DIELECTRIC;
    mat.refractiveIdx = r;
    return mat;
}

struct RayRecord {
    float time;
    vec3 point;
    vec3 normal;
    Material material;
};

vec3 lookAt(Ray ray, float t) {
    return ray.origin + t * ray.direction;
}

// ---- Shape Objects + constructors
const int SPHERE_TYPE = 0;

struct Object {
    int type;
    vec3 center;
    float radius;
    Material material;
};

Object Sphere(vec3 center, float radius, Material mat) {
    Object o;
    o.type = SPHERE_TYPE;
    o.center = center;
    o.radius = radius;
    o.material = mat;
    return o;
}

// ---- World State
Object world[] = {
    Sphere(vec3( 0, -100.5, -1.0), 100, Metal(vec3(0.9, 0.8, 0.9), 0.01)),
    Sphere(vec3( 0,      0,  -1.2), 0.5, Lambertian(vec3(0.1, 0.2, 0.5))),
    Sphere(vec3( 1,      0,  -1.0), 0.5, Metal(vec3(0.8, 0.6, 0.2), 0.9)),
    Sphere(vec3(-1,      0,  -1.0), 0.5, Dielectric(1.5)),
    Sphere(vec3(-1,      0,  -1.0), 0.4, Dielectric(1.0 / 1.5)),

    Sphere(vec3( 2,      0.3, -1.5), 0.3, Lambertian(vec3(0.7, 0.1, 0.1))),
    Sphere(vec3(-2,      0.2, -1.5), 0.2, Metal(vec3(0.9, 0.9, 0.9), 0.1)),
    Sphere(vec3( 0.5,    0.5, -2.0), 0.5, Lambertian(vec3(0.2, 0.8, 0.3))),
    Sphere(vec3(-0.5,    0.5, -2.0), 0.5, Metal(vec3(0.6, 0.7, 0.9), 0.3)),
    Sphere(vec3( 1.2,    0.4, -2.5), 0.4, Dielectric(1.3)),

    Sphere(vec3( 2.5,    0.6, -3.0), 0.6, Lambertian(vec3(0.9, 0.4, 0.2))),
    Sphere(vec3(-2.5,    0.3, -3.0), 0.3, Metal(vec3(0.7, 0.7, 0.7), 0.0)),
    Sphere(vec3( 0,      0.3, -3.5), 0.3, Lambertian(vec3(0.3, 0.5, 0.9))),
    Sphere(vec3( 1.5,    0.5, -3.5), 0.5, Dielectric(1.5)),
    Sphere(vec3(-1.5,    0.4, -3.5), 0.4, Metal(vec3(0.9, 0.8, 0.7), 0.05)),

    Sphere(vec3( 0.8,    0.2, -4.0), 0.2, Lambertian(vec3(0.8, 0.1, 0.5))),
    Sphere(vec3(-0.8,    0.2, -4.0), 0.2, Lambertian(vec3(0.1, 0.8, 0.6))),
    Sphere(vec3( 2.0,    0.3, -4.2), 0.3, Metal(vec3(0.6, 0.5, 0.7), 0.2)),
    Sphere(vec3(-2.0,    0.3, -4.2), 0.3, Metal(vec3(0.9, 0.9, 0.4), 0.3)),
    Sphere(vec3( 0,      0.4, -4.5), 0.4, Dielectric(1.4)),

    Sphere(vec3( 3.0,    0.5, -5.0), 0.5, Lambertian(vec3(0.4, 0.7, 0.2))),
    Sphere(vec3(-3.0,    0.5, -5.0), 0.5, Metal(vec3(0.5, 0.5, 0.9), 0.05)),
    Sphere(vec3( 1.0,    0.4, -5.5), 0.4, Lambertian(vec3(0.2, 0.6, 0.9))),
    Sphere(vec3(-1.0,    0.4, -5.5), 0.4, Dielectric(1.6)),
    Sphere(vec3( 2.2,    0.3, -5.8), 0.3, Metal(vec3(0.7, 0.6, 0.5), 0.15)),

    Sphere(vec3(-2.2,    0.3, -5.8), 0.3, Lambertian(vec3(0.9, 0.2, 0.2))),
    Sphere(vec3( 0.6,    0.2, -6.0), 0.2, Lambertian(vec3(0.3, 0.7, 0.4))),
    Sphere(vec3(-0.6,    0.2, -6.0), 0.2, Metal(vec3(0.6, 0.8, 0.9), 0.1)),
    Sphere(vec3( 1.5,    0.5, -6.2), 0.5, Dielectric(1.3)),
    Sphere(vec3(-1.5,    0.5, -6.2), 0.5, Lambertian(vec3(0.5, 0.5, 0.5)))
};


// Scattering properties
vec3 reflect(vec3 v, vec3 norm) {
    return v - 2 * dot(v, norm) * norm;
}

vec3 refract(vec3 uv, vec3 n, float ratio) {
    float theta = min(dot(-uv, n), 1.0);

    vec3 outPerp = ratio * (uv + theta*n);
    vec3 outPara = -sqrt(abs(1 - dot(outPerp, outPerp))) * n;
    return outPerp + outPara;
}

float schlick(float cosine, float refIdx) {
    float r0 = (1 - refIdx) / (1 + refIdx);
    r0 *= r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

bool scatter(Ray inRay, inout Ray scatterRay, inout RayRecord rec, inout vec3 attenuation) {
    switch (rec.material.type) {
        case MAT_LAMBERTIAN: {
            vec3 direction = rec.normal + randUnitVec();
            scatterRay = Ray(rec.point, direction);
            attenuation *= rec.material.albedo;
            return true;
        }
        case MAT_METAL: {
            vec3 reflected = reflect(normalize(inRay.direction), rec.normal);
            scatterRay = Ray(rec.point, reflected + rec.material.fuzz * randUnitVec());
            attenuation *= rec.material.albedo;
            return (dot(scatterRay.direction, rec.normal) > 0);
        }
        case MAT_DIELECTRIC: {
            bool frontFace = dot(inRay.direction, rec.normal) < 0;
            rec.normal = frontFace ? rec.normal : -rec.normal;
            vec3 unitDir = normalize(inRay.direction);

            float cosTheta = min(dot(-unitDir, rec.normal), 1.0);
            float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

            float ri = frontFace ? (1.0/rec.material.refractiveIdx) : rec.material.refractiveIdx;

            if ((ri * sinTheta > 1.0) || schlick(cosTheta, ri) > randFloat()) {
                vec3 reflected = reflect(unitDir, rec.normal);
                scatterRay = Ray(rec.point, reflected + rec.material.fuzz * randUnitVec());
            } else {
                vec3 refracted = refract(unitDir, rec.normal, ri);
                scatterRay = Ray(rec.point, refracted);
            }

            return true;
        }
    }
    return false;
}

// ---- Object Intersection
bool intersect(Object o, Ray r, inout RayRecord rec, float tMin, float tMax) {
    switch (o.type) {
        case SPHERE_TYPE:
            vec3 oc = r.origin - o.center;

            float a = dot(r.direction, r.direction);
            float b = dot(oc, r.direction);
            float c = dot(oc, oc) - o.radius*o.radius;
            float discriminant = b*b - a*c;

            if (discriminant > 0) {
                float t = (-b - sqrt(discriminant)) / a;

                if (t < tMin || t > tMax) {
                    t = (-b + sqrt(discriminant)) / a;
                }

                if (t > tMin && t < tMax) {
                    rec.time = t;
                    rec.point = lookAt(r, t);
                    rec.normal = (rec.point - o.center) / o.radius;
                    rec.material = o.material;
                    return true;
                }
            }

            return false;
    }
    return false;
}

bool findIntersect(Ray r, inout RayRecord rec, float tMin, float tMax) {
    bool hitFound = false;
    float closest = tMax;

    for (int i=0; i < world.length(); i++) {
        if (intersect(world[i], r, rec, tMin, closest)) {
            hitFound = true;
            closest = rec.time; 
        }
    }
    
    return hitFound;
}

// ---- Get the color
vec3 color(Ray ray) {
    vec3 attenuation = vec3(1.0);
    const int maxBounces = 30;

    for (int i = 0; i < maxBounces; ++i) {
        RayRecord rec;

        if (findIntersect(ray, rec, 0.001, 3.402823466e+38)) {
            if (!scatter(ray, ray, rec, attenuation)) {
                break;
            }
        } else {
            vec3 dir_norm = normalize(ray.direction);
            float t = 0.5 * (dir_norm.y + 1.0);
            return attenuation * ((1.0 - t) + t * vec3(0.5, 0.7, 1));
        }
    }

    return vec3(0, 0, 0);
}

struct Camera {
    ivec2 pixel;
    ivec2 res;
    vec2 aspect;
    vec3 up;
    vec3 right;
    vec3 front;
};

Camera initCamera() {
    Camera c;

    c.pixel = ivec2(id);
    c.res = imageSize(imgOutput);

    float aspect = float(c.res.x) / float(c.res.y);
    
    if (aspect > 0) {
        c.aspect = vec2(1, 1.0/aspect);
    } else {
        c.aspect = vec2(aspect, 1.0);
    }

    return c;
}

vec3 getDirection(Camera c, vec2 random) {
    vec2 uv = (vec2(c.pixel + random) / vec2(c.res)) * 2.0 - 1.0;
    uv *= c.aspect;
    
    return uv.x * right + uv.y * up + front;
}

void main() {
    Camera camera = initCamera();

    vec3 result = vec3(0);

    int samples = 25;
    for (int i=0; i < samples; i++) {
        Ray ray = Ray(origin, getDirection(camera, randVec2()));
        result += color(ray);
    }

    result /= float(samples);
    result = sqrt(result);

    imageStore(imgOutput, camera.pixel, vec4(result, 1.0));
}
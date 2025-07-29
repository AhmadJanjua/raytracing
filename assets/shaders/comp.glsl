#version 460

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform uint rndUniform;

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

struct Material {
    int type;
    vec3 albedo;
    float fuzz;
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
    Sphere(vec3( 0,      0, -1), 0.5, Lambertian(vec3(0.8, 0.3, 0.3))),
    Sphere(vec3( 0, -100.5, -1), 100, Lambertian(vec3(0.8, 0.8, 0.0))),
    Sphere(vec3( 1,      0, -1), 0.5, Metal(vec3(0.8, 0.6, 0.2), 0.9)),
    Sphere(vec3(-1,      0, -1), 0.5, Metal(vec3(0.8, 0.8, 0.8), 0.1)),
};

// Scattering properties
vec3 reflect(vec3 v, vec3 norm) {
    return v - 2 * dot(v, norm) * norm;
}

bool scatter(Ray inRay, inout Ray scatterRay, inout RayRecord rec, inout vec3 attenuation) {
    switch (rec.material.type) {
        case MAT_LAMBERTIAN:
            vec3 direction = rec.normal + randUnitVec();
            scatterRay = Ray(rec.point, direction);
            attenuation *= rec.material.albedo;
            return true;
        case MAT_METAL:
            vec3 reflected = reflect(normalize(inRay.direction), rec.normal);
            scatterRay = Ray(rec.point, reflected + rec.material.fuzz * randUnitVec());
            attenuation *= rec.material.albedo;
            return (dot(scatterRay.direction, rec.normal) > 0);
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
    const int maxBounces = 10;

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

// Cast ray and anti-alias the color results
void main() {
    ivec2 curPixelPos = ivec2(id);
    ivec2 maxPixelPos = imageSize(imgOutput);
    float aspect = float(maxPixelPos.x) / float(maxPixelPos.y);
    vec3 origin = vec3(0);

    int samples = 50;
    vec3 result = vec3(0);
    for (int i=0; i < samples; i++) {
        vec2 random = randVec2();
        vec2 uv = ((vec2(curPixelPos + random)) / vec2(maxPixelPos)) * 2.0 - 1.0;
        vec2 screenPos = vec2(uv.x * aspect, uv.y);

        // Ray
        vec3 direction = vec3(screenPos, -1.0);
        Ray ray = Ray(origin, direction);
        result += color(ray);
    }

    result /= float(samples);
    result = sqrt(result); // gamma correction

    imageStore(imgOutput, curPixelPos, vec4(result, 1.0));
}
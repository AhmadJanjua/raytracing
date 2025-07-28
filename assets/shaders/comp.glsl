#version 460

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;


// ---- Helper Functions
float PHI = 1.61803398874989484820459;  

float rand(vec2 xy, float seed){
    return fract(sin(distance(xy*PHI, xy)*seed)*xy.x);
}

// ---- Ray Structures
struct Ray {
    vec3 origin;
    vec3 direction;
};

struct RayRecord {
    float time;
    vec3 point;
    vec3 normal;
};

vec3 lookAt(Ray ray, float t) {
    return ray.origin + t * ray.direction;
}

// ---- Shape Objects + constructors
int SPHERE_TYPE = 0;

struct Object {
    int type;
    vec3 center;
    float radius;
};

Object Sphere(vec3 center, float radius) {
    Object o;
    o.type = SPHERE_TYPE;
    o.center = center;
    o.radius = radius;
    return o;
}

// ---- World State
Object world[] = {
    Sphere(vec3(0, 0, -1), 0.5),
    Sphere(vec3(0, -100.5, -1), 100)
};

// ---- Object Intersection
bool intersectSphere(Object o, Ray r, inout RayRecord rec, float tMin, float tMax) {
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
            return true;
        }
    }

    return false;
}

bool intersect(Object o, Ray r, inout RayRecord rec, float tMin, float tMax) {
    switch (o.type) {
        case 0: return intersectSphere(o, r, rec, tMin, tMax);
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
    RayRecord rec;

    if (findIntersect(ray, rec, 0, 3.402823466e+38)) {
        return 0.5 * (rec.normal + 1);
    }

    vec3 dir_norm = normalize(ray.direction);
    float t = 0.5 * (dir_norm.y + 1.0);
    return (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1);
}

// Cast ray and anti-alias the color results
void main() {
    vec2 co = gl_GlobalInvocationID.xy;
    ivec2 curPixelPos = ivec2(co);
    ivec2 maxPixelPos = imageSize(imgOutput);
    float aspect = float(maxPixelPos.x) / float(maxPixelPos.y);
    vec3 origin = vec3(0);

    // map pixel [0,1] -> [-1, 1] + anti-aliasing
    int samples = 50;
    vec3 result = vec3(0);
    for (int i=0; i < samples; i++) {
        vec2 random = vec2(
            rand(co, 1.23 + float(i)),
            rand(co, 0.71 + float(i))
        );
        vec2 uv = ((vec2(curPixelPos + random)) / vec2(maxPixelPos)) * 2.0 - 1.0;
        vec2 screenPos = vec2(uv.x * aspect, uv.y);

        // Ray
        vec3 direction = vec3(screenPos, -1.0);
        Ray ray = Ray(origin, direction);
        result += normalize(color(ray));
    }
    result /= float(samples);

    imageStore(imgOutput, curPixelPos, vec4(result, 1.0));
}
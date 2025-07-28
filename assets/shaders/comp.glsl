#version 460

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct Ray {
    vec3 origin;
    vec3 direction;
};

vec3 lookAt(Ray ray, float t) {
    return ray.origin + t * ray.direction;
}

float hitSphere(Ray ray, vec3 center, float radius) {
    vec3 oc = ray.origin - center;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4 * a * c;

    if (discriminant < 0) {
        return -1;
    } else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

vec3 color(Ray ray) {
    float t = hitSphere(ray, vec3(0, 0, -1), 0.5);
    if (t > 0) {
        vec3 norm = normalize(lookAt(ray, t) - vec3(0, 0, -1));
        return 0.5 * (norm  + 1.0);
    }

    vec3 dir_norm = normalize(ray.direction);
    t = 0.5 * (dir_norm.y + 1.0);
    return (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1);
}


void main() {
    vec2 pixelPos = vec2(gl_GlobalInvocationID.xy);
    vec2 pixelSize = vec2(imageSize(imgOutput));
    vec2 uv = pixelPos / pixelSize;

    vec3 lowerLeftCorner = vec3(-2.0, -1.0, -1.0);
    vec2 ratios = vec2(4.0, 4.0 * (pixelSize.y / pixelSize.x));
    vec3 origin = vec3(0, 0, 0);

    Ray r = {origin, lowerLeftCorner + vec3(uv * ratios, 0.0)};
    vec3 value = color(r);

    imageStore(imgOutput, ivec2(pixelPos), vec4(value, 1.0));
}
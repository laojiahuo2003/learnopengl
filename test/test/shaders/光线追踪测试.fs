#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float reflectivity;

struct Sphere {
    vec3 center;
    float radius;
    float reflectivity;
    float transparency;
};

uniform int numSpheres;
uniform Sphere spheres[10];

#define MAX_REFLECTION_DEPTH 5

// Calculate reflection vector
vec3 reflect(vec3 I, vec3 N) {
    return I - 2.0 * dot(N, I) * N;
}

// Check ray-sphere intersection
bool intersectRaySphere(vec3 rayOrigin, vec3 rayDir, Sphere sphere, out float t0, out vec3 normal) {
    vec3 L = sphere.center - rayOrigin;
    float tca = dot(L, rayDir);
    float d2 = dot(L, L) - tca * tca;
    float radius2 = sphere.radius * sphere.radius;
    if (d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    float t1 = tca + thc;
    if (t0 < 0.0) t0 = t1;
    if (t0 < 0.0) return false;
    normal = normalize((rayOrigin + t0 * rayDir) - sphere.center);
    return true;
}

vec3 traceRay(vec3 rayOrigin, vec3 rayDir, int depth) {
    if (depth >= MAX_REFLECTION_DEPTH) return vec3(0.0);

    float nearest_t = 10000.0;
    vec3 hitNormal;
    Sphere hitSphere;
    bool hit = false;

    for (int i = 0; i < numSpheres; ++i) {
        float t0;
        vec3 normal;
        if (intersectRaySphere(rayOrigin, rayDir, spheres[i], t0, normal)) {
            if (t0 < nearest_t) {
                nearest_t = t0;
                hitNormal = normal;
                hitSphere = spheres[i];
                hit = true;
            }
        }
    }

    if (!hit) return vec3(0.1, 0.1, 0.1); // Background color

    vec3 hitPoint = rayOrigin + nearest_t * rayDir;
    vec3 viewDir = normalize(viewPos - hitPoint);
    vec3 reflectDir = reflect(rayDir, hitNormal);
    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = max(dot(hitNormal, normalize(lightPos - hitPoint)), 0.0) * lightColor;
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specularStrength * lightColor;
    vec3 localColor = (ambient + diffuse + specular) * objectColor;

    vec3 reflectionColor = traceRay(hitPoint, reflectDir, depth + 1);
    return mix(localColor, reflectionColor, hitSphere.reflectivity);
}

void main() {
    vec3 rayOrigin = viewPos;
    vec3 rayDir = normalize(FragPos - viewPos);
    vec3 color = traceRay(rayOrigin, rayDir, 0);
    FragColor = vec4(color, 1.0);
}

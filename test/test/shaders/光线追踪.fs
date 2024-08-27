#version 330 core
out vec4 FragColor;

in vec3 FragPos; // 从顶点着色器传来的顶点位置
in vec3 Normal;  // 从顶点着色器传来的法向量

uniform vec3 lightPos;//光源位置
uniform vec3 viewPos;//摄像机位置
uniform vec3 lightColor;//灯光颜色
uniform vec3 objectColor;//物体颜色

uniform float ambientStrength; // 环境光强度
uniform float specularStrength; // 镜面反射强度
uniform float reflectivity; // 反射强度

struct Sphere {
    vec3 center;
    float radius;
};

uniform Sphere spheres[10]; // 假设最多有10个球体
uniform int numSpheres; // 实际球体数量

// 计算光照
vec3 calculateLighting(vec3 hitPos, vec3 hitNormal, vec3 viewDir) {
    // 环境光强度
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射
    vec3 norm = normalize(hitNormal);
    vec3 lightDir = normalize(lightPos - hitPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    return ambient + diffuse + specular;
}

// 检测光线与球体的交点
bool rayIntersectsSphere(vec3 rayOrigin, vec3 rayDir, Sphere sphere, out float t) {
    vec3 toSphere = rayOrigin - sphere.center;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(toSphere, rayDir);
    float c = dot(toSphere, toSphere) - sphere.radius * sphere.radius;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0.0) return false;
    t = (-b - sqrt(discriminant)) / (2.0 * a);
    return t > 0.0;
}

// 检测光线是否遮挡
bool isShadowed(vec3 hitPos, vec3 lightDir, float maxDistance) {
    for(int i = 0; i < numSpheres; i++) {
        float t;
        if(rayIntersectsSphere(hitPos + 0.001 * lightDir, lightDir, spheres[i], t)) {
            if(t < maxDistance) return true; // 存在遮挡物体
        }
    }
    return false; // 没有遮挡物体
}

// 计算反射
vec3 calculateReflection(vec3 rayDir, vec3 hitPos, vec3 hitNormal, int depth) {
    vec3 reflectDir = reflect(rayDir, hitNormal);
    vec3 reflectOrigin = hitPos + 0.001 * reflectDir; // 偏移一小步，避免光线与自身相交
    vec3 reflectColor = vec3(0.0);

    for (int i = 0; i < depth; ++i) {
        bool hitSomething = false;
        float closestT = 10000.0; // 一个足够大的数
        vec3 closestHitPos;
        vec3 closestHitNormal;

        for (int j = 0; j < numSpheres; ++j) {
            float t;
            if (rayIntersectsSphere(reflectOrigin, reflectDir, spheres[j], t) && t < closestT) {
                closestT = t;
                closestHitPos = reflectOrigin + t * reflectDir;
                closestHitNormal = normalize(closestHitPos - spheres[j].center);
                hitSomething = true;
            }
        }

        if (!hitSomething) break;

        // 计算光照
        vec3 viewDir = normalize(viewPos - closestHitPos);
        if (!isShadowed(closestHitPos, normalize(lightPos - closestHitPos), length(lightPos - closestHitPos))) {
            reflectColor += calculateLighting(closestHitPos, closestHitNormal, viewDir) * objectColor;
        }

        reflectOrigin = closestHitPos + 0.001 * reflectDir;
        reflectDir = reflect(rayDir, closestHitNormal);
    }

    return reflectColor * reflectivity;
}

void main() {
    vec3 rayOrigin = viewPos;
    vec3 rayDir = normalize(FragPos - viewPos); // 简化的光线方向计算
    float closestT = 10000.0; 
    vec3 color = vec3(0.0); // 默认背景色
    vec3 hitNormal;
    vec3 hitPos;

    for(int i = 0; i < numSpheres; i++) {
        float t;
        if(rayIntersectsSphere(rayOrigin, rayDir, spheres[i], t)) {
            if(t < closestT) {
                closestT = t;
                hitPos = rayOrigin + t * rayDir;
                hitNormal = normalize(hitPos - spheres[i].center);
                vec3 viewDir = normalize(viewPos - hitPos);
                if (!isShadowed(hitPos, normalize(lightPos - hitPos), length(lightPos - hitPos))) {
                    color = calculateLighting(hitPos, hitNormal, viewDir) * objectColor;
                    color += calculateReflection(rayDir, hitPos, hitNormal, 1); // 添加反射效果
                } else {
                    color = vec3(0.0); // 如果存在阴影，颜色为黑色
                    color = 0.8*calculateLighting(hitPos, hitNormal, viewDir) * objectColor;
                    color += calculateReflection(rayDir, hitPos, hitNormal, 1); // 添加反射效果
                }
            }
        }
    }

    FragColor = vec4(color, 1.0);
}

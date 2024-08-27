#version 330 core
out vec4 FragColor;

in vec3 FragPos; // �Ӷ�����ɫ�������Ķ���λ��
in vec3 Normal;  // �Ӷ�����ɫ�������ķ�����

uniform vec3 lightPos;//��Դλ��
uniform vec3 viewPos;//�����λ��
uniform vec3 lightColor;//�ƹ���ɫ
uniform vec3 objectColor;//������ɫ

uniform float ambientStrength; // ������ǿ��
uniform float specularStrength; // ���淴��ǿ��
uniform float reflectivity; // ����ǿ��

struct Sphere {
    vec3 center;
    float radius;
};

uniform Sphere spheres[10]; // ���������10������
uniform int numSpheres; // ʵ����������

// �������
vec3 calculateLighting(vec3 hitPos, vec3 hitNormal, vec3 viewDir) {
    // ������ǿ��
    vec3 ambient = ambientStrength * lightColor;

    // ������
    vec3 norm = normalize(hitNormal);
    vec3 lightDir = normalize(lightPos - hitPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ���淴��
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    return ambient + diffuse + specular;
}

// ������������Ľ���
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

// �������Ƿ��ڵ�
bool isShadowed(vec3 hitPos, vec3 lightDir, float maxDistance) {
    for(int i = 0; i < numSpheres; i++) {
        float t;
        if(rayIntersectsSphere(hitPos + 0.001 * lightDir, lightDir, spheres[i], t)) {
            if(t < maxDistance) return true; // �����ڵ�����
        }
    }
    return false; // û���ڵ�����
}

// ���㷴��
vec3 calculateReflection(vec3 rayDir, vec3 hitPos, vec3 hitNormal, int depth) {
    vec3 reflectDir = reflect(rayDir, hitNormal);
    vec3 reflectOrigin = hitPos + 0.001 * reflectDir; // ƫ��һС������������������ཻ
    vec3 reflectColor = vec3(0.0);

    for (int i = 0; i < depth; ++i) {
        bool hitSomething = false;
        float closestT = 10000.0; // һ���㹻�����
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

        // �������
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
    vec3 rayDir = normalize(FragPos - viewPos); // �򻯵Ĺ��߷������
    float closestT = 10000.0; 
    vec3 color = vec3(0.0); // Ĭ�ϱ���ɫ
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
                    color += calculateReflection(rayDir, hitPos, hitNormal, 1); // ��ӷ���Ч��
                } else {
                    color = vec3(0.0); // ���������Ӱ����ɫΪ��ɫ
                    color = 0.8*calculateLighting(hitPos, hitNormal, viewDir) * objectColor;
                    color += calculateReflection(rayDir, hitPos, hitNormal, 1); // ��ӷ���Ч��
                }
            }
        }
    }

    FragColor = vec4(color, 1.0);
}

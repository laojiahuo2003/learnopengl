#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos; // 顶点的位置
out vec3 Normal;  // 顶点的法向量

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 计算顶点位置并传递给片段着色器
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 计算法向量并传递给片段着色器
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // 计算顶点位置
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

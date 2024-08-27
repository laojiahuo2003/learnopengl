#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//flat关键字表示禁止在片段着色器输入时进行插值。这意味着对于一个三角形的所有片段，将使用同一个值，即三角形某一个顶点的值，而不会根据片段的位置在顶点之间插值。
flat out vec3 FlatNormal; // 固定的法向量，用于Flat Shading
out vec3 FragPos; // 顶点的位置

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 计算顶点位置并传递给片段着色器
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 计算法向量并传递给片段着色器
    FlatNormal = mat3(transpose(inverse(model))) * aNormal;

    // 计算顶点位置
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

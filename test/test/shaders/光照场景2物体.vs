#version 330 core
layout (location = 0) in vec3 aPos;// 位置
layout (location = 1) in vec3 aNormal;// 法向量

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 Normal;
out vec3 FragPos;  // 世界坐标(顶点的模型空间坐标经过模型变换（model transformation）计算得出的结果)
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = aNormal;
    Normal = mat3(transpose(inverse(model))) * aNormal;//局部空间法向量转换为世界空间中的正确法向量方向(不等比缩放，对法向量会产生影响)
}



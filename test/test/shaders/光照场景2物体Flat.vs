#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//flat�ؼ��ֱ�ʾ��ֹ��Ƭ����ɫ������ʱ���в�ֵ������ζ�Ŷ���һ�������ε�����Ƭ�Σ���ʹ��ͬһ��ֵ����������ĳһ�������ֵ�����������Ƭ�ε�λ���ڶ���֮���ֵ��
flat out vec3 FlatNormal; // �̶��ķ�����������Flat Shading
out vec3 FragPos; // �����λ��

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // ���㶥��λ�ò����ݸ�Ƭ����ɫ��
    FragPos = vec3(model * vec4(aPos, 1.0));

    // ���㷨���������ݸ�Ƭ����ɫ��
    FlatNormal = mat3(transpose(inverse(model))) * aNormal;

    // ���㶥��λ��
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

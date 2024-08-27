#version 330 core
layout (location = 0) in vec3 aPos;// λ��
layout (location = 1) in vec3 aNormal;// ������

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 Normal;
out vec3 FragPos;  // ��������(�����ģ�Ϳռ����꾭��ģ�ͱ任��model transformation������ó��Ľ��)
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = aNormal;
    Normal = mat3(transpose(inverse(model))) * aNormal;//�ֲ��ռ䷨����ת��Ϊ����ռ��е���ȷ����������(���ȱ����ţ��Է����������Ӱ��)
}



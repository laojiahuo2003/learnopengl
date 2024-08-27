#version 330 core
out vec4 FragColor;

in vec3 Normal;// ������
in vec3 FragPos;// ��������

uniform vec3 objectColor;// ������ɫ
uniform vec3 lightColor;// ��Դ��ɫ

uniform float ambientStrength;// ������ǿ��
uniform float specularStrength;// ���淴��ǿ��
uniform vec3 lightPos;//��Դλ��

uniform vec3 viewPos;//�����λ��
void main()
{
    // ������ǿ��
    vec3 ambient = ambientStrength * lightColor;// ��Դ����ɫ�ͻ�����ǿ����ģ�⻷����

    //������
    vec3 norm = normalize(Normal);// ��������λ����
    vec3 lightDir = normalize(lightPos - FragPos);// ���߷�������
    float diff = max(dot(norm, lightDir), 0.0);// �������͹���������ˣ�ģ���ǿ
    vec3 diffuse = diff * lightColor;

    // ���淴��
    vec3 viewDir = normalize(viewPos - FragPos);// ���߷���
    vec3 reflectDir = reflect(-lightDir, norm);  // ���䷽��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);// �������
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);

}
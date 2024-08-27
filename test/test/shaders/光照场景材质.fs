#version 330 core
struct Material {
    vec3 ambient;// �������ڻ���������������淴�����ʲô��ɫ 
    vec3 diffuse;// ������������±������ɫ
    vec3 specular;// ����߹����ɫ
    float shininess;// ����߹��ɢ��/�뾶 
}; 
uniform Material material;// �����������(�������Ӱ�����Ч��)

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;// ����ǿ������


out vec4 FragColor;

in vec3 Normal;// ������
in vec3 FragPos;// ��������

//uniform vec3 objectColor;// ������ɫ
//uniform vec3 lightColor;// ��Դ��ɫ

uniform float ambientStrength;// ������ǿ��
uniform float specularStrength;// ���淴��ǿ��
uniform vec3 lightPos;//��Դλ��

uniform vec3 viewPos;//�����λ��
void main()
{
    // ������
    vec3 ambient = light.ambient * material.ambient;// ��Դ����ɫ�ͻ�����ǿ����ģ�⻷����

    //������
    vec3 norm = normalize(Normal);// ��������λ����
    vec3 lightDir = normalize(lightPos - FragPos);// ���߷�������
    float diff = max(dot(norm, lightDir), 0.0);// �������͹���������ˣ�ģ���ǿ
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // ���淴��
    vec3 viewDir = normalize(viewPos - FragPos);// ���߷���
    vec3 reflectDir = reflect(-lightDir, norm);  // ���䷽��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// �������
    vec3 specular = light.specular * (spec * material.specular); 
        
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);

}
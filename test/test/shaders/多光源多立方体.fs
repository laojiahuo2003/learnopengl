#version 330 core
out vec4 FragColor; // �����Ƭ����ɫ

// ���ʽṹ�壬���������䡢���淴����ͼ�͹��������
struct Material {
    sampler2D diffuse; // ������������ͼ
    sampler2D specular; // ���淴��������ͼ
    float shininess; // ��������ӣ����ھ���߹�
}; 

// ƽ�й�ṹ��
struct DirLight {
    vec3 direction; // ��ķ���
    
    vec3 ambient; // ���������
    vec3 diffuse; // ���������
    vec3 specular; // ���淴�����
};

// ���Դ�ṹ��
struct PointLight {
    vec3 position; // ���Դ��λ��
    
    float constant; // ������˥��
    float linear; // ����˥��
    float quadratic; // ���η�˥��
    
    vec3 ambient; // ���������
    vec3 diffuse; // ���������
    vec3 specular; // ���淴�����
};

// �۹�ƽṹ��
struct SpotLight {
    vec3 position; // �۹��λ��
    vec3 direction; // �۹�Ʒ���
    float cutOff; // ��Բ׶�ǵ�����ֵ
    float outerCutOff; // ��Բ׶�ǵ�����ֵ
  
    float constant; // ������˥��
    float linear; // ����˥��
    float quadratic; // ���η�˥��
  
    vec3 ambient; // ���������
    vec3 diffuse; // ���������
    vec3 specular; // ���淴�����
};

#define NR_POINT_LIGHTS 4 // ���Դ������

in vec3 FragPos; // Ƭ��λ��
in vec3 Normal; // Ƭ�η���
in vec2 TexCoords; // Ƭ�ε���������

uniform vec3 viewPos; // �۲���λ��
uniform DirLight dirLight; // ƽ�й�
uniform PointLight pointLights[NR_POINT_LIGHTS]; // ���Դ����
uniform SpotLight spotLight; // �۹��
uniform Material material; // ����

// ����ԭ������
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // ����
    vec3 norm = normalize(Normal); // ��һ������
    vec3 viewDir = normalize(viewPos - FragPos); // �������߷���
    
    // == =====================================================
    // ���ռ����Ϊ�����׶Σ�ƽ�й⡢���Դ�Ϳ�ѡ�ľ۹��
    // ÿ���׶ζ���һ�����㺯�������ڼ���ÿ���ƹ����ɫ�Ĺ���
    // �� main() �����У����ǽ����м��������ɫ��ӣ��õ�Ƭ�ε�������ɫ
    // == =====================================================
    // �׶� 1��ƽ�й�
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // �׶� 2�����Դ
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // �׶� 3���۹��
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0); // ����Ƭ����ɫ
}

// ����ƽ�й����µ���ɫ
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); // ������߷���
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0); 
    // ���淴����ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ��Ͻ��
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); // ������
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); // ������
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // ���淴��
    return (ambient + diffuse + specular); // ����������ɫ
}

// ������Դ�µ���ɫ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); // ������߷���
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // ���淴����ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥������
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // ��Ͻ��
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); // ������
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); // ������
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // ���淴��
    ambient *= attenuation; // ����˥��
    diffuse *= attenuation; // ����˥��
    specular *= attenuation; // ����˥��
    return (ambient + diffuse + specular); // ����������ɫ
}

// ����۹���µ���ɫ
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); // ������߷���
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // ���淴����ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥������
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // �۹��ǿ��
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // ��Ͻ��
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); // ������
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); // ������
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // ���淴��
    ambient *= attenuation * intensity; // ����˥����ǿ��
    diffuse *= attenuation * intensity; // ����˥����ǿ��
    specular *= attenuation * intensity; // ����˥����ǿ��
    return (ambient + diffuse + specular); // ����������ɫ
}

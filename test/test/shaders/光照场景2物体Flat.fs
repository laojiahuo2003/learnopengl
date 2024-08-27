#version 330 core
out vec4 FragColor;

flat in vec3 FlatNormal; // �Ӷ�����ɫ�������Ĺ̶�������(��Ҫ���޸���flat)
in vec3 FragPos; // �Ӷ�����ɫ�������Ķ���λ��

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float ambientStrength;// ������ǿ��
uniform float specularStrength;// ���淴��ǿ��

void main()
{
    // ���㻷����
    //float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // ����������
    vec3 norm = normalize(FlatNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // ���㾵�淴��
    //float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;      
    
    vec3 lighting = ambient + diffuse + specular;
    vec3 result = lighting * objectColor;
    FragColor = vec4(result, 1.0);
}

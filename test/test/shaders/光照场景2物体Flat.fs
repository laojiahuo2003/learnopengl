#version 330 core
out vec4 FragColor;

flat in vec3 FlatNormal; // 从顶点着色器传来的固定法向量(主要是修改了flat)
in vec3 FragPos; // 从顶点着色器传来的顶点位置

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float ambientStrength;// 环境光强度
uniform float specularStrength;// 镜面反射强度

void main()
{
    // 计算环境光
    //float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // 计算漫反射
    vec3 norm = normalize(FlatNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 计算镜面反射
    //float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;      
    
    vec3 lighting = ambient + diffuse + specular;
    vec3 result = lighting * objectColor;
    FragColor = vec4(result, 1.0);
}

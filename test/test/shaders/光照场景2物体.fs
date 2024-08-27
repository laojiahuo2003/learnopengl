#version 330 core
out vec4 FragColor;

in vec3 Normal;// 法向量
in vec3 FragPos;// 世界坐标

uniform vec3 objectColor;// 物体颜色
uniform vec3 lightColor;// 光源颜色

uniform float ambientStrength;// 环境光强度
uniform float specularStrength;// 镜面反射强度
uniform vec3 lightPos;//光源位置

uniform vec3 viewPos;//摄像机位置
void main()
{
    // 环境光强度
    vec3 ambient = ambientStrength * lightColor;// 光源的颜色和环境光强度来模拟环境光

    //漫反射
    vec3 norm = normalize(Normal);// 法向量单位向量
    vec3 lightDir = normalize(lightPos - FragPos);// 光线方向向量
    float diff = max(dot(norm, lightDir), 0.0);// 法向量和光线向量点乘，模拟光强
    vec3 diffuse = diff * lightColor;

    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);// 视线方向
    vec3 reflectDir = reflect(-lightDir, norm);  // 反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);// 镜面分量
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);

}
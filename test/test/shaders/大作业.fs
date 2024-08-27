#version 330 core
out vec4 FragColor; // 输出的片段颜色

// 材质结构体，包含漫反射、镜面反射贴图和光泽度因子
struct Material {
    sampler2D diffuse; // 漫反射纹理贴图
    sampler2D specular; // 镜面反射纹理贴图
    float shininess; // 光泽度因子，用于镜面高光
}; 

// 平行光结构体
struct DirLight {
    vec3 direction; // 光的方向
    
    vec3 ambient; // 环境光分量
    vec3 diffuse; // 漫反射分量
    vec3 specular; // 镜面反射分量
};

// 点光源结构体
struct PointLight {
    vec3 position; // 点光源的位置
    
    float constant; // 常数项衰减
    float linear; // 线性衰减
    float quadratic; // 二次方衰减
    
    vec3 ambient; // 环境光分量
    vec3 diffuse; // 漫反射分量
    vec3 specular; // 镜面反射分量
};

// 聚光灯结构体
struct SpotLight {
    vec3 position; // 聚光灯位置
    vec3 direction; // 聚光灯方向
    float cutOff; // 内圆锥角的余弦值
    float outerCutOff; // 外圆锥角的余弦值
  
    float constant; // 常数项衰减
    float linear; // 线性衰减
    float quadratic; // 二次方衰减
  
    vec3 ambient; // 环境光分量
    vec3 diffuse; // 漫反射分量
    vec3 specular; // 镜面反射分量
};

#define NR_POINT_LIGHTS 4 // 点光源的数量

in vec3 FragPos; // 片段位置
in vec3 Normal; // 片段法线
in vec2 TexCoords; // 片段的纹理坐标

uniform vec3 viewPos; // 观察者位置
uniform DirLight dirLight; // 平行光
uniform PointLight pointLights[NR_POINT_LIGHTS]; // 点光源数组
uniform SpotLight spotLight; // 聚光灯
uniform Material material; // 材质

// 函数原型声明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // 属性
    vec3 norm = normalize(Normal); // 归一化法线
    vec3 viewDir = normalize(viewPos - FragPos); // 计算视线方向
    
    // == =====================================================
    // 光照计算分为三个阶段：平行光、点光源和可选的聚光灯
    // 每个阶段都有一个计算函数，用于计算每个灯光对颜色的贡献
    // 在 main() 函数中，我们将所有计算出的颜色相加，得到片段的最终颜色
    // == =====================================================
    // 阶段 1：平行光
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // 阶段 2：点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // 阶段 3：聚光灯
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0); // 设置片段颜色
}

// 计算平行光照下的颜色
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); // 计算光线方向
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0); 
    // 镜面反射着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 组合结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); // 环境光
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); // 漫反射
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // 镜面反射
    return (ambient + diffuse + specular); // 返回最终颜色
}

// 计算点光源下的颜色
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); // 计算光线方向
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面反射着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减计算
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // 组合结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); // 环境光
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); // 漫反射
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // 镜面反射
    ambient *= attenuation; // 考虑衰减
    diffuse *= attenuation; // 考虑衰减
    specular *= attenuation; // 考虑衰减
    return (ambient + diffuse + specular); // 返回最终颜色
}

// 计算聚光灯下的颜色
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); // 计算光线方向
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面反射着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减计算
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // 聚光灯强度
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // 组合结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); // 环境光
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); // 漫反射
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // 镜面反射
    ambient *= attenuation * intensity; // 考虑衰减和强度
    diffuse *= attenuation * intensity; // 考虑衰减和强度
    specular *= attenuation * intensity; // 考虑衰减和强度
    return (ambient + diffuse + specular); // 返回最终颜色
}

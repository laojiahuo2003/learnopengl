#version 330 core
struct Material {
    vec3 ambient;// 定义了在环境光照下这个表面反射的是什么颜色 
    vec3 diffuse;// 在漫反射光照下表面的颜色
    vec3 specular;// 镜面高光的颜色
    float shininess;// 镜面高光的散射/半径 
}; 
uniform Material material;// 物体材质属性(物体材质影响光照效果)

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;// 光照强度向量


out vec4 FragColor;

in vec3 Normal;// 法向量
in vec3 FragPos;// 世界坐标

//uniform vec3 objectColor;// 物体颜色
//uniform vec3 lightColor;// 光源颜色

uniform float ambientStrength;// 环境光强度
uniform float specularStrength;// 镜面反射强度
uniform vec3 lightPos;//光源位置

uniform vec3 viewPos;//摄像机位置
void main()
{
    // 环境光
    vec3 ambient = light.ambient * material.ambient;// 光源的颜色和环境光强度来模拟环境光

    //漫反射
    vec3 norm = normalize(Normal);// 法向量单位向量
    vec3 lightDir = normalize(lightPos - FragPos);// 光线方向向量
    float diff = max(dot(norm, lightDir), 0.0);// 法向量和光线向量点乘，模拟光强
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);// 视线方向
    vec3 reflectDir = reflect(-lightDir, norm);  // 反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// 镜面分量
    vec3 specular = light.specular * (spec * material.specular); 
        
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);

}
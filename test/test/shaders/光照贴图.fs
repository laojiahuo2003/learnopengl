#version 330 core
struct Material {
//    vec3 ambient;// 定义了在环境光照下这个表面反射的是什么颜色 
    sampler2D diffuse;// 在漫反射光照下表面的颜色
    sampler2D specular;// 镜面高光的颜色
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
in vec2 TexCoords;// 材质坐标

uniform vec3 viewPos;//摄像机位置
void main()
{
    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));// 光源的颜色和环境光强度来模拟环境光

    //漫反射
    vec3 norm = normalize(Normal);// 法向量单位向量
    vec3 lightDir = normalize(light.position - FragPos);// 光线方向向量
    float diff = max(dot(norm, lightDir), 0.0);// 法向量和光线向量点乘，模拟光强
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);// 视线方向
    vec3 reflectDir = reflect(-lightDir, norm);  // 反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// 镜面分量
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords))); 
        
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);

}
#version 330 core
out vec4 FragColor;

// �����
struct DirectionLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

// ���Դ
struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

// �۹��
struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

// ����
struct Material {
  sampler2D diffuse; // ��������ͼ
  sampler2D specular; // �������ͼ
  float shininess; // �߹�ָ��
};

#define NR_POINT_LIGHTS 4

uniform Material material;
uniform DirectionLight directionLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform sampler2D awesomeMap; // Ц����ͼ

in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

uniform vec3 viewPos;

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {

  vec4 objectColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

  vec3 viewDir = normalize(viewPos - outFragPos);
  vec3 normal = normalize(outNormal);

  // �������
  vec3 result = CalcDirectionLight(directionLight, normal, viewDir);

  // ���Դ
  for(int i = 0; i < NR_POINT_LIGHTS; i++) {
    result += CalcPointLight(pointLights[i], normal, outFragPos, viewDir);
  }
  // �۹��Դ
  //result += CalcSpotLight(spotLight, normal, outFragPos, viewDir) * texture(awesomeMap, outTexCoord).rgb;
    result += CalcSpotLight(spotLight, normal, outFragPos, viewDir);
  FragColor = vec4(result, 1.0);
}

// ���㶨���
vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  // �ϲ�
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

  return ambient + diffuse + specular;
}

// ������Դ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
  float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));    
    // �ϲ����
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

// ����۹��
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;
  return (ambient + diffuse + specular);
}
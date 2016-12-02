#version 410 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHT 4

struct SpotLight
{
    vec3 position;
    vec3 direction;  // 从物体指向光源的方向
    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHT];
uniform SpotLight spotLight;


vec3 CalcuDirectionLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcuPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);
vec3 CalcuSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // 平行光照
    vec3 result = CalcuDirectionLight(dirLight, norm, viewDir);
    
    // 众多点光源
    for (int i = 0; i < NR_POINT_LIGHT; ++i)
        result += CalcuPointLight(pointLights[i], norm, FragPos, viewDir);
    
    // 聚光
    result += CalcuSpotLight(spotLight, norm, FragPos, viewDir);
    
    color = vec4(result, 1.0f);
}


// 平行光计算
vec3 CalcuDirectionLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // 漫反射强度
    float diff = max(dot(normal, lightDir), 0.0f);
    
    // 高光强度
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    
    // 合并各个光照分量
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    return (ambient + diffuse + specular);
}


// 点光源计算
vec3 CalcuPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    
    // 漫反射强度
    float diff = max(dot(normal, lightDir), 0.0f);
    
    // 高光强度
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    
    // 衰减
    float ligDistance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + ligDistance * light.linear + ligDistance * ligDistance * light.quadratic);
    
    // 合并各分量
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    return ((ambient + diffuse + specular) * attenuation);
}


// 聚光计算
vec3 CalcuSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    
    // 漫反射强度
    float diff = max(dot(normal, lightDir), 0.0f);
    
    // 高光强度
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    
    // 衰减
    float ligDistance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + ligDistance * light.linear + ligDistance * ligDistance * light.quadratic);
    
    // 边缘平滑
    float theta = dot(lightDir, normalize(-light.direction));
    float epslion = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epslion, 0.0f, 1.0f);
    
    // 合并分量
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    return ((ambient + diffuse + specular) * attenuation * intensity);
}


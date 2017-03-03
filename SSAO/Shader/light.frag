#version 410 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Light
{
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

uniform Light light;


void main()
{
    // G-buffer
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    // 环境遮挡因子
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    // 环境光
    vec3 ambient = vec3(0.3 * AmbientOcclusion);
    vec3 lighting  = ambient;
    vec3 viewDir  = normalize(-FragPos);
    
    // 漫反射
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    
    // 镜面反射
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec;
    
    // 渐变
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;
    
    FragColor = vec4(lighting, 1.0);
}

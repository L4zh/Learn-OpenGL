#version 410 core

layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

// 获取线性深度
const float NEAR = 0.1;  // 投影矩阵的近平面
const float FAR = 50.0f;  // 投影矩阵的远平面

float  LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}


//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

void main()
{
    // 位置矢量到第一个G缓冲纹理
    gPositionDepth.xyz = FragPos;
    
    // 存储线性深度
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    
    // 存储法线信息
    gNormal = normalize(Normal);
    
    // 存储漫反射颜色
    gAlbedoSpec.rgb = vec3(0.95);
}

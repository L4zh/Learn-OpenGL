// 从 G-Buffer 中获取线性深度 噪声纹理 法相半球核心

#version 410 core

out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

const vec2 noiseScale = vec2(800.0f / 4.0f, 600.0f / 4.0f);

int kernelSize = 64;
float radius = 1.0;

void main()
{
    // SSAO 输入
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
    
    // 切线空间到模型空间转换
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // 采样核心 计算遮蔽因子
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // 得到采样位置
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // 保证范围0.0-1.0
        
        // 采样深度
        float sampleDepth = -texture(gPositionDepth, offset.xy).w;
        
        // 测试范围
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}

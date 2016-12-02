#version 410 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float viDig;

void main()
{
    color = mix(texture(ourTexture1, TexCoord),
                texture(ourTexture2, vec2(1.0f - TexCoord.x, TexCoord.y)) * vec4(ourColor, 1.0f), viDig);
}
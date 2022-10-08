#version 400 core

#include "hairlighting.glsl"
//输入
in vec4 position_g;
in vec3 tangent_g;//切线
in float colorVariation_g;

out vec4 fragColor;//输出

void main()
{
    fragColor = hairLighting(position_g, tangent_g, colorVariation_g);
}

#version 400 core

#include "meshlighting.glsl"

//以下输入可由mesh.vert的输出得到
in vec4 position_v;
in vec4 normal_v;
in vec2 uv_v;

uniform mat4 projection;//投影矩阵

out vec4 fragColor;//片段着色器的颜色输出 坐标

void main(){

    //参数是原mesh坐标经过modle变换后得到的（世界坐标），其中uv_v没有经过变换
    fragColor = meshLighting(position_v, normal_v, uv_v);//得到片段着色器处理后的颜色输出（融合了光源的影响，包含漫反射光分量、等其他光分量）
}

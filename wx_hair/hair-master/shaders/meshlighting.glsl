#include "constants.glsl"
#include "opacitymapping.glsl"

uniform vec3 hairColor;
uniform vec3 lightPosition;//光源位置
uniform mat4 view, eyeToLight;//view世界到相机的转换矩阵  eyeToLight用于渲染阴影贴图的矩阵
uniform sampler2D hairGrowthMap;//用于生成头的2D纹理采样器（纹理贴图）

vec3 meshColor;
//得到漫反射光分量
vec3 colorContribution(in vec4 position_WS, in vec4 normal_WS, in vec4 lightPosition_WS)
{
    vec4 toLight = lightPosition_WS - position_WS;//计算光源和片段位置之间的方向向量
    vec4 normal_N = normalize(normal_WS);
    float diffuse = max(0.f, dot(normalize(toLight), normal_N));//点乘，计算光源对当前片段实际的漫发射影响
    return diffuse * MESH_DIFFUSE_INTENSITY * meshColor;
}

vec4 meshLighting(in vec4 position_WS, in vec4 normal_WS, in vec2 uv) {
    meshColor = mix(MESH_COLOR, MIN_COLOR*hairColor,texture(hairGrowthMap, uv).r);//混合色

    vec4 position_lightSpace = eyeToLight * view * position_WS;

    vec4 color;
    color.w = 1.0;
     //以下（连乘）计算得到的color为输出的最终颜色
    // 主光线（Key light）
    color.xyz = colorContribution(position_WS, normal_WS, vec4(lightPosition, 1.0));
    color.xyz *= getHairTransmittance(position_lightSpace);
    color.xyz *= getMeshVisibility(position_lightSpace);

    // 辅助光（Fill light）
    color.xyz += FILL_LIGHT_INTENSITY_MESH * colorContribution(position_WS, normal_WS, FILL_LIGHT_POS);

    // 环境光照(Ambient Lighting)
    color.xyz += MESH_AMBIENT_INTENSITY * meshColor;//两个因子分别表示光照颜色 × 物体颜色=（反射色）人眼看到的颜色

    return color;//片段的最终输出颜色
}

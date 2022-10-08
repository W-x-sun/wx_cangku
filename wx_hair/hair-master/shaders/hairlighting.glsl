#include "constants.glsl"
#include "opacitymapping.glsl"

in float tessx_g;

uniform vec3 color;//
uniform float specIntensity, diffuseIntensity;//镜面反射强度  漫反射强度
uniform mat4 view, eyeToLight;//view世界到相机的转换矩阵  eyeToLight用于渲染阴影贴图的矩阵
uniform vec3 lightPosition;//光源位置向量
uniform float opacity;//不透明度
uniform float maxColorVariation;
//得到光的各种分量（影响）
vec3 colorContribution(in vec4 position_ES, in vec3 tangent_ES, in vec4 lightPosition_ES, in float colorVariation)
{
    vec4 toLight_N = normalize(lightPosition_ES - position_ES);
    vec3 tangent_N = normalize(tangent_ES);
    vec3 toEye_N = normalize(-position_ES.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);

    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));//漫反射分量
    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), HAIR_SHININESS);//镜面反射

    // 添加颜色变化
    vec3 colorMultiplier = vec3(1.0 + maxColorVariation * (2.0 * colorVariation - 1.0));

    // 添加颜色渐变
    colorMultiplier *= mix(MIN_COLOR, 1.0, smoothstep(MIN_COLOR_END, MAX_COLOR_START, tessx_g));

    return (diffuseIntensity * diffuse + specIntensity * specular) * color * colorMultiplier;
}
//得到最终的片段颜色输出
vec4 hairLighting(in vec4 position_ES, in vec3 tangent_ES, in float colorVariation)
{
    vec4 position_lightSpace = eyeToLight * position_ES;

    vec4 color;
    color.w = opacity;

    // Key light 主光线效果
    vec4 lightPos = view * vec4(lightPosition, 1.);
    color.xyz = colorContribution(position_ES, tangent_ES, lightPos, colorVariation);
    color.xyz *= getHairTransmittance(position_lightSpace);
    color.xyz *= getMeshVisibility(position_lightSpace);

    // Fill light 辅助光效果
    lightPos = view * FILL_LIGHT_POS;
    color.xyz += FILL_LIGHT_INTENSITY_HAIR * colorContribution(position_ES, tangent_ES, lightPos, colorVariation);

    return color;
}

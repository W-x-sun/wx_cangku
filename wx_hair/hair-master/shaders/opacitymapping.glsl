uniform sampler2D hairShadowMap;
uniform sampler2DShadow meshShadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;
uniform bool useShadows;

float currDepth;

// 在给定UV坐标下对不透明度贴图的所有层进行采样。返回一个
//与其他头发碎片的遮挡量相对应的编号。
float occlusionSample(vec2 uv)
{
    vec4 opacityMapValues = texture(opacityMap, uv);

    float occlusion = 0.; // 不透明度贴图层的遮挡量
    float layerSize = OPACITY_MAP_LAYER_SIZE; // 当前层大小
    float layerStart = texelFetch(hairShadowMap, ivec2(uv * textureSize(hairShadowMap, 0)), 0).r;

    for (int layer = 0; layer < 4; layer++)
    {
        float t = clamp((currDepth - layerStart) / layerSize, 0.0, 1.0);
        occlusion += mix(0, opacityMapValues[layer], t);

        layerStart += layerSize;
        layerSize *= 2.0;
    }
    return occlusion;
}

// 基于深度不透明度贴图计算光源到点p的透射比，其中p位于光源的剪裁空间中。
float getHairTransmittance(vec4 p)
{
    vec4 shadowCoord = (p / p.w + 1.0) / 2.0;
    vec2 uv = shadowCoord.xy;
    currDepth = shadowCoord.z - 0.0001;

    vec2 size = textureSize(hairShadowMap, 0); // Size of texture (e.g. 1024, 1024)
    vec2 texelSize = vec2(1.) / size; // Size of texel (e.g. 1/1024, 1/1024)

    ivec2 offset = ivec2(0, 1);

    // 在深度不透明贴图的四个采样之间进行线性插值。
    vec2 f = fract(uv * size);
    float s1 = occlusionSample(uv + texelSize * offset.xx);
    float s2 = occlusionSample(uv + texelSize * offset.xy);
    float s3 = occlusionSample(uv + texelSize * offset.yx);
    float s4 = occlusionSample(uv + texelSize * offset.yy);
    float occlusion = mix( mix(s1, s2, f.y), mix(s3, s4, f.y), f.x );

    float transmittance = exp(- shadowIntensity * occlusion);
    return mix(1.0, transmittance, useShadows);
}

float getMeshVisibility(vec4 p)
{
    vec4 shadowCoord = (p / p.w + 1.0) / 2.0;
    shadowCoord.z -= 0.0003;
    float meshVisibility = texture(meshShadowMap, shadowCoord.xyz);
    return mix(1.0, meshVisibility, useShadows);
}

uniform sampler2D depthPeelMap;

/**
 * 检查点p（在剪辑空间中）是否在最后渲染的深度值之后。
 * 如果是，则颜色不变。如果不是，则颜色设置为背景色
 * 并且调整gl_ FragDepth。
 */
void depthPeel(inout vec4 color, in vec4 p)
{
    vec4 clipCoord = (p / p.w + 1.0) / 2.0;
    vec2 uv = clipCoord.xy;

    float currDepth = gl_FragCoord.z - 1e-4;
    float lastDepth = texelFetch(depthPeelMap, ivec2(uv * textureSize(depthPeelMap, 0)), 0).r;

    // 如果深度小于上一个深度剥离贴图中的值，则应丢弃碎片.
    float discardFrag = step(currDepth, lastDepth);

    color = mix(color, BACKGROUND_COLOR, discardFrag);
    gl_FragDepth = mix(gl_FragCoord.z, 1.0, discardFrag);

}

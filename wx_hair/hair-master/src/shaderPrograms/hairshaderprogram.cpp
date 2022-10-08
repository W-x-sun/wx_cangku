#include "hairshaderprogram.h"

#include "resourceloader.h"
//头发着色器程序  (返回值为所创建的完整的着色器程序对应句柄)
GLuint HairShaderProgram::createShaderProgram()
{
    //创建完整的着色器程序
    return ResourceLoader::createFullShaderProgram(//顶点 片段 几何 细分曲面控制着色器tcs、细分曲面计算着色器tes
                ":/shaders/hair.vert", ":/shaders/hair.frag", ":/shaders/hair.geom",
                ":/shaders/hair.tcs", ":/shaders/hair.tes");
}
//设置全局的uniform变量值
void HairShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("view", uniforms.view);//把参数2的值 传给参数1
    setUniformMatrix4f("projection", uniforms.projection);//
    setUniformMatrix4f("eyeToLight", uniforms.eyeToLight);
    setUniform3f("lightPosition", uniforms.lightPosition);
    setUniform1i("hairShadowMap", uniforms.hairShadowMap);
    setUniform1i("meshShadowMap", uniforms.meshShadowMap);
    setUniform1i("opacityMap", uniforms.opacityMap);
    setUniform1i("depthPeelMap", uniforms.depthPeelMap);
    setUniform1i("noiseTexture", uniforms.noiseTexture);//
    setUniform1f("shadowIntensity", uniforms.shadowIntensity);
    setUniform1i("useShadows", uniforms.useShadows);
}
//设置每个object对象的uniform
void HairShaderProgram::setPerObjectUniforms()
{
    setUniformMatrix4f("model", uniforms.model);//
    setUniform1i("numPatchHairs", uniforms.numGroupHairs);//
    setUniform1i("numSplineVertices", uniforms.numSplineVertices);//
    setUniform1f("hairGroupSpread", uniforms.hairGroupSpread);//
    setUniform1f("hairRadius", uniforms.hairRadius);//
    setUniform1f("taperExponent", uniforms.taperExponent);//
    setUniform1f("noiseAmplitude", uniforms.noiseAmplitude);//
    setUniform1f("noiseFrequency", uniforms.noiseFrequency);//
    setUniform3f("color", uniforms.color);
    setUniform1f("specIntensity", uniforms.specIntensity);
    setUniform1f("diffuseIntensity", uniforms.diffuseIntensity);
    setUniform1f("opacity", uniforms.opacity);
    setUniform1f("maxColorVariation", uniforms.maxColorVariation);
}
//设置每个绘制draw的uniform
void HairShaderProgram::setPerDrawUniforms()
{
    setUniform1f("hairLength", uniforms.length);//
    setUniform1i("numHairSegments", uniforms.numHairVertices-1);//
    setUniform3fv("vertexData", uniforms.numHairVertices, uniforms.vertexData);//
    setUniform3fv("triangleFace", 2, uniforms.triangleFace);//
}

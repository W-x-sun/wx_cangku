#include "meshshaderprogram.h"

#include "resourceloader.h"
#include <vector>
//网格着色器
GLuint MeshShaderProgram::createShaderProgram()
{
    //只包含顶点、片段着色器
    return ResourceLoader::createBasicShaderProgram(":/shaders/mesh.vert", ":/shaders/mesh.frag");
}
//设置全局的uniform，给着色器中的定义的uniform采样器变量赋值
void MeshShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("projection", uniforms.projection);
    setUniformMatrix4f("view", uniforms.view);
    setUniformMatrix4f("eyeToLight", uniforms.eyeToLight);
    setUniform1i("hairShadowMap", uniforms.hairShadowMap);
    setUniform1i("meshShadowMap", uniforms.meshShadowMap);
    setUniform1i("opacityMap", uniforms.opacityMap);
    setUniform3f("lightPosition", uniforms.lightPosition);
    setUniform1f("shadowIntensity", uniforms.shadowIntensity);
    setUniform1i("useShadows", uniforms.useShadows);
}
//设置每个object对象的uniform
void MeshShaderProgram::setPerObjectUniforms()
{
    setUniformMatrix4f("model", uniforms.model);
    setUniform1i("hairGrowthMap", uniforms.hairGrowthMap);
    setUniform3f("hairColor", uniforms.color);
}

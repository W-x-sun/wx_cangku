#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "hairCommon.h"
#include <map>
#include <string>

#define MAX_HAIR_VERTICES 64

struct Uniforms {
    glm::mat4 model, view, projection;

    glm::vec3 lightPosition;

    glm::mat4 eyeToLight; // 用于渲染阴影贴图的矩阵 (eye space --> light space).

    int numHairVertices; // 每撮的顶点数.

    glm::vec3 vertexData[MAX_HAIR_VERTICES]; // 当前撮头发 的顶点位置数据.
//几何模块所有参数
    int numGroupHairs; // 每一撮头发数量.
    int numSplineVertices; // 使用样条线渲染的顶点数.
    float hairGroupSpread; // 头发到其相应导向头发的最大距离.
    float hairRadius; // 一根头发的半径.
    float taperExponent; // 控制头发在末端开始逐渐变细的距离.（变尖指数）
    float noiseAmplitude; // 添加到每个头发顶点位置的噪波量.毛躁
    float noiseFrequency;//自然卷

    glm::vec3 triangleFace[2]; // 与头发法向量正交的平面的基向量.

    glm::vec3 color;

    float length;

    // 纹理 uniforms
    int noiseTexture;
    int hairShadowMap;
    int meshShadowMap;
    int opacityMap;
    int hairGrowthMap;
    int depthPeelMap;

    float specIntensity;//镜面强度
    float diffuseIntensity;//漫反射强度

    float opacity;//不透明度
    float maxColorVariation;

    // Shadows
    bool useShadows;
    float shadowIntensity; // 控制阴影暗度
};

class ShaderProgram
{
public:
    ShaderProgram() { }

    virtual ~ShaderProgram() { }

    virtual void create();

    virtual void bind();

    virtual void unbind();

    // 设置不发生变化的对象之间的uniforms.
    virtual void setGlobalUniforms() { }

    // 设置发生变化的对象之间的uniforms.
    virtual void setPerObjectUniforms() { }

    // 设置在绘制调用之间更改的所有uniforms（如果每个对象使用多个绘制调用）.
    virtual void setPerDrawUniforms() { }

    Uniforms uniforms;

    GLuint id;

protected:
    // 调用ResourceLoader中的一个程序创建函数，并返回程序ID。
    virtual GLuint createShaderProgram() = 0;

    void setUniform1i(GLchar const *name, int value);
    void setUniform1f(GLchar const *name, float value);
    void setUniform3f(GLchar const *name, glm::vec3 &value);
    void setUniform3fv(GLchar const *name, GLsizei count, glm::vec3 *values);
    void setUniformMatrix4f(GLchar const *name, glm::mat4 &value);

    std::map<std::string, int> m_uniformLocs;

private:
    GLint getUniformLocation(GLchar const *name);

    bool m_created = false;  //表示没有创建着色器程序，若创建成功，为ture
};

#endif // SHADERPROGRAM_H

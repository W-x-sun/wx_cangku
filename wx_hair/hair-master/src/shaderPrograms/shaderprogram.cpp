#include "shaderprogram.h"

#include "resourceloader.h"
#include "errorchecker.h"
#include <vector>
////创建着色器程序
void ShaderProgram::create()
{
    id = createShaderProgram();//创建着色器程序
    m_created = true;
}

void ShaderProgram::bind()
{
    if (!m_created)
    {
        std::cout << "尝试在创建着色器程序之前绑定它" << std::endl;
        exit(1);
    }
    glUseProgram(id);//使用对应id的着色器程序
    //glUseProgram()使用程序对象作为当前渲染状态的一部分  参数：指定程序对象的句柄，该程序对象的可执行文件将用作当前渲染状态的一部分。
}

void ShaderProgram::unbind()
{
    glUseProgram(0);//如果 参数为0，则当前渲染状态是无效的程序对象，并且着色器执行是未确定的
}

void ShaderProgram::setUniform1i(const GLchar *name, int value)
{
    //下边的函数功能是给变量传输数据
    //glUniform 1i（1i表示 1维int类型数据）（参数1：获取到的uniform变量的地址   参数2：给前边参数所赋的值）
    glUniform1i(getUniformLocation(name), value);//getUniformLocation（）获取指定名称uniform变量的存储地址
}

void ShaderProgram::setUniform1f(const GLchar *name, float value)
{
    //（1f表示 1维float类型数据）（参数1：获取到的uniform变量的地址   参数2：给前边参数所赋的值）
    glUniform1f(getUniformLocation(name), value);//getUniformLocation（）获取指定名称uniform变量的存储地址
}


void ShaderProgram::setUniform3f(const GLchar *name, glm::vec3 &value)
{
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));//传输三维浮点型顶点数据
}

//参数count：指明要更改的元素个数。如果目标uniform变量不是一个数组，那么这个值应该设为1；如果是数组，则应该设置为>=1。
//参数vlaue：指定一个具有count个数值的数组指针，用来更新指定的uniform变量
void ShaderProgram::setUniform3fv(const GLchar *name, GLsizei count, glm::vec3 *values)
{
    glUniform3fv(getUniformLocation(name), count, &values[0][0]);
}

void ShaderProgram::setUniformMatrix4f(const GLchar *name, glm::mat4 &value)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

GLint ShaderProgram::getUniformLocation(const GLchar *name)
{
    if (m_uniformLocs.count(name) == 0)
        m_uniformLocs[name] = glGetUniformLocation(id, name);
    return m_uniformLocs[name];
}

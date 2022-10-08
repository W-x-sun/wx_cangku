#include "openglshape.h"
#include <iostream>
#include <stdio.h>
/** 在GPU上创建新的缓冲区和顶点数组. */
void OpenGLShape::create()
{
    //为该形状生成缓冲区和顶点数组。
    glGenBuffers(1, &m_bufferID);//创建vbo
    glGenVertexArrays(1, &m_vaoID);//创建vao

    m_created = true;
}

void OpenGLShape::destroy()
{
    // 删除缓冲区和顶点数组.
    glDeleteBuffers(1, &m_bufferID);
    glDeleteVertexArrays(1, &m_vaoID);
}
//用于检测是否创建
bool OpenGLShape::checkIfCreated()
{
    if (!m_created) printf("Tried to use OpenGLShape before calling create().\n");
    return m_created;
}

/** 使用给定的顶点数据初始化缓冲区. *///给vbo中传输数据
void OpenGLShape::setVertexData(float *data, int size, int numVertices)
{
    if (!checkIfCreated()) return;

    m_numVertices = numVertices;

    // 存储顶点数据到缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);//绑定缓冲区
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);//将顶点数据复制到VBO中
    glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑
}

//启用指定的属性，并使用给定参数调用GlvertexAttribute指针
void OpenGLShape::setAttribute(
        GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset)
{
    if (!checkIfCreated()) return;

    // 绑定顶点数组和缓冲区
    glBindVertexArray(m_vaoID);//绑定vao
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

    // 定义与缓冲区数据相关的指定顶点属性。
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, (void*) offset);

    // 解绑顶点数组和缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
//绘制几何图形
void OpenGLShape::draw(GLenum drawMode)
{
    if (!checkIfCreated()) return;

    glBindVertexArray(m_vaoID);//启用
    glDrawArrays(drawMode, 0, m_numVertices);//提供绘制功能，从数组数据中提取数据渲染基本图元。
    // mode:需要渲染的图元类型   参2：从数组缓存中的哪一位开始绘制，一般为0 参3：数组中顶点的数量.
    glBindVertexArray(0);//解绑
}

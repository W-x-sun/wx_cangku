#ifndef OPENGLSHAPE_H
#define OPENGLSHAPE_H

#include "GL/glew.h"

class OpenGLShape
{
public:
    OpenGLShape() : m_created(false) { }

    virtual ~OpenGLShape() { }

    /** 在GPU上创建新的缓冲区和顶点数组. */
    void create();

    /** 从GPU中删除缓冲区和顶点数组. */
    void destroy();

    /** 使用给定的顶点数据初始化缓冲区. */
    void setVertexData(float *data, int size, int numVertices);

    /** 启用指定的属性，并使用给定参数调用GlvertexAttribute指针. */
    void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized,
                      GLsizei stride, size_t offset);

    /** 绘制初始化的几何图形. */
    void draw(GLenum drawMode);

private:
    bool checkIfCreated();

    bool m_created;

    GLuint m_bufferID; /// 顶点缓冲区的ID
    GLuint m_vaoID;    /// 顶点数组对象（VAO）的ID
    int m_numVertices; /// 要绘制的顶点数.
};

#endif // OPENGLSHAPE_H

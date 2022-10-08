#include "tessellator.h"

#include "hairfeedbackshaderprogram.h"
#include "resourceloader.h"
#include "errorchecker.h"

Tessellator::Tessellator()
{
    program = new HairFeedbackShaderProgram();
}

Tessellator::~Tessellator()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_bufferID);
    glDeleteQueries(1, &m_primitivesQuery);
    safeDelete(program);
}
   //参数：镶嵌所渲染需要的三角形数量(参数)
void Tessellator::init(int numTriangles)
{
    // 初始化镶嵌着色器程序。
    program->create();

    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_bufferID);
    glGenQueries(1, &m_primitivesQuery);

    //主要用于创建镶嵌器所需三角形对应的缓冲区 vbo
    setNumTriangles(numTriangles);

    // 启用位置和切线属性。.
    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//主要用于创建镶嵌器所需三角形对应的缓冲区 vbo
bool Tessellator::setNumTriangles(int numTriangles)
{
    if (numTriangles != m_numTriangles)
    {
        m_numTriangles = numTriangles;
        int bufferSize = numTriangles//创建对应的三角形所需缓冲区 vbo
                * 3 // 每个三角形的顶点
                * 8 // 每个顶点的浮点数 (position.xyz, tangent.xyz, colorVariation, offset
                * sizeof(GLfloat);

        // 重新初始化变换反馈缓冲区
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_COPY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }
    return false;
}

void Tessellator::beginTessellation()
{
    program->bind();//
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_bufferID);

    // Keep track of the number of primitives written.
//    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_primitivesQuery); //TODO?

    glBeginTransformFeedback(GL_TRIANGLES);
}
//结束镶嵌
void Tessellator::endTessellation()
{
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    program->unbind();
    glFlush();
}
//绘制
void Tessellator::draw()
{
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, m_numTriangles * 3);//绘制三角形 从第一个数据开始绘制  数组中顶点的数量
    glBindVertexArray(0);
}

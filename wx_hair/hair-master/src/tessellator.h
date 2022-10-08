#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include "hairCommon.h"
/****
Tessellation技术就是利用GPU硬件加速，将现有3D模型的三角形拆分得更细小、更细致，
也就是大大增加三角形数量，使得渲染对象的表面和边缘更平滑、更精细。
大幅提升模型细节和画质,bu需手动设计上百万个三角形的复杂模型，只需简单勾绘一个轮廓，
剩下的就可以交给Tessellation技术自动镶嵌，大大提高开发效率
*****/


class ShaderProgram;

class Tessellator
{
public:
    Tessellator();

    virtual ~Tessellator();

    void init(int numTriangles);

    bool setNumTriangles(int numTriangles);

    void beginTessellation();

    void endTessellation();

    void draw();

    ShaderProgram *program;

private:
    int m_numTriangles = 0;
    GLuint m_primitivesQuery;
    GLuint m_vaoID = 0;
    GLuint m_bufferID = 0;
};

#endif // TESSELLATOR_H

#ifndef HAIR_H
#define HAIR_H

#include "hairCommon.h"

#include "openglshape.h"
#include "shaderprogram.h"


class Hair
{
public:
    Hair(int numSegments, double length, glm::vec3 location, glm::vec3 dir, glm::vec3 normal);

    virtual ~Hair();

    void update(float time);
    void paint(ShaderProgram *_program);


public:
    QList<HairVertex*> m_vertices;//存储了很多根头发数据，每根头发数据存储在结构体HairVertex里（被hairinterface成员函数 更新状态标签 进行调用）

    OpenGLShape m_patch;
    int m_numSegments;//头发的分段总数
    double m_length;//长度
    glm::vec3 m_triangleFace[2];//与dir正交的平面的基向量

};

#endif // HAIR_H

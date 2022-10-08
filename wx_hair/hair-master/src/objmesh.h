#ifndef OBJMESH_H
#define OBJMESH_H

#include "hairCommon.h"
#include "openglshape.h"
#include <QThread>
#include <QtCore>
#include <limits>

class MeshOctTree;

class ObjMesh : public QObject
{
public:
    ObjMesh();

    /**
     * @param 用于初始化网格的OBJ文件
     * @param 计算期间缩放网格的比例因子（即碰撞检测）
     */
    void init(const char * objFile, float scale = 1);

    void draw();

    bool contains(glm::vec3 &normal, glm::vec3 ro, float &insideDist);

    std::vector<Triangle> triangles;

private:
    OpenGLShape m_shape;

    glm::vec3 m_min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_max = glm::vec3(std::numeric_limits<float>::min());

};

#endif // OBJMESH_H

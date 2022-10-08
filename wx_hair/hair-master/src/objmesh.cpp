#include "objmesh.h"
#include "errorchecker.h"
#include "objloader.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/random.hpp>
#include "QTime"

#include "meshocttree.h"

#define _ELLIPSOID_COLISIONS_ true

ObjMesh::ObjMesh()
{
}
//参数1：obj文件用来初始化(obj模型)头的模型（包含顶点以及顶点属性）   参数二：用来计算期间缩放网格比例因子（太小不行，头发会跑到脑壳里）
void ObjMesh::init(const char *objFile, float scale)
{
    //1.用于存储obj文件的信息
    std::vector<glm::vec3> vertices;//顶点坐标
    std::vector<glm::vec2> uvs;//纹理坐标
    std::vector<glm::vec3> normals;//顶点法向量
    bool loaded = OBJLoader::loadOBJ(objFile, vertices, uvs, normals);//参数：要加载的obj文件，对应obj文件的信息（输出:顶点 贴图 法向量）

    if (!loaded) {
        printf("Failed to load OBJ: %s\n", objFile);
        exit(1);
    }

    // 2.依据（vertices）初始化三角形   m_triangles（现在的光栅化图形渲染技术的核心是绘制大量三角形来组成3D模型）
    for (unsigned int i=0; i < vertices.size()/3; i++) {//遍历每一个三角形
        unsigned int i1 = i*3, i2 = i1+1, i3=i2+1;//每一个三角形对应的三个顶点
        Triangle t(vertices[i1] * scale, vertices[i2] * scale, vertices[i3] * scale,//三个顶点
                   uvs[i1], uvs[i2], uvs[i3],//每个顶点对应的纹理坐标
                   normals[i1], normals[i2], normals[i3]);//每个顶点对应的法向量
        triangles.push_back(t);
    }

    // 3.初始化vbo  (用加载模型得到的顶点、纹理、法向量进行初始化)
    std::vector<GLfloat> vboData;
    for (unsigned int i=0; i < vertices.size(); i++) {
        //数据中的v
        vboData.push_back(vertices[i].x);
        vboData.push_back(vertices[i].y);
        vboData.push_back(vertices[i].z);
        //数据中的vt
        vboData.push_back(uvs[i].x);
        vboData.push_back(1 - uvs[i].y);
        //数据中的vn
        vboData.push_back(normals[i].x);
        vboData.push_back(normals[i].y);
        vboData.push_back(normals[i].z);

        // 设置对象边界.（比例因子不能太大也不能太小，会影响边界）
        m_min = glm::min(m_min, scale * vertices[i]);
        m_max = glm::max(m_max, scale * vertices[i]);
    }

    m_shape.create();///** 4.在GPU上创建新的缓冲区vbo和顶点数组vao. */
    m_shape.setVertexData(&vboData[0], sizeof(GLfloat) * vboData.size(), vertices.size()); /** 使用给定的顶点数据初始化缓冲区. */
    //启用指定的属性，并使用给定参数调用GlvertexAttribute指针
    m_shape.setAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
    m_shape.setAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 3 * sizeof(GLfloat));
    m_shape.setAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 5 * sizeof(GLfloat));
}

//绘制初始化的几何三角图形
void ObjMesh::draw()
{
    m_shape.draw(GL_TRIANGLES);
}

/*
 * 从该点发射一条光线，如果它与偶数个三角形相交，则该光线位于外部
 *　如果它与奇数个三角形相交，则它位于内部
 * TODO: 更好地生成随机向量
 */
bool ObjMesh::contains(glm::vec3 &normal, glm::vec3 ro, float &insideDist)
{
#if _ELLIPSOID_COLISIONS_
#define _SQV_(v) glm::pow(v, glm::vec3(2.f))
#define _SUM_(v) glm::dot(v, glm::vec3(1.f))


    normal = glm::normalize(ro / _SQV_(m_max));

    float phi = acos( glm::dot(normal, glm::vec3(0, 0, 1)) );
    float theta = acos( glm::dot( glm::vec3(normal.x, normal.y, 0), glm::vec3(1, 0, 0) ) );

    glm::vec3 v = glm::vec3(cos(theta) * sin(phi), sin(theta) * cos(phi), cos(phi));
    float r = 1.f / glm::sqrt(_SUM_(_SQV_( v / m_max )));
    insideDist = r - glm::length(ro);

    return _SUM_(_SQV_(ro/m_max)) < 1;

#undef _SUM_
#undef _SQV_
#else
    // 如果点在边界立方体之外，则返回false.
    if (glm::any(glm::lessThan(ro, m_min)) || glm::any(glm::greaterThan(ro, m_max)))
        return false;

    int numIntersections = 0;
    double rand1 = rand() % 1000; double rand2 = rand() % 1000; double rand3 = rand() % 1000;
    glm::vec3 randDir = glm::normalize(glm::vec3(rand1, rand2, rand3));
    randDir = glm::normalize(ro);

//    OctTree, 我认为它有效，但性能不确定
//    QList<Triangle> reducedTriangles;
//    m_octTree->getRelevantTriangles(reducedTriangles, ro, randDir);

    for (unsigned int i = 0; i < triangles.size(); ++i)
    {
        Triangle currTriangle = triangles.at(i);
        glm::vec3 intersectionPoint = glm::vec3(0.0);

        if (currTriangle.intersect(intersectionPoint, ro, randDir))
        {
            normal = (currTriangle.n1 + currTriangle.n2 + currTriangle.n3) / 3.0f;
            numIntersections++;
        }
    }
    return (numIntersections % 2);
#endif
}

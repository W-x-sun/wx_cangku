#include "hair.h"
#include "errorchecker.h"

/*
 * @file hair.cpp
 *
 * 存储顶点数据  hair类处理数据是以三角网格（撮）为单位的
 */

//每根参数（头发分段数20、　长度、　每根头发的顶点位置坐标，dir 以及法向量）
Hair::Hair(int numSegments, double length, glm::vec3 location, glm::vec3 dir, glm::vec3 normal)
{
    if (numSegments < 1)
        cerr << "Number of hair segments should be at least 1" << endl;

    if (length <= 0)
        cerr << "Hair length should be positive" << endl;

    dir = glm::normalize(dir);

    // 计算与dir正交的平面的基向量.（dir垂直于这两个向量）
    m_triangleFace[0] = glm::normalize(glm::vec3(-normal.y, normal.x, 0));
    m_triangleFace[1] = glm::cross(normal, m_triangleFace[0]);

    m_numSegments = numSegments;//传进来固定值20
    m_length = length;

    double stepSize = (double) length / numSegments;//分割后每一小截头发的长度
    for (int i = 0; i < numSegments + 1; ++i)//遍历每根头发的每小段
    {   //给存储每一根头发的结构体变量newVert 赋值
        HairVertex *newVert = new HairVertex(glm::vec3(location.x, location.y, location.z) + dir * (float) (stepSize * i));
        if (i > 0)
        {
            HairVertex *oldVert = m_vertices.at(i - 1);
            double dot = CLAMP(glm::dot(oldVert->position - newVert->position, glm::vec3(0, -1, 0)), -1.0, 1.0);
            double det = CLAMP(glm::dot(oldVert->position - newVert->position, glm::vec3(1, 0, 0)), -1.0, 1.0);
            newVert->theta = -atan2(det, dot);
            newVert->pointVector = dir;
        }
        newVert->segLen = stepSize;//分割后每一段的长度
        m_vertices.append(newVert);//m_vertices顶点列表存储很多头发的数据（存储每个三角形里生成的每一根头发的结构体）  （在paint函数里调用）
        
    }



    GLfloat data[] = {-.5, +.5, 0,
                      +.5, +.5, 0,
                      -.5, -.5, 0,
                      +.5, -.5, 0};

    m_patch.create();//创建vao vbo
    m_patch.setVertexData(data, sizeof(data), 4);//绑定vao、vbo  并给vbo中传输数据
    m_patch.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//配置属性指针glVertexAttribPointer并Enable

}

Hair::~Hair()
{
    m_patch.destroy();
    for (int i = 0; i < m_vertices.size(); ++i)
        delete m_vertices.at(i);
}


void Hair::update(float time)
{

}
//（每一根）头发的绘制函数（依据着色器里的输入 来绘制头发）
void Hair::paint(ShaderProgram *_program)
{
    _program->uniforms.triangleFace[0] = m_triangleFace[0];//triangleFace 与头发法向量正交的平面的基向量(与dir正交的平面的基向量)
    _program->uniforms.triangleFace[1] = m_triangleFace[1];
    _program->uniforms.numHairVertices = MIN(m_vertices.size(), MAX_HAIR_VERTICES);//每撮头发的顶点数据（vao）numHairVertices
    _program->uniforms.length = m_length;
    for (int i = 0; i < _program->uniforms.numHairVertices; i++){
        _program->uniforms.vertexData[i] = m_vertices.at(i)->position;//保存每一个顶点数据到着色器里
    }
    _program->setPerDrawUniforms();

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    m_patch.draw(GL_PATCHES);//绘制几何图形

}



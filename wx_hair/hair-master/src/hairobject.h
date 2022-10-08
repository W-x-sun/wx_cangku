#ifndef HAIROBJECT_H
#define HAIROBJECT_H

#include "QList"
#include "hairCommon.h"
#include "shaderprogram.h"
#include "objmesh.h"

class Hair;
class Simulation;
class Texture;

class HairObject
{
public:
    ~HairObject();
    
    // 用于根据创造的头发（贴图）在网格上生成头发
    HairObject(ObjMesh *mesh,//obj（头）模型
               float hairsPerUnitArea,//头发密度
               float maxHairLength,//头发长度
               QImage &hairGrowthMap,//用于生成头发的贴图
               QImage &hairGroomingMap,//渲染窗口Direction Map的背景图
               Simulation *simulation,
               HairObject *oldObject = NULL,
               int _face = 0 );//初始化状态为双面（值为0）
    
    
    void update(float _time);
    void paint(ShaderProgram *program);
    void setAttributes(HairObject *_oldObject);//传入一个对象指针
    void setAttributes(//这里直接给参数初始化了  （调用此函数的时候可以传别的参数对应的值）
            glm::vec3 _color = glm::vec3(15./255, .286f, .202f),
            int _numGroupHairs = 64,
            float _hairGroupSpread = 0.08,
            float _hairRadius = 0.001f,
            float _noiseAmplitude = 0.15f,
            float _noiseFrequency = 0.2f,
            int _numSplineVertices = 20,
            float _shadowIntensity = 20,
            float _diffuseIntensity = 1,
            float _specularIntensity = .5,
            float _transparency = .25,
            float _useHairColorVariation = true,
            float _hairColorVariation = 1.5f);
    
public:
    
    QList<Hair*> m_guideHairs;//其.size()用来表示多少撮头发(被hairinterface成员函数updateStatsLabel调用来计算总头发数量，更新状态标签的显示)
    
    QList<glm::vec3> poses;//自定义（用来存放 pos坐标的 向量容器）
    QList<glm::vec3> normals;
    QList<glm::vec3> dirs;
    QColor hairGrowth;

    Simulation *m_simulation;
    
    QImage m_hairGrowthMap;
    QImage m_hairGroomingMap;
    Texture *m_blurredHairGrowthMapTexture;
    
    //int m_numGuideHairs;没用到，可以注释掉
    //int m_numHairVertices;

    //1.几何构造模块里的控件 的对应参数（赋值：被hairinterface类成员函数setHairObject调用其对象m_hairObject指针进行参数赋值）
    int m_numHairsPerPatch;//每一撮头发的数量（滑动条的值）
    int m_numGroupHairs;// 每一撮头发的数量 （文本框的值）  更新状态：被hairinterface成员函数updateStatsLabel调用
    float m_hairGroupSpread;//扩散程度
    float m_hairRadius;//头发半径
    float m_noiseAmplitude;//毛躁
    float m_noiseFrequency;//自然卷
    int m_numSplineVertices;//样条顶点数，更新状态标签的时候也会被HairInterface::updateStatsLabel调用

    bool m_useHairColorVariation;//选择框的选择，被HairInterface::toggleHairColorVariation调用

    //头发颜色的参数赋值（被hairinterface类成员函数setHairObject调用传入其对象m_hairObject指针进行参数赋值）
    glm::vec3 m_color;
    //2.渲染模块参数（被hairinterface类成员函数setHairObject调用传入其对象m_hairObject指针进行参数赋值）
    float m_shadowIntensity;
    float m_diffuseIntensity;
    float m_specularIntensity;
    float m_transparency;
    float m_hairColorVariation;
};

#endif // HAIROBJECT_H

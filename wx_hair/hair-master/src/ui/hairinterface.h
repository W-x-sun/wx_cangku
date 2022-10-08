#ifndef HAIRINTERFACE_H
#define HAIRINTERFACE_H

#include <QObject>
#include <QTime>

namespace Ui {
    class MainWindow;
}

class HairObject;//头发 类
class GLWidget;//绘制窗口 类
class ObjMesh;//网格 类


class HairInterface : public QObject
{
    Q_OBJECT

public:
    HairInterface(Ui::MainWindow *ui);
    virtual ~HairInterface() { }
    //给对应的参数赋值
    void setGLWidget(GLWidget *glWidget);
    void setHairObject(HairObject *hairObject);
    void setMesh(ObjMesh *mesh);
    //更新
    void updateFPSLabel(int totalNumFrames);
    void updateFPSLabelPaused(float fps);
    void updateStatsLabel();

public slots:
    void showHideGroupSim();//是否隐藏仿真模块内容
    void showHideGroupTess();//是否隐藏几何结构模块内容
    void showHideGroupRender();//是否隐藏渲染模块内容
    
    void resetSimulation();
    void resetAllValues();

    void inputHairsPerPatchText(QString text);
    void setHairsPerPatch(int);
    void inputHairGroupSpreadText(QString text);
    void setHairGroupSpread(int);
    void inputSplineVerticesText(QString text);
    void setSplineVertices(int numVertices);
    void inputHairRadiusText(QString text);
    void setHairRadius(int value);
    void inputNoiseAmpText(QString text);
    void setNoiseAmp(int value);
    void inputNoiseFreqText(QString text);
    void setNoiseFreq(int value);
    void inputHairColorRText(QString text);
    void setHairColorR(int value);
    void inputHairColorGText(QString text);
    void setHairColorG(int value);
    void inputHairColorBText(QString text);
    void setHairColorB(int value);
    void inputWindMagnitudeText(QString text);
    void setWindMagnitude(int value);
    void inputShadowIntensityText(QString text);
    void setShadowIntensity(int value);
    void inputDiffuseIntensityText(QString text);
    void setDiffuseIntensity(int value);
    void inputSpecularIntensityText(QString text);
    void setSpecularIntensity(int value);
    void inputStiffnessText(QString text);
    void setStiffness(int value);
    void inputTransparencyText(QString text);
    void setTransparency(int value);
    void setHairColorVariation(int value);
    void inputHairColorVariationText(QString text);
    void inputWindDirectionXText(QString value);
    void inputWindDirectionYText(QString value);
    void inputWindDirectionZText(QString value);
    
    void setShadows(bool);
    void setSupersampling(bool);
    void setFrictionSim(bool);
    void toggleTransparency(bool checked);
    void toggleHairColorVariation(bool checked);
    
    void togglePaused();
    void startEditScene();

    void right_genhair();
    void left_genhair();


private:
    void connectUserInputs();//（用户和主窗口ui的交互接口：１仿真、２几何构造、３渲染）　连接用户对于主窗口的发型参数的输入信号和槽


    Ui::MainWindow *m_ui;
    //先在HairInterface的构造函数里初始化为null 再在相应的成员函数 里进行赋值
    HairObject *m_hairObject;
    ObjMesh *m_mesh;
    GLWidget *m_glWidget;

    QTime m_clock;


};



#endif // HAIRINTERFACE_H

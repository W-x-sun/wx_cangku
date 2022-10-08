#ifndef HAIRINTERFACE2_H
#define HAIRINTERFACE2_H

#include <QObject>
#include <QTime>



class HairInterface : public QObject
{
    Q_OBJECT


};

namespace Ui {
    class MainWindow;
}
class HairObject;
class GLWidget;
class ObjMesh;

class HairInterface2 : public QObject
{
    Q_OBJECT
public:
    HairInterface2(Ui::MainWindow *ui);
    virtual ~HairInterface2() { }

    void setGLWidget(GLWidget *glWidget);
    void setHairObject(HairObject *hairObject);
    void setMesh(ObjMesh *mesh);
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

private:
    void connectUserInputs();

    Ui::MainWindow *m_ui;
    HairObject *m_hairObject;
    ObjMesh *m_mesh;
    GLWidget *m_glWidget;
    QTime m_clock;

};

#endif // HAIRINTERFACE2_H

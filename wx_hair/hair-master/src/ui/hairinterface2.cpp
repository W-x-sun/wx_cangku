#include "hairinterface2.h"

#include "ui_mainwindow.h"



#include "ui_sceneeditor.h"
#include "sceneeditor.h"

#include "glwidget.h"
#include "hairobject.h"
#include "hair.h"
#include "simulation.h"
#include "objmesh.h"

/********    本文件：成员函数的实现（主要包含主窗口相关功能的对应接口实现） hairobject的参数初始化  *********/

HairInterface2::HairInterface2(Ui::MainWindow *ui)
{
    m_ui = ui;
    m_glWidget = NULL;
    m_hairObject = NULL;//发型对象
    m_mesh = NULL;//网格（setMesh函数的参数）
    connectUserInputs();//用户和主窗口ui的交互接口(信号槽集合)：１仿真、２几何构造、３渲染

    //布局和一些窗体控件的设置
    m_ui->testHolder->setAlignment(Qt::AlignTop);//AlignTop:垂直方向靠上
    m_ui->scrollArea->setWidget(m_ui->controlsBox);
    m_ui->scrollArea->setFrameShape(QFrame::NoFrame);//设置滚动轴没有边框

    // to start with a group shown, use one of these:使用其中一个进行三个模块的隐藏内容展示
    //    showHideGroupSim();仿真模块
    //    showHideGroupTess();几何结构模块
    //    showHideGroupRender();渲染模块

    // to start with a group hidden, use one of these:用于三个模块的隐藏内容的功能
   m_ui->groupTess->hide();
   m_ui->groupRender->hide();
    m_ui->groupSim->hide();
}

//（用户和主窗口ui的交互接口：１仿真、２几何构造、３渲染）　连接用户对于主窗口的发型参数的输入信号和槽
void HairInterface::connectUserInputs()
{
    //三个模块是否隐藏
    connect(m_ui->showHideGroupSim, SIGNAL(pressed()), this, SLOT(showHideGroupSim()));
    connect(m_ui->showHideGroupTess, SIGNAL(pressed()), this, SLOT(showHideGroupTess()));
    connect(m_ui->showHideGroupRender, SIGNAL(pressed()), this, SLOT(showHideGroupRender()));
    //重设所有参数值（槽函数里调用resetSimulation（）重新进行仿真，并更新状态标签）
    connect(m_ui->actionReset_All_Values, SIGNAL(triggered()), this, SLOT(resetAllValues()));
    //１．几何构造模块接口
    connect(m_ui->sliderHairsPerPatch, SIGNAL(valueChanged(int)), this, SLOT(setHairsPerPatch(int)));
    connect(m_ui->inputHairsPerPatch, SIGNAL(textChanged(QString)), this, SLOT(inputHairsPerPatchText(QString)));
    connect(m_ui->sliderHairGroupSpread, SIGNAL(valueChanged(int)), this, SLOT(setHairGroupSpread(int)));
    connect(m_ui->inputHairGroupSpread, SIGNAL(textChanged(QString)), this, SLOT(inputHairGroupSpreadText(QString)));
    connect(m_ui->sliderSplineVertices, SIGNAL(valueChanged(int)), this, SLOT(setSplineVertices(int)));
    connect(m_ui->inputSplineVertices, SIGNAL(textChanged(QString)), this, SLOT(inputSplineVerticesText(QString)));
    connect(m_ui->sliderHairRadius, SIGNAL(valueChanged(int)), this, SLOT(setHairRadius(int)));
    connect(m_ui->inputHairRadius, SIGNAL(textChanged(QString)), this, SLOT(inputHairRadiusText(QString)));
    connect(m_ui->sliderNoiseAmp, SIGNAL(valueChanged(int)), this, SLOT(setNoiseAmp(int)));
    connect(m_ui->inputNoiseAmp, SIGNAL(textChanged(QString)), this, SLOT(inputNoiseAmpText(QString)));
    connect(m_ui->sliderNoiseFreq, SIGNAL(valueChanged(int)), this, SLOT(setNoiseFreq(int)));
    connect(m_ui->inputNoiseFreq, SIGNAL(textChanged(QString)), this, SLOT(inputNoiseFreqText(QString)));
    //头发颜色的设置　　（ｒｇｂ模式，备选，ＵＩ界面用的ｈｓｖ模式）
    connect(m_ui->sliderHairColorR, SIGNAL(valueChanged(int)), this, SLOT(setHairColorR(int)));
    connect(m_ui->inputHairColorR, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorRText(QString)));
    connect(m_ui->sliderHairColorG, SIGNAL(valueChanged(int)), this, SLOT(setHairColorG(int)));
    connect(m_ui->inputHairColorG, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorGText(QString)));
    connect(m_ui->sliderHairColorB, SIGNAL(valueChanged(int)), this, SLOT(setHairColorB(int)));
    connect(m_ui->inputHairColorB, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorBText(QString)));
    //２.仿真模块接口
    connect(m_ui->sliderWindMagnitude, SIGNAL(valueChanged(int)), this, SLOT(setWindMagnitude(int)));
    connect(m_ui->inputWindMagnitude, SIGNAL(textChanged(QString)), this, SLOT(inputWindMagnitudeText(QString)));
    connect(m_ui->sliderStiffness, SIGNAL(valueChanged(int)), this, SLOT(setStiffness(int)));
    connect(m_ui->inputStiffness, SIGNAL(textChanged(QString)), this, SLOT(inputStiffnessText(QString)));
    connect(m_ui->inputWindDirectionX, SIGNAL(textChanged(QString)), this, SLOT(inputWindDirectionXText(QString)));
    connect(m_ui->inputWindDirectionY, SIGNAL(textChanged(QString)), this, SLOT(inputWindDirectionYText(QString)));
    connect(m_ui->inputWindDirectionZ, SIGNAL(textChanged(QString)), this, SLOT(inputWindDirectionZText(QString)));
    //3.渲染模块接口
    connect(m_ui->sliderShadowIntensity, SIGNAL(valueChanged(int)), this, SLOT(setShadowIntensity(int)));
    connect(m_ui->inputShadowIntensity, SIGNAL(textChanged(QString)), this, SLOT(inputShadowIntensityText(QString)));
    connect(m_ui->sliderDiffuseIntensity, SIGNAL(valueChanged(int)), this, SLOT(setDiffuseIntensity(int)));
    connect(m_ui->inputDiffuseIntensity, SIGNAL(textChanged(QString)), this, SLOT(inputDiffuseIntensityText(QString)));
    connect(m_ui->sliderSpecularIntensity, SIGNAL(valueChanged(int)), this, SLOT(setSpecularIntensity(int)));
    connect(m_ui->inputSpecularIntensity, SIGNAL(textChanged(QString)), this, SLOT(inputSpecularIntensityText(QString)));
    connect(m_ui->sliderTransparency, SIGNAL(valueChanged(int)), this, SLOT(setTransparency(int)));
    connect(m_ui->inputTransparency, SIGNAL(textChanged(QString)), this, SLOT(inputTransparencyText(QString)));
    connect(m_ui->sliderHairColorVariation, SIGNAL(valueChanged(int)), this, SLOT(setHairColorVariation(int)));
    connect(m_ui->inputHairColorVariation, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorVariationText(QString)));
    // 是否选择（摩擦（仿真模块）、阴影、超采样、颜色种类、透明度）选项切换 toggle
    connect(m_ui->frictionSimCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFrictionSim(bool)));
    connect(m_ui->shadowCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShadows(bool)));
    connect(m_ui->supersampleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSupersampling(bool)));
    connect(m_ui->transparencyCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleTransparency(bool)));
    connect(m_ui->hairColorVariationCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleHairColorVariation(bool)));
    //按钮功能接口 buttons(暂停按钮、重新仿真按钮、场景编辑按钮)
    connect(m_ui->pauseButton, SIGNAL(pressed()), this, SLOT(togglePaused()));
    connect(m_ui->buttonResetSim, SIGNAL(pressed()), this, SLOT(resetSimulation()));
    connect(m_ui->sceneEditorButton, SIGNAL(pressed()), this, SLOT(startEditScene()));
}

//设置m_glWidget仿真模型窗口
void HairInterface::setGLWidget(GLWidget *glWidget)
{
    m_glWidget = glWidget;
}

//网格mesh
void HairInterface::setMesh(ObjMesh *mesh)
{
    m_mesh = mesh;
}

//对（仿真模型）实例发型对象　的各个参数值的初始化设置（各个模块包含的功能对应文本框以及滑动条的值的设置）
void HairInterface::setHairObject(HairObject *hairObject)//参数表示创建的发型实例
{
    m_hairObject = hairObject;
    //几何构造模块
    m_ui->sliderHairsPerPatch->setValue(m_hairObject->m_numGroupHairs);
    m_ui->inputHairsPerPatch->setText(QString::number(m_hairObject->m_numGroupHairs));
    m_ui->sliderHairGroupSpread->setValue(m_hairObject->m_hairGroupSpread*1000);
    m_ui->inputHairGroupSpread->setText(QString::number(m_hairObject->m_hairGroupSpread));
    m_ui->sliderSplineVertices->setValue(m_hairObject->m_numSplineVertices);
    m_ui->inputSplineVertices->setText(QString::number(m_hairObject->m_numSplineVertices));
    m_ui->sliderHairRadius->setValue(m_hairObject->m_hairRadius*10000);
    m_ui->inputHairRadius->setText(QString::number(m_hairObject->m_hairRadius));
    m_ui->sliderNoiseAmp->setValue(m_hairObject->m_noiseAmplitude*100);
    m_ui->inputNoiseAmp->setText(QString::number(m_hairObject->m_noiseAmplitude));
    m_ui->sliderNoiseFreq->setValue(m_hairObject->m_noiseFrequency*100);
    m_ui->inputNoiseFreq->setText(QString::number(m_hairObject->m_noiseFrequency));
    //头发颜色设置
    m_ui->sliderHairColorR->setValue(m_hairObject->m_color.x*2550);
    m_ui->sliderHairColorG->setValue(m_hairObject->m_color.y*2550);
    m_ui->sliderHairColorB->setValue(m_hairObject->m_color.z*2550);
    m_ui->inputHairColorR->setText(QString::number(m_hairObject->m_color.x*255, 'g', 2));
    m_ui->inputHairColorG->setText(QString::number(m_hairObject->m_color.y, 'g', 2));
    m_ui->inputHairColorB->setText(QString::number(m_hairObject->m_color.z, 'g', 2));
    //仿真模块
    m_ui->sliderWindMagnitude->setValue(m_glWidget->m_testSimulation->m_windMagnitude*100);
    m_ui->inputWindMagnitude->setText(QString::number(m_glWidget->m_testSimulation->m_windMagnitude, 'g', 3));
    m_ui->sliderStiffness->setValue(m_glWidget->m_testSimulation->m_stiffness*1000);
    m_ui->inputStiffness->setText(QString::number(m_glWidget->m_testSimulation->m_stiffness, 'g', 4));
    m_ui->inputWindDirectionX->setText(QString::number(m_glWidget->m_testSimulation->m_windDir.x, 'g', 4));
    m_ui->inputWindDirectionY->setText(QString::number(m_glWidget->m_testSimulation->m_windDir.y, 'g', 4));
    m_ui->inputWindDirectionZ->setText(QString::number(m_glWidget->m_testSimulation->m_windDir.z, 'g', 4));

    //渲染模块
    m_ui->sliderShadowIntensity->setValue(m_glWidget->m_hairObject->m_shadowIntensity*10);
    m_ui->inputShadowIntensity->setText(QString::number(m_glWidget->m_hairObject->m_shadowIntensity, 'g', 3));
    m_ui->sliderDiffuseIntensity->setValue(m_glWidget->m_hairObject->m_diffuseIntensity*100);
    m_ui->inputDiffuseIntensity->setText(QString::number(m_glWidget->m_hairObject->m_diffuseIntensity, 'g', 3));
    m_ui->sliderSpecularIntensity->setValue(m_glWidget->m_hairObject->m_specularIntensity*100);
    m_ui->inputSpecularIntensity->setText(QString::number(m_glWidget->m_hairObject->m_specularIntensity, 'g', 3));
    m_ui->sliderTransparency->setValue(m_hairObject->m_transparency*1000);
    m_ui->inputTransparency->setText(QString::number(m_hairObject->m_transparency, 'g', 4));
    m_ui->sliderHairColorVariation->setValue(m_hairObject->m_hairColorVariation*1000);
    m_ui->inputHairColorVariation->setText(QString::number(m_hairObject->m_hairColorVariation, 'g', 4));

    // Sync toggles复选框切换　　　　　是否选择（阴影、超采样、颜色种类、透明度）选项切换
    m_ui->frictionSimCheckBox->setChecked(m_glWidget->useFrictionSim);
    m_ui->shadowCheckBox->setChecked(m_glWidget->useShadows);
    m_ui->supersampleCheckBox->setChecked(m_glWidget->useSupersampling);
    m_ui->transparencyCheckBox->setChecked(m_glWidget->useTransparency);
    m_ui->hairColorVariationCheckBox->setChecked(m_hairObject->m_useHairColorVariation);

    updateStatsLabel();//更新标签的状态
}

//给fps标签实时显示内容（显示帧率）
void HairInterface::updateFPSLabel(int totalNumFrames)
{
    int updateFrequency = 5;
    if (totalNumFrames % updateFrequency == 1) {
        if (totalNumFrames > 1) {
            double fps = updateFrequency * 1000.0 / m_clock.elapsed();
            m_ui->fpsLabel->setText(QString::number(fps, 'f', 1) + " FPS");
        }
        m_clock.restart();
    }
}

//暂停按钮按下的时候，帧率label应当显示的内容（帧率）
void HairInterface::updateFPSLabelPaused(float fps)
{
    m_ui->fpsLabel->setText(QString::number(fps, 'f', 1) + " FPS");
}

//更新状态label标签的状态【引导头发(头发方向) 、渲染头发 、模拟顶点 、渲染三角形（角度）】
void HairInterface::updateStatsLabel()
{
    // Update stats label.
    int numGuideHairs = m_hairObject->m_guideHairs.size();
    int numGroupHairs = m_hairObject->m_numGroupHairs;
    int numGuideVertices = m_hairObject->m_guideHairs[0]->m_vertices.size();
    int numSplineVertices = m_hairObject->m_numSplineVertices;
    m_ui->statsLabel->setText(
                QString::number(numGuideHairs) + " guide hairs\n" +
                QString::number(numGuideHairs * numGroupHairs) + " rendered hairs\n" +
                QString::number(numGuideHairs * numGuideVertices) + " simulated vertices\n" +
                QString::number(numGuideHairs * numGroupHairs * (numSplineVertices-1) * 2) + " rendered triangles");
}

//重新仿真,并更新状态label标签
void HairInterface::resetSimulation()
{
    //当场景编辑没有打开使用的时候
    if (m_glWidget->m_sceneEditor == NULL){
        m_glWidget->resetSimulation();
        m_glWidget->forceUpdate();//如果当前状态为暂停，则重新绘制场景。
        updateStatsLabel();//更新状态label标签的状态【引导头发(头发方向) 、渲染头发 、模拟顶点 、渲染三角形（角度）】
    }
}

//是否隐藏仿真模块内容
void HairInterface::showHideGroupSim()
{
    if (m_ui->groupSim->isVisible()){
        m_ui->groupSim->hide();
        m_ui->showHideGroupSim->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupSim->show();
        m_ui->showHideGroupSim->setIcon(QIcon(":/images/chevron_down"));
    }
}
//是否隐藏几何结构模块内容
void HairInterface::showHideGroupTess()
{
    if (m_ui->groupTess->isVisible()){
        m_ui->groupTess->hide();
        m_ui->showHideGroupTess->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupTess->show();
        m_ui->showHideGroupTess->setIcon(QIcon(":/images/chevron_down"));
    }
}
//是否隐藏渲染模块内容
void HairInterface::showHideGroupRender()
{
    if (m_ui->groupRender->isVisible()){
        m_ui->groupRender->hide();
        m_ui->showHideGroupRender->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupRender->show();
        m_ui->showHideGroupRender->setIcon(QIcon(":/images/chevron_down"));
    }
}

//每一撮头发　文本框输入（并进行对应滑动条的同步）
void HairInterface::inputHairsPerPatchText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    int value = text.toInt(&ok);
    if (!ok){
        value = m_hairObject->m_numGroupHairs;
    } else if (value == m_hairObject->m_numGroupHairs) return;
    setHairsPerPatch(value);//进行对应滑动条的同步
}
//每一撮头发  滑动条数值设置（１．同步到对应文本框　　２．更新状态label进行显示　３．并重新绘制场景）
void HairInterface::setHairsPerPatch(int numHairs)
{
    m_hairObject->m_numGroupHairs = numHairs;
    m_ui->inputHairsPerPatch->setText(QString::number(numHairs));//同步到对应文本框
    m_ui->sliderHairsPerPatch->setValue(numHairs);
    updateStatsLabel();//更新状态label进行显示
    m_glWidget->forceUpdate();//若为暂停状态，重新绘制场景
}

//头发爆炸程度　文本框输入（并进行对应滑动条的同步）
void HairInterface::inputHairGroupSpreadText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_hairGroupSpread;
    } else if (value == m_hairObject->m_hairGroupSpread) return;
    if (value == 0) return;
    setHairGroupSpread(1000*value);//同步到对应滑动条（槽）
    m_ui->sliderHairGroupSpread->setValue(1000*value);//值若发生变化，则触发信号
}
//头发爆炸程度滑动条　（１．同步到对应文本框　　２．并重新绘制场景）
void HairInterface::setHairGroupSpread(int value)
{
    m_hairObject->m_hairGroupSpread = value/1000.;
    m_ui->inputHairGroupSpread->setText(QString::number(m_hairObject->m_hairGroupSpread, 'g', 4));
    m_glWidget->forceUpdate();
}

//样条定点（发际线）　文本框输入（并进行对应滑动条的同步）
void HairInterface::inputSplineVerticesText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    int value = text.toInt(&ok);
    if (!ok){
        value = m_hairObject->m_numSplineVertices;
    } else if (value == m_hairObject->m_numSplineVertices) return;
    setSplineVertices(value);//并进行对应滑动条的同步
}
//样条定点（发际线）滑动条　（１．同步到对应文本框　　２．更新状态label进行显示　３．并重新绘制场景）
void HairInterface::setSplineVertices(int numVertices)
{
    //    if (value <= 0) return;
    m_hairObject->m_numSplineVertices = numVertices;
    m_ui->inputSplineVertices->setText(QString::number(numVertices));
    m_ui->sliderSplineVertices->setValue(numVertices);
    updateStatsLabel();
    m_glWidget->forceUpdate();
}
// 头发半径
void HairInterface::inputHairRadiusText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_hairRadius;
    } else if (value == m_hairObject->m_hairRadius) return;
    if (value == 0) return;
    setHairRadius(10000*value);
    m_ui->sliderHairRadius->setValue(10000*value);
}
void HairInterface::setHairRadius(int value)
{
    m_hairObject->m_hairRadius = value/10000.;
    m_ui->inputHairRadius->setText(QString::number(m_hairObject->m_hairRadius, 'g', 4));
    m_glWidget->forceUpdate();
}
//噪声放大器（毛躁程度）
void HairInterface::inputNoiseAmpText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_noiseAmplitude;
    } else if (value == m_hairObject->m_noiseAmplitude) return;
    setNoiseAmp(100*value);
    m_ui->sliderNoiseAmp->setValue(100*value);
}
void HairInterface::setNoiseAmp(int value)
{
    if (value < 0) return;
    m_hairObject->m_noiseAmplitude = value/100.;
    m_ui->inputNoiseAmp->setText(QString::number(m_hairObject->m_noiseAmplitude, 'g', 3));
    m_glWidget->forceUpdate();
}
//自然卷程度
void HairInterface::inputNoiseFreqText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_noiseFrequency;
    } else if (value == m_hairObject->m_noiseFrequency) return;
    setNoiseFreq(100*value);
    m_ui->sliderNoiseFreq->setValue(100*value);
}
void HairInterface::setNoiseFreq(int value)
{
    if (value < 0) return;
    m_hairObject->m_noiseFrequency = value/100.;
    m_ui->inputNoiseFreq->setText(QString::number(m_hairObject->m_noiseFrequency, 'g', 3));
    m_glWidget->forceUpdate();
}
//头发颜色（ＲＧＢ）
void HairInterface::inputHairColorRText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.x;
    } else if (value == m_hairObject->m_color.x) return;
    setHairColorR(10*value);
    m_ui->sliderHairColorR->setValue(10*value);
}
void HairInterface::setHairColorR(int value)
{
    if (value < 0) return;
    m_hairObject->m_color.x = value/2550.;
    m_ui->inputHairColorR->setText(QString::number(value/10, 'g', 3));
    m_glWidget->forceUpdate();
}

void HairInterface::inputHairColorGText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.y;
    } else if (value == m_hairObject->m_color.y) return;
    setHairColorG(2550*value);
    m_ui->sliderHairColorG->setValue(2550*value);
}
void HairInterface::setHairColorG(int value)
{
    if (value < 0) return;
    m_hairObject->m_color.y = value/2550.;
    m_ui->inputHairColorG->setText(QString::number(m_hairObject->m_color.y, 'g', 3));
    m_glWidget->forceUpdate();
}

void HairInterface::inputHairColorBText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.z;
    } else if (value == m_hairObject->m_color.z) return;
    setHairColorB(2550*value);
    m_ui->sliderHairColorB->setValue(2550*value);
}
void HairInterface::setHairColorB(int value)
{
    if (value < 0) return;
    m_hairObject->m_color.z = value/2550.;
    m_ui->inputHairColorB->setText(QString::number(m_hairObject->m_color.z, 'g', 3));
    m_glWidget->forceUpdate();
}
//风力
void HairInterface::inputWindMagnitudeText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windMagnitude;
    } else if (value == m_glWidget->m_testSimulation->m_windMagnitude) return;
    setWindMagnitude(100*value);
    m_ui->sliderWindMagnitude->setValue(100*value);
}
void HairInterface::setWindMagnitude(int value)
{
    if (value < 0) return;
    m_glWidget->m_testSimulation->m_windMagnitude = value/100.;
    m_ui->inputWindMagnitude->setText(QString::number(m_glWidget->m_testSimulation->m_windMagnitude, 'g', 3));
}
//方向（ｘｙｚ）
void HairInterface::inputWindDirectionXText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windDir.x;
    } else if (value == m_glWidget->m_testSimulation->m_windDir.x) return;
    m_glWidget->m_testSimulation->m_windDir.x = value;
}
void HairInterface::inputWindDirectionYText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windDir.y;
    } else if (value == m_glWidget->m_testSimulation->m_windDir.y) return;
    m_glWidget->m_testSimulation->m_windDir.y = value;
}
void HairInterface::inputWindDirectionZText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windDir.z;
    } else if (value == m_glWidget->m_testSimulation->m_windDir.z) return;
    m_glWidget->m_testSimulation->m_windDir.z = value;
}
//阴影
void HairInterface::inputShadowIntensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_hairObject->m_shadowIntensity;
    } else if (value == m_glWidget->m_hairObject->m_shadowIntensity) return;
    setShadowIntensity(10*value);
    m_ui->sliderShadowIntensity->setValue(10*value);
}
void HairInterface::setShadowIntensity(int value)
{
    if (value < 0) return;
    m_glWidget->m_hairObject->m_shadowIntensity = value/10.;
    m_ui->inputShadowIntensity->setText(QString::number(m_glWidget->m_hairObject->m_shadowIntensity, 'g', 3));
    m_glWidget->forceUpdate();
}
//颜色扩散水平（上色深浅程度）
void HairInterface::inputDiffuseIntensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_hairObject->m_diffuseIntensity;
    } else if (value == m_glWidget->m_hairObject->m_diffuseIntensity) return;
    setDiffuseIntensity(100*value);
    m_ui->sliderDiffuseIntensity->setValue(100*value);
}
void HairInterface::setDiffuseIntensity(int value)
{
    if (value < 0) return;
    m_glWidget->m_hairObject->m_diffuseIntensity = value/100.;
    m_ui->inputDiffuseIntensity->setText(QString::number(m_glWidget->m_hairObject->m_diffuseIntensity, 'g', 3));
    m_glWidget->forceUpdate();
}
//镜面
void HairInterface::inputSpecularIntensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_hairObject->m_specularIntensity;
    } else if (value == m_glWidget->m_hairObject->m_specularIntensity) return;
    setSpecularIntensity(100*value);
    m_ui->sliderSpecularIntensity->setValue(100*value);
}
void HairInterface::setSpecularIntensity(int value)
{
    if (value < 0) return;
    m_glWidget->m_hairObject->m_specularIntensity = value/100.;
    m_ui->inputSpecularIntensity->setText(QString::number(m_glWidget->m_hairObject->m_specularIntensity, 'g', 3));
    m_glWidget->forceUpdate();
}
//刚度（头发硬度）
void HairInterface::inputStiffnessText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_stiffness;
    } else if (value == m_glWidget->m_testSimulation->m_stiffness) return;
    setStiffness(1000*value);
    m_ui->sliderStiffness->setValue(1000*value);
}
void HairInterface::setStiffness(int value)
{
    if (value < 0) return;
    m_glWidget->m_testSimulation->m_stiffness = value/1000.;
    m_ui->inputStiffness->setText(QString::number(m_glWidget->m_testSimulation->m_stiffness, 'g', 3));
}
//透明度（颜色深浅）
void HairInterface::inputTransparencyText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_transparency;
    } else if (value == m_hairObject->m_transparency) return;
    setTransparency(1000*value);
    m_ui->sliderTransparency->setValue(1000*value);
}
void HairInterface::setTransparency(int value)
{
    if (value < 0) return;
    m_hairObject->m_transparency = value/1000.;
    m_ui->inputTransparency->setText(QString::number(m_hairObject->m_transparency, 'g', 3));
    m_glWidget->forceUpdate();
}
//颜色种类
void HairInterface::inputHairColorVariationText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_hairColorVariation;
    } else if (value == m_hairObject->m_hairColorVariation) return;
    setHairColorVariation(1000*value);
    m_ui->sliderHairColorVariation->setValue(1000*value);
}
void HairInterface::setHairColorVariation(int value)
{
    if (value < 0) return;
    m_hairObject->m_hairColorVariation= value/1000.;
    m_ui->inputHairColorVariation->setText(QString::number(m_hairObject->m_hairColorVariation, 'g', 3));
    m_glWidget->forceUpdate();
}


//是否　设置阴影
void HairInterface::setShadows(bool checked)
{
    m_glWidget->useShadows = checked;
    m_glWidget->forceUpdate();//更新
}
//是否　设置超采样
void HairInterface::setSupersampling(bool checked)
{
    m_glWidget->useSupersampling = checked;
    m_glWidget->forceUpdate();//更新
}
//是否　设置摩擦（仿真模块）
void HairInterface::setFrictionSim(bool checked)
{
    m_glWidget->useFrictionSim = checked;
}
//是否　设置透明度
void HairInterface::toggleTransparency(bool checked)
{
    m_glWidget->useTransparency = checked;
    m_glWidget->forceUpdate();
}
//是否　设置颜色种类
void HairInterface::toggleHairColorVariation(bool checked)
{
    m_hairObject->m_useHairColorVariation = checked;
    m_glWidget->forceUpdate();
}

//暂停按钮　的触发槽函数，实现仿真模型的仿真暂停
void HairInterface::togglePaused()
{
    if (m_glWidget->isPaused())
    {
        m_glWidget->unpause();//没暂停的状态
        m_ui->pauseButton->setText("Pause");
    }

    else
    {
        m_glWidget->pause();//点击暂停
        m_ui->pauseButton->setText("Continue");
    }
}

//场景编辑初始化（设置场景编辑的子窗口、显示、激活窗口等操作）
void HairInterface::startEditScene(){
    if (m_glWidget->m_sceneEditor == NULL){
        if (!m_glWidget->isPaused()){
            togglePaused();
        }

        m_glWidget->m_sceneEditor = new SceneEditor(m_glWidget, m_mesh);

        m_glWidget->m_sceneEditor->show();
        m_glWidget->m_sceneEditor->raise();
        m_glWidget->m_sceneEditor->activateWindow();
    }
}
//重设所有值
void HairInterface::resetAllValues(){
    m_glWidget->resetSimulation(true);
    updateStatsLabel();
}

#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include "hairCommon.h"

class ObjMesh;
class HairObject;
class Simulation;
class ShaderProgram;
class HairInterface;
class Texture;
class Framebuffer;
class SceneEditor;
class Tessellator;

class GLWidget : public QGLWidget
{
    Q_OBJECT
    friend class SceneWidget;
    friend class HairInterface;
    friend class SceneEditor;
    
public:
    GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent = 0); /*HairInterface *hairInterface,*/
    ~GLWidget();

    void resetSimulation(bool hardReset = false);
    void applySceneEditor(Texture *_hairGrowthTexture, Texture *_hairGroomingTexture);

    void pause();
    void unpause();
    bool isPaused();
    void forceUpdate(); // 如果此时处于暂停仿真状态 ，除了仿真模块参数，如果其他模块的参数变化，绘制效果也更新
    //渲染模块的选择框参数（被hairinterface成员函数sethairobject调用）
    bool useShadows = true;
    bool useSupersampling = true;//点击控件后，HairInterface::setSupersampling进行参数设置，然后被sethairobject调用读取状态
    bool useFrictionSim = true;
    bool useTransparency = true;

    int which_face;//标志位（左侧-1  双面0  右侧1）
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void initSimulation();
    void initCamera();

protected slots:
    /** 重新绘制画布。m_ timer每秒调用60次。 */
    void updateCanvas();

private:
    void _drawHair(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection, bool bindProgram = true);
    void _drawMesh(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void _drawHairFromFeedback(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void _resizeDepthPeelFramebuffers();

    bool m_paused = false;//用于判断是否为仿真暂停状态
    bool m_pausedLastFrame = true;

    HairInterface *m_hairInterface;
    SceneEditor *m_sceneEditor;

    ObjMesh *m_highResMesh, *m_lowResMesh;//cpp文件里的initSimulation函数中进行调用，两个的区别是模型的顶点数相差较大
    HairObject *m_hairObject; //被hariinterface成员函数setHairObject调用（用于给渲染模块参数赋值操作）    
    HairObject *old_Object;

    Simulation *m_testSimulation;//被hariinterface成员函数setHairObject调用（用于给仿真模块参数赋值操作）

    Tessellator *m_tessellator;

    Texture *m_noiseTexture;

    std::vector<ShaderProgram*> m_programs;//着色器容器（添加下边的着色器）
    ShaderProgram *m_hairProgram,
                  *m_meshProgram,
                  *m_hairOpacityProgram,
                  *m_whiteMeshProgram,
                  *m_whiteHairProgram,
                  *m_hairDepthPeelProgram,
                  *m_meshDepthPeelProgram,

                  // 变换反馈
                  *m_TFwhiteHairProgram,
                  *m_TFhairDepthPeelProgram,
                  *m_TFhairOpacityProgram,
                  *m_TFhairProgram;

    std::vector<Framebuffer*> m_framebuffers;//缓冲区容器（添加下边的缓冲区）
    Framebuffer *m_hairShadowFramebuffer,
                *m_meshShadowFramebuffer,
                *m_opacityMapFramebuffer,
                *m_finalFramebuffer,
                *m_depthPeel0Framebuffer,
                *m_depthPeel1Framebuffer;

    // 相机参数   投影矩阵和全局视图
    glm::mat4 m_projection, m_view;//   观察矩阵(View Matrix)里，它被用来将世界坐标变换到观察空间（摄像机空间）
    //投影矩阵(Projection Matrix) 将顶点坐标从观察(相机空间)变换到裁剪空间（用于后续判断哪些顶点将会出现在屏幕上）
    float m_zoom, m_angleX, m_angleY;
    QPoint m_prevMousePos;
    QPoint m_prevXformPos;
    QPoint m_prevRotPos;

    // Light parameters
    glm::vec3 m_lightPosition;
    glm::mat4 m_eyeToLight;

    //绘制窗口里globals模块里的参数
    float m_hairDensity;// 头发密度（构造函数给初始值150）
    float m_maxHairLength;//头发长度 （构造函数给初始值0.45）
    QTime m_clock;
    QTimer m_timer; /** 每秒钟响应６０次 */
    int m_increment; /** 每次调用paintGL时递增. */
    float m_targetFPS;//帧率

    int msec;
    
    Texture *resetFromSceneEditorGrowthTexture, *resetFromSceneEditorGroomingTexture;
};

#endif // GLWIDGET_H

#include "glwidget.h"
#include "resourceloader.h"
#include "errorchecker.h"
#include "hairCommon.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include "hairobject.h"
#include "simulation.h"
#include "hairshaderprogram.h"
#include "meshshaderprogram.h"
#include "hairopacityshaderprogram.h"
#include "whitehairshaderprogram.h"
#include "whitemeshshaderprogram.h"
#include "hairdepthpeelprogram.h"
#include "meshdepthpeelprogram.h"
#include "hairinterface.h"
#include "meshocttree.h"
#include "texture.h"
#include "framebuffer.h"
#include "tessellator.h"
#include "hairrendershaderprogram.h"

#include "sceneeditor.h"

#include <glm/gtx/color_space.hpp>

#define SHIFT_CLICK true

#define FEEDBACK true

GLWidget::GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent)
    : QGLWidget(format, parent),
      m_hairInterface(hairInterface),
      //1.渲染窗口 参数初始化
      m_hairDensity(150),//头发密度
      m_maxHairLength(.45),//头发长度
      m_timer(this),//每秒60次
      m_increment(0),///** 每次调用paintGL时递增. */
      m_targetFPS(60.f)//60帧
{
    //2.其他类的对象初始化
    m_highResMesh = NULL;//ObjMesh类
    m_lowResMesh = NULL;//ObjMesh类
    m_hairObject = NULL;//类HairObject（头发的一些属性参数）
    old_Object = NULL;
    m_testSimulation = NULL;//仿真
    m_sceneEditor = NULL;//渲染的主窗口

    which_face = 0;//初始化

    resetFromSceneEditorGrowthTexture = NULL;//纹理（其纹理值 可来自于渲染子窗口的所生成的纹理，apply按钮槽函数触发赋值）
    resetFromSceneEditorGroomingTexture = NULL;
    
    m_noiseTexture = new Texture();
    
    // 3. 着色器容器（包含各种 Shader programs）
    m_programs = {
        m_hairProgram = new HairShaderProgram(),
        m_meshProgram = new MeshShaderProgram(),
        m_hairOpacityProgram = new HairOpacityShaderProgram(),
        m_whiteHairProgram = new WhiteHairShaderProgram(),
        m_whiteMeshProgram = new WhiteMeshShaderProgram(),
        m_hairDepthPeelProgram = new HairDepthPeelShaderProgram(),
        m_meshDepthPeelProgram = new MeshDepthPeelShaderProgram(),

        // 变换反馈
        m_TFhairProgram = new HairRenderShaderProgram(),
        m_TFwhiteHairProgram = new WhiteHairFeedbackShaderProgram(),
        m_TFhairDepthPeelProgram = new HairFeedbackDepthPeelShaderProgram(),
        m_TFhairOpacityProgram = new HairFeedbackOpacityShaderProgram(),
    };
    
    // 4.帧缓冲区（容器）（包含各种对应缓冲区 ）
    m_framebuffers = {
        m_hairShadowFramebuffer = new Framebuffer(),
        m_meshShadowFramebuffer = new Framebuffer(),
        m_opacityMapFramebuffer = new Framebuffer(),
        m_finalFramebuffer = new Framebuffer(),
        m_depthPeel0Framebuffer = new Framebuffer(),
        m_depthPeel1Framebuffer = new Framebuffer(),
    };
    //5.创建Tessellator
    m_tessellator = new Tessellator();
    
    m_hairInterface->setGLWidget(this);//给用户和ui交互的窗口上设置 glwidget窗口，这样ui里可以使用glwidget
    
    // 用于更新画布    设置60 FPS的绘制循环.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateCanvas()));//通过定时器来调用widget的update函数来触发paintGl（）进行绘制
    m_timer.start(1000.0f / m_targetFPS);

}


GLWidget::~GLWidget()
{
    for (auto program = m_programs.begin(); program != m_programs.end(); ++program)
        safeDelete(*program);
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        safeDelete(*framebuffer);

    safeDelete(m_noiseTexture);
    safeDelete(m_highResMesh);
    safeDelete(m_lowResMesh);
    safeDelete(m_testSimulation);
    safeDelete(m_hairObject);

    safeDelete(old_Object);
}
//初始化
void GLWidget::initializeGL()
{
    //1、初始化ｇｌｅｗ
    ResourceLoader::initializeGlew();
    //2、启用一些flag
    glEnable(GL_DEPTH_TEST);//启用深度测试
    glEnable(GL_CULL_FACE);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    
    // 3.初始化（创建）　着色器
    for (auto program = m_programs.begin(); program != m_programs.end(); ++program)
        (*program)->create();//创建着色器程序容器里每个对应的着色器程序
    
    // 4.初始化（创建）包含给定图像的 noise纹理.
    m_noiseTexture->createColorTexture(":/images/noise128.jpg", GL_LINEAR, GL_LINEAR);
    
    // 5.初始化（创建）　帧缓冲. vbo
    int shadowMapRes = 2048;
    glm::vec2 finalSize = glm::vec2(2 * width(), 2 * height());
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        (*framebuffer)->create();//创建对应的帧缓冲
    //给每个对应的缓冲区添加所需（纹理、渲染缓冲对象）附件，能够完成 对应所需功能 的离屏渲染
    // （参数：width、height、放大缩小的过滤方式）
    m_hairShadowFramebuffer->generateDepthTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);//生成depth纹理（对纹理参数设置：像素颜色采用邻近过滤GL_NEAREST）
    m_meshShadowFramebuffer->generateDepthTexture(shadowMapRes, shadowMapRes, GL_LINEAR, GL_LINEAR);
    m_opacityMapFramebuffer->generateColorTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);//生成color纹理
    m_opacityMapFramebuffer->generateDepthBuffer(shadowMapRes, shadowMapRes);//生成depth缓存
    m_finalFramebuffer->generateColorTexture(finalSize.x, finalSize.y, GL_LINEAR, GL_LINEAR);
    m_finalFramebuffer->generateDepthBuffer(finalSize.x, finalSize.y);
    m_depthPeel0Framebuffer->generateColorTexture(finalSize.x, finalSize.y, GL_LINEAR, GL_LINEAR);
    m_depthPeel0Framebuffer->generateDepthTexture(finalSize.x, finalSize.y, GL_NEAREST, GL_NEAREST);
    m_depthPeel1Framebuffer->generateColorTexture(finalSize.x, finalSize.y, GL_LINEAR, GL_LINEAR);
    m_depthPeel1Framebuffer->generateDepthBuffer(finalSize.x, finalSize.y);
    
    // 初始化仿真.
    initSimulation();
    
    initCamera();//相机初始化（可调整观察视角）

    ErrorChecker::printGLErrors("end of initializeGL");
}
//绘制
void GLWidget::paintGL()
{
    ErrorChecker::printGLErrors("start of paintGL");
    //应用场景编辑（两个窗口的绘制纹理）
    if (resetFromSceneEditorGroomingTexture != NULL && resetFromSceneEditorGrowthTexture != NULL){

        applySceneEditor(resetFromSceneEditorGrowthTexture, resetFromSceneEditorGroomingTexture);//赋值给hairobject类
        resetFromSceneEditorGroomingTexture = NULL;//便于下次使用
        resetFromSceneEditorGrowthTexture = NULL;//便于下次使用
    }

    m_clock.restart();

    _resizeDepthPeelFramebuffers();//（判断是否超采样）调整深度PeelFrameBuffers的大小
    
    // 如果当时为仿真状态，则更新仿真.
    if (!isPaused())
    {
        m_increment++;/** 每次调用paintGL时递增. */
        float time = m_increment / (float) m_targetFPS; // 以秒为单位的时间（假设为60 FPS）。
        m_testSimulation->update(time);//更新m_time

//        old_Object->update(time);
        m_hairObject->update(time);  //对这个hairobject的对象 进行仿真效果更新
    }
    
    //每次绘制时更新  变换矩阵.
    glm::mat4 model = glm::mat4(1.f);//用于将物体的坐标将会从局部变换到世界空间
    model = m_testSimulation->m_xform;//将变换（叠加了平移旋转效果）更新后的矩阵赋值给modle矩阵

    m_lightPosition = glm::vec3(2, 2, 2);//表示光源在场景的世界空间坐标中的位置
    glm::mat4 lightProjection = glm::perspective(1.3f, 1.f, .1f, 100.f);
    glm::mat4 lightView = glm::lookAt(m_lightPosition, glm::vec3(1), glm::vec3(0,1,0));
    m_eyeToLight = lightProjection * lightView * glm::inverse(m_view);
    
    // 绑定（noise等到）对应 渲染缓冲附件 的纹理单元，并把纹理数据发送到着色器
    m_noiseTexture->bind(GL_TEXTURE0);
    m_hairShadowFramebuffer->depthTexture->bind(GL_TEXTURE1);
    m_opacityMapFramebuffer->colorTexture->bind(GL_TEXTURE2);
    m_meshShadowFramebuffer->depthTexture->bind(GL_TEXTURE3);
    m_finalFramebuffer->colorTexture->bind(GL_TEXTURE4);
    m_hairObject->m_blurredHairGrowthMapTexture->bind(GL_TEXTURE5);//这个纹理是刷子绘制后经过模糊处理创建的纹理
    m_depthPeel0Framebuffer->depthTexture->bind(GL_TEXTURE6);
    m_depthPeel0Framebuffer->colorTexture->bind(GL_TEXTURE7);
    m_depthPeel1Framebuffer->colorTexture->bind(GL_TEXTURE8);

                     //根据复选框的选择进行条件编译
#if FEEDBACK
    int numTriangles =
            m_hairObject->m_guideHairs.size()       // # guide hairs
            * m_hairObject->m_numGroupHairs         // # hairs per guide hair
            * (m_hairObject->m_numSplineVertices-1) // # segments per hair
            * 2;                                    // # triangles per segment
    m_tessellator->setNumTriangles(numTriangles);

    m_tessellator->beginTessellation();
    _drawHair(m_tessellator->program, model, m_view, m_projection, false);
    m_tessellator->endTessellation();
#endif


    if (useShadows)
    {
        // 渲染头发阴影map.（通过绑定对应的缓冲区）
        m_hairShadowFramebuffer->bind();
        //告诉opengl渲染窗口的尺寸大小，这样才能才知道根据窗口大小显示数据和坐标
        glViewport(0, 0, m_hairShadowFramebuffer->depthTexture->width(), m_hairShadowFramebuffer->depthTexture->height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓冲和深度缓冲，否则会使用上一次迭代渲染的结果

#if FEEDBACK
        _drawHairFromFeedback(m_TFwhiteHairProgram, model, lightView, lightProjection);
#else
        _drawHair(m_whiteHairProgram, model, lightView, lightProjection);
#endif
        
        // 渲染网格阴影map.（通过绑定对应的缓冲区）
        m_meshShadowFramebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _drawMesh(m_whiteMeshProgram, model, lightView, lightProjection);
        
        // 为不透明度map启用添加混合.
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);//设置混合函数(参数1源，参数2目标因子)GL_ONE因子值为1，实际上相当于完全的使用了源和目标颜色 混合比例都是1
        glBlendEquation(GL_FUNC_ADD);//允许改变方程中源和目标部分的运算符,GL_FUNC_ADD为默认值：相加
        
        // 渲染不透明度map。.（通过绑定对应的缓冲区）
        m_opacityMapFramebuffer->bind();
        glViewport(0, 0, m_hairShadowFramebuffer->depthTexture->width(), m_hairShadowFramebuffer->depthTexture->height());
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if FEEDBACK
        _drawHairFromFeedback(m_TFhairOpacityProgram, model, lightView, lightProjection);
#else
        _drawHair(m_hairOpacityProgram, model, lightView, lightProjection);
#endif
        
        // 恢复以前的状态.
        m_opacityMapFramebuffer->unbind();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
    }
    
    if (useTransparency)
    {
        glViewport(0, 0, m_depthPeel0Framebuffer->colorTexture->width(), m_depthPeel0Framebuffer->colorTexture->height());
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        
        //绘制第一个（最前面）深度剥离层（通过绑定对应的缓冲区）
        m_depthPeel0Framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if FEEDBACK
        _drawHairFromFeedback(m_TFhairProgram, model, m_view, m_projection);
#else
        _drawHair(m_hairProgram, model, m_view, m_projection);
#endif
        _drawMesh(m_meshProgram, model, m_view, m_projection);
        
        // 绘制第二深度剥离层.（通过绑定对应的缓冲区）
        m_depthPeel1Framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if FEEDBACK
        _drawHairFromFeedback(m_TFhairDepthPeelProgram, model, m_view, m_projection);
#else
        _drawHair(m_hairDepthPeelProgram, model, m_view, m_projection);
#endif
        _drawMesh(m_meshDepthPeelProgram, model, m_view, m_projection);
        
        // 渲染到屏幕的最远层.
        m_depthPeel1Framebuffer->unbind();
        glViewport(0, 0, width(), height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        m_depthPeel1Framebuffer->colorTexture->renderFullScreen();
        
        // 在顶部混合更紧密的层.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//参数意义：源颜色×源因子分量 + 目标颜色×（1-源因子分量）
        m_depthPeel0Framebuffer->colorTexture->renderFullScreen();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
    
    else
    {
        if (useSupersampling)
        {
            // 渲染到超采样帧缓冲区。（通过绑定对应的缓冲区）
            m_finalFramebuffer->bind();
            glViewport(0, 0, m_finalFramebuffer->colorTexture->width(), m_finalFramebuffer->colorTexture->height());
        }
        else
        {
            // 渲染到默认帧缓冲区。。。
            glViewport(0, 0, width(), height());
        }
        
        // 渲染场景
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if FEEDBACK
        _drawHairFromFeedback(m_TFhairProgram, model, m_view, m_projection);
#else
        _drawHair(m_hairProgram, model, m_view, m_projection);
#endif
        _drawMesh(m_meshProgram, model, m_view, m_projection);
        
        if (useSupersampling)
        {
            // 渲染超采样纹理。
            m_finalFramebuffer->unbind();
            glViewport(0, 0, width(), height());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_finalFramebuffer->colorTexture->renderFullScreen();
        }
    }
    
    
    // 最后清除缓冲内容（防止影响下次的渲染操作）
    m_noiseTexture->unbind(GL_TEXTURE0);
    m_hairShadowFramebuffer->depthTexture->unbind(GL_TEXTURE1);
    m_opacityMapFramebuffer->colorTexture->unbind(GL_TEXTURE2);
    m_meshShadowFramebuffer->depthTexture->unbind(GL_TEXTURE3);
    m_finalFramebuffer->colorTexture->unbind(GL_TEXTURE4);
    m_hairObject->m_blurredHairGrowthMapTexture->unbind(GL_TEXTURE5);
    m_depthPeel0Framebuffer->depthTexture->bind(GL_TEXTURE6);
    m_depthPeel0Framebuffer->colorTexture->bind(GL_TEXTURE7);
    m_depthPeel1Framebuffer->colorTexture->bind(GL_TEXTURE8);
    
    // 更新 UI.
    m_hairInterface->updateFPSLabel(m_increment);
    if (m_paused || m_pausedLastFrame)
    {
        m_hairInterface->updateFPSLabelPaused(1000.0 / m_clock.elapsed());
    }
    m_pausedLastFrame = isPaused();
}

//设定渲染窗口尺寸，并设置投影矩阵
void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);//调整渲染窗口的大小
/*  perspective创建了一个定义了可视空间的大平截头体，任何在这个平截头体以外的东西最后都不会出现在裁剪空间体积内，并且将会受到裁剪
它的第一个参数定义了fov的值，它表示的是视野(Field of View)，并且设置了观察空间的大小。
第二个参数设置了宽高比，由视口的宽除以高所得。第三和第四个参数设置了平截头体的近和远平面。我们通常设置近距离为0.1f，
而远距离设为100.0f。所有在近平面和远平面内且处于平截头体内的顶点都会被渲染。
*/
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);//投影矩阵,在场景中使用透视投影
    
    m_finalFramebuffer->colorTexture->resize(2*w, 2*h);
    m_finalFramebuffer->resizeDepthBuffer(2*w, 2*h);
    
    forceUpdate();
}

//调整深度PeelFrameBuffers的大小
void GLWidget::_resizeDepthPeelFramebuffers()
{   //是否采用超采样
    int w = useSupersampling ? 2 * width() : width();
    int h = useSupersampling ? 2 * height() : height();
    
    Texture *colorTexture0 = m_depthPeel0Framebuffer->colorTexture;
    if (colorTexture0->width() != w || colorTexture0->height() != h)
    {
        m_depthPeel0Framebuffer->colorTexture->resize(w, h);
        m_depthPeel0Framebuffer->depthTexture->resize(w, h);
        m_depthPeel1Framebuffer->colorTexture->resize(w, h);
        m_depthPeel1Framebuffer->resizeDepthBuffer(w, h);
    }
}
//1.
void GLWidget::_drawHair(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection, bool bindProgram)
{
    if (bindProgram)
    {
        program->bind();//激活着色器程序
    }
    //给着色器加载参数
    program->uniforms.noiseTexture = 0;
    program->uniforms.hairShadowMap = 1;
    program->uniforms.opacityMap = 2;
    program->uniforms.meshShadowMap = 3;
    program->uniforms.depthPeelMap = 6;
    program->uniforms.projection = projection;
    program->uniforms.view = view;
    program->uniforms.model = model;
    program->uniforms.eyeToLight = m_eyeToLight;
    program->uniforms.lightPosition = m_lightPosition;
    program->uniforms.shadowIntensity = m_hairObject->m_shadowIntensity;////渲染模块参数 阴影强度
    program->uniforms.useShadows = useShadows;////渲染模块 阴影复选框
    program->uniforms.specIntensity = m_hairObject->m_specularIntensity;////渲染模块参数
    program->uniforms.diffuseIntensity = m_hairObject->m_diffuseIntensity;////渲染模块参数
    program->uniforms.opacity = 1.f - m_hairObject->m_transparency;////渲染模块参数
    if (m_hairObject->m_useHairColorVariation){////若color Variation复选框有效
        //给着色器的maxColorVariation赋值
        program->uniforms.maxColorVariation = m_hairObject->m_hairColorVariation;////渲染模块参数
    } else {
        program->uniforms.maxColorVariation = 0;
    }
    program->setGlobalUniforms();
    m_hairObject->paint(program);
}

//2.从反馈里绘制头发
void GLWidget::_drawHairFromFeedback(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    program->bind();//启用
    program->uniforms.noiseTexture = 0;
    program->uniforms.hairShadowMap = 1;
    program->uniforms.opacityMap = 2;
    program->uniforms.meshShadowMap = 3;
    program->uniforms.depthPeelMap = 6;
    program->uniforms.projection = projection;
    program->uniforms.view = view;
    program->uniforms.model = model;
    program->uniforms.eyeToLight = m_eyeToLight;
    program->uniforms.lightPosition = m_lightPosition;

    program->uniforms.shadowIntensity = m_hairObject->m_shadowIntensity;////渲染模块
    program->uniforms.useShadows = useShadows;////渲染模块复选框
    program->uniforms.specIntensity = m_hairObject->m_specularIntensity;////渲染模块
    program->uniforms.diffuseIntensity = m_hairObject->m_diffuseIntensity;////渲染模块
    program->uniforms.opacity = 1.f - m_hairObject->m_transparency;////渲染模块
    program->uniforms.hairRadius = m_hairObject->m_hairRadius;
    if (m_hairObject->m_useHairColorVariation){////若color Variation复选框有效
        program->uniforms.maxColorVariation = m_hairObject->m_hairColorVariation;////渲染模块参数
    } else {
        program->uniforms.maxColorVariation = 0;
    }
    program->uniforms.color = glm::rgbColor(
                glm::vec3(m_hairObject->m_color.x*255, m_hairObject->m_color.y, m_hairObject->m_color.z));

    program->setGlobalUniforms();
    program->setPerObjectUniforms();
    program->setPerDrawUniforms();
    m_tessellator->draw();//从镶嵌器里draw
}

//3.
void GLWidget::_drawMesh(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    program->bind();
    program->uniforms.hairShadowMap = 1;
    program->uniforms.opacityMap = 2;
    program->uniforms.meshShadowMap = 3;
    program->uniforms.hairGrowthMap = 5;
    program->uniforms.projection = projection;
    program->uniforms.view = view;
    program->uniforms.model = model;
    program->uniforms.lightPosition = m_lightPosition;
    program->uniforms.eyeToLight = m_eyeToLight;
    program->uniforms.shadowIntensity = m_hairObject->m_shadowIntensity;
    program->uniforms.useShadows = useShadows;
    program->uniforms.color = 2.f * glm::rgbColor(glm::vec3(m_hairObject->m_color.x*255, m_hairObject->m_color.y, m_hairObject->m_color.z)); // multiplying by 2 because it looks better...
    program->setGlobalUniforms();
    program->setPerObjectUniforms();
    m_highResMesh->draw();//从mesh里draw
}


void GLWidget::initSimulation()
{
    safeDelete(m_highResMesh);
    safeDelete(m_lowResMesh);
    HairObject *_oldHairObject = m_hairObject;//将现在的参数  给到上一个（临时做记录）
    //1.加载头部模型
    m_highResMesh = new ObjMesh();
    m_highResMesh->init(":/models/head.obj");//01.obj高分辨率模型（模型更完成平滑），将模型数据加载到vbo中并绑定，同时利用顶点数据初始化三角形
    m_hairInterface->setMesh(m_highResMesh);//将head.obj模型加载到m_mesh对象里

    m_lowResMesh = new ObjMesh();
    m_lowResMesh->init(":/models/headLowRes.obj", 1.1);//低分辨率模型（模型棱角分明），比例缩放因子太小太大都不可以（头发与头的边界影响重合部分）
    //2.仿真模块加载其参数
    Simulation *_oldSim = m_testSimulation;//m_testSimulation用于给仿真模块参数赋值操作（在hairinterface成员函数里调用）
    m_testSimulation = new Simulation(this, m_lowResMesh, _oldSim);//其构造函数：加载仿真模块参数

    //3.用于根据创造的头发（贴图）在网格上生成头发（HairObject类的构造函数）
    if (_oldHairObject == NULL){
        QImage initialGrowthMap(":/images/headHair.jpg");//这幅纹理贴图贴到了渲染窗口的生长图的人的头上
        QImage initialGroomingMap(initialGrowthMap.width(), initialGrowthMap.height(), initialGrowthMap.format());//Direction Map的背景图
        initialGroomingMap.fill(QColor(128, 128, 255));//渲染窗口Direction Map的背景图颜色
        //其构造函数完成对每个三角网格里头发顶点的数据的处理（通过里边new 了hair类）
        m_hairObject = new HairObject(m_highResMesh, m_hairDensity, m_maxHairLength, initialGrowthMap, initialGroomingMap, m_testSimulation, _oldHairObject);//_oldHairObject传进去用于加载几何模块参数
    } else {  //用于重设仿真（里边传的纹理和仿真参数为上一个hairobject）
        m_hairObject = new HairObject(m_highResMesh, m_hairDensity, m_maxHairLength, _oldHairObject->m_hairGrowthMap, _oldHairObject->m_hairGroomingMap, m_testSimulation, _oldHairObject);
    }
    
    safeDelete(_oldSim);//用完即删  （临时指针变量）
    safeDelete(_oldHairObject);//用完即删  （临时指针变量）

    safeDelete(m_tessellator);
    //4.镶嵌器 配置
    m_tessellator = new Tessellator();
    int numTriangles =//这个三角形数量是渲染头发所需的（另一个三角形是mesh模型顶点所得到的三角网格数）
            m_hairObject->m_guideHairs.size()       // # guide hairs总撮（三角网格）数
            * m_hairObject->m_numGroupHairs         // # hairs per guide hair（每一个网格里头发数量）
            * (m_hairObject->m_numSplineVertices-1) // # segments per hair  样条顶点数-1=分割段数
            * 2;                                    // # triangles per segment  每一个分割有两个三角形组成（的长方形）
    m_tessellator->init(numTriangles);//镶嵌所渲染 头发 需要的三角形数量（包含vao、vbo、setAttrib等）

    m_hairInterface->setHairObject(m_hairObject);//更新窗口上控件的文本数据

    
}

void GLWidget::initCamera(){
    
    m_angleX = 0.0;//0脸部  3后脑勺  其他值可看两侧
    m_angleY = .05;//正前方  （大值看头顶）
    m_zoom = 3.5;//仿真模型的视图放大缩小比例3.5较为合适
    
    // 初始化全局视图 矩阵
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *//缩放（向垂直屏幕朝里方向平移m_zoom个单位）
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *//绕x轴
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));//绕y轴
    // 初始化投影矩阵
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    
}
//应用场景编辑（传入　两个窗口的绘制纹理）
void GLWidget::applySceneEditor(Texture *_hairGrowthTexture, Texture *_hairGroomingTexture){
    
    HairObject *_oldHairObject = m_hairObject;

    old_Object = m_hairObject;
    
    //更新m_hairObject  （主要是更新里边的纹理图像为新制作的mask）
    m_hairObject = new HairObject(
                m_highResMesh, m_hairDensity, m_maxHairLength,
                _hairGrowthTexture->m_image, _hairGroomingTexture->m_image,//此时这个纹理图是刚绘制出的新的
                m_testSimulation, _oldHairObject, which_face);//仿真参数用的是之前 _oldhairobject 调整好的

    safeDelete(_oldHairObject);//上边调用（使用）结束 可以删除
    
    m_hairInterface->setHairObject(m_hairObject);//仅用于更新显示控件上的文本数据
    
}
//重设仿真(被hairinterface里的resetSimulation（）所调用，那个是按钮的（重名了）槽函数)
void GLWidget::resetSimulation(bool hardReset)
{   
    
    Simulation *oldSim = NULL;
    HairObject *oldHairObject = NULL;
    if (hardReset){
        oldSim = m_testSimulation;
        oldHairObject = m_hairObject;
        m_testSimulation = NULL;//先赋空，然后再下边的初始化仿真函数里 重新new之后赋值
        m_hairObject = NULL;//先赋空，然后再下边的初始化仿真函数里 重新new之后赋值，else里执行 完成重设仿真
    }

    initSimulation();//初始化仿真(此时为双面生成)
    initCamera();//初始化视角


    delete oldHairObject;
    delete oldSim;
}


// 更新画布。每秒调用60次
void GLWidget::updateCanvas()
{
    update();//更新窗口部件,当需要重新绘制屏幕时,程序调用update()函数
}
//相关鼠标响应　事件（用于控制观察视角）
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        m_prevMousePos = event->pos();
    }
    if (event->button() == Qt::LeftButton)
    {
        m_prevXformPos = event->pos();
        m_testSimulation->m_headMoving = true;
    }
    if (event->button() == Qt::MiddleButton)
    {
        m_prevRotPos = event->pos();
        m_testSimulation->m_headMoving = true;
    }
}
// 通过调整视角矩阵 来进行模型的视角变换 （左键平移 右键旋转  滑轮缩放）
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::RightButton)//可扭转模型（旋转效果）
    {
        m_angleX += 10 * (event->x() - m_prevMousePos.x()) / (float) width();//10为比例因子，其值可用于调节旋转快慢（或灵敏度）
        m_angleY += 10 * (event->y() - m_prevMousePos.y()) / (float) height();
        m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *//沿着z轴向反方向平移m_zoom3.5个单位（向垂直屏幕方向向里，相当于缩小）
                glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *//饶着x轴旋转
                glm::rotate(m_angleX, glm::vec3(0, 1, 0));//饶着y轴旋转
        m_prevMousePos = event->pos();
    }

#if SHIFT_CLICK
    if (event->buttons() == Qt::LeftButton && !(event->modifiers() & Qt::ShiftModifier))
    {
#else
    if (event->buttons() == Qt::LeftButton)//左右两侧视角（平移视角效果）
    {
#endif  //相机空间的 三个方向（up、look、right）

        glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
        glm::mat4 inverseView = glm::inverse(m_view);
        glm::vec3 look = glm::normalize(glm::vec3(inverseView * glm::vec4(0, 0, -1.0, 1)));//摄像机方向向量（指向）？
//        cout << "up: " << glm::to_string(up) << endl;
//        cout << "view: " << glm::to_string(m_view) << endl;
//        cout << "look: " << glm::to_string(look) << endl;
        //右向量(Right Vector)，它代表摄像机空间的x轴的正方向，由up上向量和look方向向量进行叉乘得到
        glm::vec3 right = glm::cross(up, look);
        QPoint delta = event->pos() - m_prevXformPos;//  点delta（△x，△y）表示鼠标移动前后两个点之差
        glm::vec3 xform = glm::vec3();
        xform += (float) delta.x() * 0.005f * right;//在x轴上移动的距离（这里是用作 平移的方向向量）
        xform += (float) -delta.y() * 0.005f * up;//在y轴上移动的距离
        if (look.z < 0.0) xform = -xform;
        m_testSimulation->updatePosition(m_hairObject, xform);//更新Position
        m_prevXformPos = event->pos();
    }

#if SHIFT_CLICK
    if (event->buttons() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier))
    {
#else
    if (event->buttons() == Qt::MiddleButton)//放大缩小效果
    {
#endif

        glm::vec3 p0 = glm::vec3(m_prevRotPos.x() / (float)width(), 1.f - m_prevRotPos.y() / (float)height(), 0.f);
        glm::vec3 p1 = glm::vec3(event->pos().x() / (float)width(), 1.f - event->pos().y() / (float)height(), 0.f);

        p0 = 2.f * p0 - 1.f;
        p1 = 2.f * p1 - 1.f;

        float angle = 5 * glm::length(p1 - p0);
       //Vclip=M_projection⋅M_view⋅M_model⋅V_local得到裁剪坐标,最后的顶点应该被赋值到顶点着色器中的gl_Position，
        //OpenGL将会自动进行透视除法和裁剪,随后OpenGL会使用glViewPort内部的参数来将标准化设备坐标映射到屏幕坐标
        glm::mat4 clipToObject = glm::inverse(m_projection * m_view * m_testSimulation->m_xform);//m_xform就是（更新后：比如平移，旋转后的）modle矩阵
        p0 = glm::vec3(glm::normalize( clipToObject * glm::vec4(p0, 0.f) ));
        p1 = glm::vec3(glm::normalize( clipToObject * glm::vec4(p1, 0.f) ));

        glm::vec3 axis = glm::cross(p0, p1);//通过p0和p1点乘 得到旋转轴

        m_testSimulation->updateRotation(m_hairObject, angle, axis);//后两个参数：旋转的弧度，绕哪个轴线旋转

        m_prevRotPos = event->pos();


        //        QPoint delta = event->pos() - m_prevRotPos;
        //        if (fabs(delta.x()) > fabs(delta.y()))
        //        {
        //            // Rotate in up
        //            glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
        //            float angle = delta.x() * 0.001f;
        //            m_testSimulation->updateRotation(m_hairObject, angle, glm::vec3(0, 1, 0));
        //        }
        //        else
        //        {
        //            // Rotate in right
        ////            glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
        ////            glm::mat4 inverseView = glm::inverse(m_view);
        ////            glm::vec3 look = glm::normalize(glm::vec3(inverseView * glm::vec4(0, 0, 0, 1)));
        ////            glm::vec3 right = glm::cross(up, look);
        //            float angle = delta.y() * 0.001f;
        //            m_testSimulation->updateRotation(m_hairObject, angle, glm::vec3(1,0,0));
        //        }
    }
    
    forceUpdate();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_testSimulation->m_headMoving = false;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_zoom -= event->delta() / 100.f;
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));
    
    forceUpdate();
}
//暂停仿真
void GLWidget::pause()
{
    if (!m_paused)//m_paused一开始为false（表示没有暂停，暂停按钮没有按下，此时可以仿真）
    {
        m_paused = true;//更改状态，使其不能进行仿真，仿真暂停
        m_timer.stop();//关闭定时，此时不会触发paintGl（）则停止绘制
    }
}
//继续仿真
void GLWidget::unpause()
{
    if (m_paused)//表示此时暂停按钮按下了，此时不能进行仿真
    {
        m_paused = false;//更改状态，使其能够仿真
        m_timer.start();//启动定时器，定时到时则触发paintGl（）进行绘制
    }
}

bool GLWidget::isPaused()
{
    return m_paused;
}

//如果此时处于暂停仿真状态 ，除了仿真模块参数，如果其他模块的参数变化，绘制效果也更新
void GLWidget::forceUpdate()
{
    if (isPaused())//如果此时处于暂停仿真状态
    {
        update();//重新绘制场景
    }
}

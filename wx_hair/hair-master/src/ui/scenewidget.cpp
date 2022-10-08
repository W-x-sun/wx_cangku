#include "scenewidget.h"
#include "QWidget"
#include "hairinterface.h"
#include "QMouseEvent"

#include "hairobject.h"
#include "texture.h"
#include "glwidget.h"



SceneWidget::SceneWidget(GLWidget *parent, ObjMesh *mesh) :
    QGLWidget(parent),
    m_timer(this),
    m_mesh(mesh)
{
    mainWidget = parent;
    
    setMouseTracking(true);//鼠标跟踪，用于头顶刷子的绘制
    //刷子参数初始化
    m_grayscale = 1;//灰度程度
    m_radius = 20;//半径
    m_opacity = 1;//透明度
    //淹膜、混色、鼠标按下等初始化
    m_mask = NULL;
    m_blendBuffer = NULL;
    m_blendBufferValid = false;//混合缓冲区有效
    m_mouseDown = false;
    //方向，反向不一样用不同的颜色区分
    m_brushDirColor = glm::vec3(0, 0, 0);
    m_brushDir = "left";
    //纹理
    m_densityMapTexture = NULL;
    m_directionMapTexture = NULL;
    m_currentTexture = NULL;//用于当前显示的
    //衰减（三种）
    m_brushFalloffType = CONSTANT;
    updateBrushSettings();//更新刷子的设置（三种），以创建不同大小的mask用于手动绘制
    //设置定时器更新画布
    m_timer.start(1000.0f / 60.);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateCanvas()));//通过定时器来进行窗口的update，则自动触发paintGL（）进行绘制
}

SceneWidget::~SceneWidget(){
    delete[] m_mask;
    delete[] m_blendBuffer;
}

//更新画布（定时器结束时）
void SceneWidget::updateCanvas()
{
    update();
}
//OpenGL进行的初始化　（纹理）
void SceneWidget::initializeGL()
{    
    
    m_densityMapTexture = new Texture();
    //生成纹理   参数m_hairGrowthMap为图片headhair.png
    m_densityMapTexture->createColorTexture(mainWidget->m_hairObject->m_hairGrowthMap, GL_LINEAR, GL_LINEAR);
    
    m_directionMapTexture = new Texture();
    m_directionMapTexture->createColorTexture(mainWidget->m_hairObject->m_hairGroomingMap, GL_LINEAR, GL_LINEAR);
    
    m_currentTexture = m_densityMapTexture;
}
//设定了渲染窗口了视口大小
void SceneWidget::resizeGL(int width, int height)//参数是：整个绘制窗口的尺寸
{
   // note：这个参数：只是渲染窗口的尺寸，注意和绘制窗口尺寸作区分
     glViewport(0, 0, width, height);//起点为窗口左下角（0,0）
    //glViewport(0, 0, width/2, height/2);//起点为窗口左下角（0,0）    进行测试
}
//主要的绘制逻辑
void SceneWidget::paintGL(){

    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_currentTexture->bind(GL_TEXTURE_2D);//激活纹理单元并把纹理数据发送到着色器
    //1. 把纹理图片绘制为四边形（这个四边形也是鼠标绘制的底板）
    glBegin(GL_QUADS);//（把纹理图像":/images/headHair.jpg"绘制到渲染子窗口）
    glTexCoord2f(0,1); glVertex3f(-1, -1, -1);//   前边的为 纹理坐标 左上角，后边的为四边形顶点坐标  左下角
    glTexCoord2f(1,1); glVertex3f(1, -1, -1);//前边的为 纹理坐标 右上角，后边的为四边形顶点坐标  右下角
    glTexCoord2f(1,0); glVertex3f(1, 1, -1);//前边的为 纹理坐标  右下角  后边的为四边形顶点坐标 右上角
    glTexCoord2f(0,0); glVertex3f(-1, 1, -1);//前边的为 纹理坐标  左下角   后边的为四边形顶点坐标 左上角
    glEnd();

//    glBegin(GL_QUADS);//绘制梯形（修改）   由于不是正四边形，所以鼠标的涂抹位置略微不准
//    glTexCoord2f(0,1); glVertex3f(-1, 0, -1);
//    glTexCoord2f(1,1); glVertex3f(1, 0, -1);
//    glTexCoord2f(1,0); glVertex3f(1, 1, -1);
//    glTexCoord2f(0,0); glVertex3f(-0.57, 1, -1);
//    glEnd();

//    glBegin(GL_QUADS);//绘制多边形（修改）   由于不是正四边形，所以鼠标的涂抹位置略微不准
//     glTexCoord2f(0.25,1); glVertex3f(-0.5, 1, -1);
//     glTexCoord2f(0,0.6); glVertex3f(-1, 0.2, -1);
//    glTexCoord2f(1,1); glVertex3f(1, 0, -1);
//    glTexCoord2f(1,0); glVertex3f(1, 1, -1);
//    glTexCoord2f(0,0); glVertex3f(-1, 0, -1);
//    glEnd();

    m_currentTexture->unbind(GL_TEXTURE_2D);

    // 使能混色
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//参数意义：源颜色×源因子分量 + 目标颜色×（1-源因子分量）
    glColor4f(0, 1, 1, 0.25);//网格的颜色

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 线框

    // 缩放和平移UV贴图（必须在glbegin和end之前设置才能有效）
    glMatrixMode(GL_MODELVIEW);//GL_MODELVIEW,对模型视图矩阵堆栈应用随后的矩阵操作
    glPushMatrix();

    //其网格的原点在左下角
    glTranslatef(-1, -1, 0);//原数据（正数往右平移，正数往上， 0 ）
    glScalef( 2, 2, 0);//原数据（左右缩放，上下缩放）

//    glTranslatef(0, 0.07, 0);// 修改  (头顶网格缩放数据)
//    glScalef(1.44, 1.2, 0);// 修改

    // 2. 绘制网格三角形 UV
    glBegin(GL_TRIANGLES);//GL_TRIANGLES把每个顶点作为一个独立的三角形，顶点3n-2，3n-1和3n定义了第n个三角形，绘制了N/3个三角形。
    for (unsigned int i = 0; i < m_mesh->triangles.size(); i++)//模型顶点 初始化得到的三角形
    {
        Triangle t = m_mesh->triangles[i];
        //每一个顶点对应的uv
        glVertex3f(t.uv1.x,t.uv1.y,-1);//原数据(t.uv1.x, t.uv1.y, -1)
        glVertex3f(t.uv2.x,t.uv2.y,-1);
        glVertex3f(t.uv3.x,t.uv3.y,-1);

        //每个顶点对应的 v（顶点本身）(z,y为侧脸mesh；xy为正脸mesh)
//        if( t.v1.y>0 && t.v2.y>0 && t.v3.y>0){
//            glVertex3f( t.v1.x, t.v1.z,1);// t.v1.y修改值（xy对应正脸网格,z的值若为0则是二维，不为0则是三维，可增加相机视角）
//            glVertex3f( t.v2.x, t.v2.z,1);//t.v2.y
//            glVertex3f( t.v3.x, t.v3.z,1);//t.v3.y
//        }

    }
    glEnd();//glBegin()是和glEnd()结合起来使用。

    glPopMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
    glColor3f(1,1,1);
}


//刷子的三种设置（三种：持续不变、线性、二次型）以及实现
void SceneWidget::updateBrushSettings(){
    if (m_brushFalloffType == CONSTANT){
        makeConstantMask();//对应实现
    } else if (m_brushFalloffType == LINEAR){
        makeLinearMask();//对应实现
    } else if (m_brushFalloffType == QUADRATIC){
        makeQuadraticMask();//对应实现
    }
}

//鼠标事件（用于刷子对发型的绘制，按下＆＆移动）绘制完成更新图像
void SceneWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouseDown = true;

    //鼠标的x 除以 绘制窗口的宽width()表示所占窗口的比例，再乘以 纹理的宽表示窗口上鼠标点的坐标映射到纹理图上的对应坐标位置
    QPoint pos = QPoint(round(event->x()*m_currentTexture->width()/width()), round(event->y()*m_currentTexture->height()/height()));

    //绘制并更新(先根据刷子半径得到mask，在得到pix)     y*2是因为纹理图y的坐标上移了半个窗口
    paintTexture(glm::vec2(pos.x(), pos.y()), m_currentTexture->m_image.bits(), glm::vec2(m_currentTexture->m_image.width(), m_currentTexture->m_image.height()));
    m_currentTexture->updateImage();//更新绘制的图像（绑定纹理，设置纹理参数、把生成的纹理发送到着色器）
}
void SceneWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseDown){
        QPoint pos = QPoint(round(event->x()*m_currentTexture->width()/width()), round(event->y()*m_currentTexture->height()/height()));
        //绘制并更新
        paintTexture(glm::vec2(pos.x(), pos.y()), m_currentTexture->m_image.bits(), glm::vec2(m_currentTexture->m_image.width(), m_currentTexture->m_image.height()));
        m_currentTexture->updateImage();//更新绘制的图像
    }
}
//鼠标释放后　原状态恢复，以等待下次使用
void SceneWidget::mouseReleaseEvent(QMouseEvent *event)//恢复原状态，以等待下次使用
{
    m_mouseDown = false;
    m_blendBufferValid = false;//混色缓冲区是否有效
}

//纹理绘制函数               （参数：鼠标坐标、当前纹理m_currentTexture数据bits，尺寸）         得到pix（像素）
void SceneWidget::paintTexture(glm::vec2 center, uchar *data, glm::vec2 imgSize)
{
    //画布尺寸设置(当前纹理图像的尺寸)
    int canvasW = imgSize.x;
    int canvasH = imgSize.y;
    
    int maskSize = 2*m_radius+1;//mask尺寸
    //依据鼠标坐标 得到mask的边界
    int colStart = center.x-m_radius;//mask左侧起始位置
    int colEnd = center.x+m_radius+1;//mask右侧结束位置
    int rowStart = center.y-m_radius;//mask上边缘起始位置
    int rowEnd = center.y+m_radius+1;//mask下边缘结束位置
    //mask 对应画布上的 四个边缘像素临界值
    int maskColStart = 0;
    int maskColEnd = maskSize;
    int maskRowStart = 0;
    int maskRowEnd = maskSize;
    
    if (colStart < 0){
        maskColStart -= colStart;//限制涂抹左边界
        colStart = 0;
    }
    if (colEnd > canvasW){
        maskColEnd -= (colEnd-canvasW);//限制涂抹右边界
        colEnd = canvasW;
    }
    if (rowStart < 0){
        maskRowStart -= rowStart;//上
        rowStart = 0;
    }
    if (rowEnd > canvasH){
        maskRowEnd -= (rowEnd-canvasH);//下
        rowEnd = canvasH;
    }
    
    BGRA *pix = (BGRA*)data;//（纹理数据）
    
    if (!m_blendBufferValid){
        m_blendBufferValid = true;
        
        delete[] m_blendBuffer;
        m_blendBuffer = new float[canvasW*canvasH];//
        int i; for (i = 0; i < canvasW*canvasH; i++) m_blendBuffer[i] = 0;
    }
    
    int row, col, maskRow, maskCol;
    for (row = rowStart, maskRow = maskRowStart; row < rowEnd; row++, maskRow++){
        for (col = colStart, maskCol = maskColStart; col < colEnd; col++, maskCol++){
            float maskValue = m_mask[maskRow*maskSize+maskCol];//得到当前像素点的mask
            
            float blendValue = m_opacity*maskValue;//透明度
            
            float blendLeftToMax = (m_opacity-m_blendBuffer[row*canvasW+col])/(1.0-m_blendBuffer[row*canvasW+col]);
            if (blendValue > blendLeftToMax) blendValue = blendLeftToMax;
            if (blendValue < 0) blendValue = 0;
            m_blendBuffer[row*canvasW+col] += (1.0-m_blendBuffer[row*canvasW+col])*blendValue; //合并以前的混合
            //依据不同的map图纹理 计算像素pix值（r g b）
            if (m_currentTexture == m_densityMapTexture){
                pix[row*canvasW+col].r = (pix[row*canvasW+col].r)*(1-blendValue) + (255*m_grayscale)*(blendValue);
                pix[row*canvasW+col].g = (pix[row*canvasW+col].g)*(1-blendValue) + (255*m_grayscale)*(blendValue);
                pix[row*canvasW+col].b = (pix[row*canvasW+col].b)*(1-blendValue) + (255*m_grayscale)*(blendValue);
            } else if (m_currentTexture == m_directionMapTexture){
                if (m_brushDir == "left" || m_brushDir == "right"){
                    pix[row*canvasW+col].r = (pix[row*canvasW+col].r)*(1-blendValue) + (255*m_brushDirColor.x)*(blendValue);
                } else if (m_brushDir == "up" || m_brushDir == "down"){
                    pix[row*canvasW+col].g = (pix[row*canvasW+col].g)*(1-blendValue) + (255*m_brushDirColor.y)*(blendValue);
                }
            }
        }
    }
}

//纹理应用到主窗口的模型实现中
void SceneWidget::apply(){
   // m_densityMapTexture->m_image.save("output.png");
//把子窗口的纹理指针  传到主窗口的纹理指针
    mainWidget->resetFromSceneEditorGrowthTexture = m_densityMapTexture;//m_densityMapTexture生成的纹理（可以通过图片来生成）
    mainWidget->resetFromSceneEditorGroomingTexture = m_directionMapTexture;
    
    mainWidget->unpause();
}
//刷子设置（constant）对应实现  （根据刷子半径生成mask，mask较大）
void SceneWidget::makeConstantMask()
{
    delete[] m_mask;
    int len = (2*m_radius+1);
    m_mask = new float[len*len];//根据刷子半径生成mask（2r+1近似创建了一个圆形mask）

    int row, col;
    for (row = 0; row < len; row++){
      for (col = 0; col < len; col++){
        float distance = sqrt((row-m_radius)*(row-m_radius)+(col-m_radius)*(col-m_radius));
        m_mask[row*len+col] = (distance <= m_radius ? 1 : 0);
      }
    }
}
//刷子设置（Linear）对应实现  （根据刷子半径生成mask，mask中等大小）
void SceneWidget::makeLinearMask()
{
    delete[] m_mask;
    int len = (2*m_radius+1);
    m_mask = new float[len*len];
    
    int row, col;
    for (row = 0; row < len; row++){
        for (col = 0; col < len; col++){
            float distance = sqrt((row-m_radius)*(row-m_radius)+(col-m_radius)*(col-m_radius));
            m_mask[row*len+col] = 1-distance/m_radius;
            if (m_mask[row*len+col] < 0) m_mask[row*len+col] = 0;
        }
    }
}
//刷子设置（Quadratic）对应实现   （根据刷子半径生成mask，mask较小）
void SceneWidget::makeQuadraticMask()
{
    delete[] m_mask;
    int len = (2*m_radius+1);
    m_mask = new float[len*len];

    int row, col;
    for (row = 0; row < len; row++){
      for (col = 0; col < len; col++){
        float distance = sqrt((row-m_radius)*(row-m_radius)+(col-m_radius)*(col-m_radius));
        m_mask[row*len+col] = (1-distance/m_radius);
        if (m_mask[row*len+col] < 0) m_mask[row*len+col] = 0; // because apparently using max(a, b) is a bitch
        else m_mask[row*len+col] *= (1-distance/m_radius);
      }
    }
}

//清空子窗口里已经绘制过得纹理，并更新图像
void SceneWidget::clearTexture(int r, int g, int b, Texture *texture){
    if (texture == NULL) texture = m_currentTexture;
//    cout << r << ", " << g << ", " << b << endl;
    texture->m_image.fill(QColor(r, g, b));
//    QRgb test = texture->m_image.pixel(0, 0);
//    cout << qRed(test) << ", " <<qGreen(test) << ", " << qBlue(test)<< endl;
    texture->updateImage();//更新
}
//设置刷子的不同方向的对应颜色（上下左右）
void SceneWidget::setBrushDir(QString dir){
    m_brushDir = dir;
    if (dir == "left"){
        m_brushDirColor = glm::vec3(0, 0.5, 0.5); // change r
    } else if (dir == "right"){
        m_brushDirColor = glm::vec3(1, 0.5, 0.5); // change r
    } else if (dir == "up"){
        m_brushDirColor = glm::vec3(0.5, 1, 0.5); // change g
    } else if (dir == "down"){
        m_brushDirColor = glm::vec3(0.5, 0, 0.5); // change g
    }
}

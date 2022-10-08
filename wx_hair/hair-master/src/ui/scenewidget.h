#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include "hairCommon.h"

#include <QTimer>
#include <QGLWidget>

class Texture;
class GLWidget;
class ObjMesh;


enum BrushFalloff { CONSTANT, LINEAR, QUADRATIC };

class SceneWidget : public QGLWidget
{
    Q_OBJECT
    friend class SceneEditor;
    friend class HairInterface;
    
public:
    explicit SceneWidget(GLWidget *parent, ObjMesh *mesh);
    ~SceneWidget();
    
    GLWidget *mainWidget;
    
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    
    void updateBrushSettings();
    
    void paintTexture(glm::vec2 center, uchar *pix, glm::vec2 imgSize);    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    
    void apply();
    
    void makeConstantMask();
    void makeLinearMask();
    void makeQuadraticMask();
    
    void setBrushDir(QString dir);    
    void clearTexture(int r, int g, int b, Texture *texture = NULL);


    
signals:
    
public slots:
    void updateCanvas();
    
protected:
    QTimer m_timer;
//三个纹理指针，前两个用来对第三个m_currtexture进行赋值
    Texture *m_densityMapTexture;
    Texture *m_directionMapTexture;
    Texture *m_currentTexture;

    ObjMesh *m_mesh;
    
    double m_grayscale;//灰度
    double m_opacity;//不透明度
    int m_radius;//半径
    
    QString m_brushDir;//刷子方向（left、right、up、down）
    glm::vec3 m_brushDirColor;//对应方向的刷子的颜色
    
    float *m_mask;
    float *m_blendBuffer;
    QImage m_previewBuffer;
    bool m_blendBufferValid;//混色缓冲区有效
    
    BrushFalloff m_brushFalloffType;//刷子的衰减方式
    
    
    bool m_mouseDown;
    

    
    
};

#endif // SCENEWIDGET_H

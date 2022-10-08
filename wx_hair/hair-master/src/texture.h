#ifndef TEXTURE_H
#define TEXTURE_H

#include "hairCommon.h"

class Quad;
class TexturedQuadShaderProgram;
class QImage;

class Texture
{
public:
    Texture();

    virtual ~Texture();

    // 创建包含给定图像的纹理.
    void createColorTexture(const char *imageFile, GLint magFilter, GLint minFilter);
    void createColorTexture(QImage &m_image, GLint magFilter, GLint minFilter);

    // 创建具有给定宽度和高度的黑色纹理.
    void createColorTexture(int width, int height, GLint magFilter, GLint minFilter);

    void createDepthTexture(int width, int height, GLint magFilter, GLint minFilter);

    // 更新存储的纹理以反映对m_image的更改
    void updateImage();
    
    // 调整纹理大小并将其设置为黑色.
    void resize(int width, int height);

    void bind(GLenum textureUnit);

    void unbind(GLenum textureUnit);

    // 渲染纹理的全屏四边形.
    void renderFullScreen();

    int width();
    int height();

    GLuint id;
    
    QImage m_image;//创建一个图像，SceneWidget类里当要清除绘制的纹理时通过fill（r=0,g=0,b=0）调用进行清除

private:
    void _create(GLvoid *data,
            GLint internalFormat,
            int width,
            int height,
            GLenum format,
            GLenum type,
            GLint magFilter,
            GLint minFilter);

    // glTexImage2D的参数
    int m_width, m_height;
    GLint m_internalFormat;
    GLenum m_format;
    GLenum m_type;
    GLint m_magFilter;
    GLint m_minFilter;

    // 用于全屏渲染
    Quad *m_quad;
    TexturedQuadShaderProgram *m_program;
};

#endif // TEXTURE_H

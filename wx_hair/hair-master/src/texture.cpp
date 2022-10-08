#include "texture.h"

#include "texturedquadshaderprogram.h"
#include "quad.h"
#include <QImage>

Texture::Texture()
{
    m_quad = NULL;
    m_program = NULL;
}

Texture::~Texture()
{
    safeDelete(m_quad);
    safeDelete(m_program);
}
//创建包含给定图像的纹理（参数：传入图像、纹理过滤方式、纹理过滤方式）
void Texture::createColorTexture(const char *imageFile, GLint magFilter, GLint minFilter)
{
    m_image = QImage(imageFile);
    //创建纹理并绑定，同时生成纹理，设置纹理参数
    _create(m_image.bits(), GL_RGBA, m_image.width(), m_image.height(), GL_RGBA,
            GL_UNSIGNED_BYTE, magFilter, minFilter);
}

void Texture::createColorTexture(QImage &image, GLint magFilter, GLint minFilter)
{
    m_image = image;
    //创建、生成纹理 ，且设置纹理过滤参数
    _create(m_image.bits(), GL_RGBA, m_image.width(), m_image.height(), GL_RGBA,
            GL_UNSIGNED_BYTE, magFilter, minFilter);
}
//创建具有给定宽度和高度的黑色纹理
void Texture::createColorTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    _create(0, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, magFilter, minFilter);
}
//创建具有给定宽度和高度的depth纹理
void Texture::createDepthTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    _create(0, GL_DEPTH_COMPONENT16, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, magFilter, minFilter);
    bind(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    unbind(GL_TEXTURE0);
}

void Texture::_create(
        GLvoid *data, GLint internalFormat, int width, int height, GLenum format,
        GLenum type, GLint magFilter, GLint minFilter)
{
    m_internalFormat = internalFormat;
    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;
    m_minFilter = magFilter;
    m_magFilter = minFilter;
//创建纹理并绑定
    glGenTextures(1, &id);
    bind(GL_TEXTURE0);
    //生成纹理并设置纹理参数
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);//magFilter放大时对像素颜色采用的过滤方式（邻近或线性过滤）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    unbind(GL_TEXTURE0);
}
//更新存储的纹理以反映对m_image的更改（重新生成纹理并设置纹理参数）
void Texture::updateImage(){
    bind(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, m_image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
    unbind(GL_TEXTURE0);
}
//调整纹理大小并将其设置为黑色
void Texture::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    bind(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_format, m_type, 0);
    unbind(GL_TEXTURE0);
}

void Texture::bind(GLenum textureUnit)
{
    glActiveTexture(textureUnit);//激活对应纹理单元
    glBindTexture(GL_TEXTURE_2D, id);
    glActiveTexture(GL_TEXTURE0);
}

void Texture::unbind(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}
//渲染纹理的全屏四边形
void Texture::renderFullScreen()
{
    if (!m_quad)
    {
        m_quad = new Quad();
        m_quad->init();
    }
    if (!m_program)
    {
        m_program = new TexturedQuadShaderProgram();
        m_program->create();
    }
    m_program->bind();
    bind(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(m_program->id, "tex"), 0);
    m_quad->draw();
    unbind(GL_TEXTURE0);
    m_program->unbind();  
    
}

int Texture::width()
{
    return m_width;
}

int Texture::height()
{
    return m_height;
}

#include "framebuffer.h"

#include "texture.h"

Framebuffer::~Framebuffer()
{
    safeDelete(colorTexture);
    safeDelete(depthTexture);
}

void Framebuffer::create()
{
    glGenFramebuffers(1, &m_id);//参数1：要生成的帧缓冲区对象名称的数量
    //参数2：生成的帧缓冲对象的句柄
}

void Framebuffer::bind(GLenum target)//参数为要绑定的类型（读取或写入 或两者都有）
{
    glBindFramebuffer(target, m_id);
}

void Framebuffer::unbind(GLenum target)
{
    glBindFramebuffer(target, 0);
}
//附加颜色纹理（单个）
void Framebuffer::attachColorTexture(GLuint textureID, GLenum target)
{
    std::vector<GLuint> textureIDs = {textureID};
    attachColorTextures(textureIDs, target);
}
//附加颜色纹理（多个）
void Framebuffer::attachColorTextures(std::vector<GLuint> textureIDs, GLenum target)
{
    bind(target);//1.绑定到对应的缓冲区类型

    // 2.将对应纹理 顺序附加 到相应的缓冲类型target里
    int numTextures = textureIDs.size();
    GLenum drawBuffers[numTextures];
    for (int i = 0; i < numTextures; i++)
    {
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        glFramebufferTexture(target, GL_COLOR_ATTACHMENT0 + i, textureIDs[i], 0);//将纹理附加到缓冲区上
        //参数（附加的缓冲区类型  、附加的附件类型（颜色附件）、  附加的纹理本身 、多级渐远级别这保留为0）
    }
    glDrawBuffers(numTextures, drawBuffers);

    // 3.检查 framebuffer 状态是否完整
    GLenum framebufferStatus = glCheckFramebufferStatus(target);
    if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete. Status: " << framebufferStatus << std::endl;

    unbind(target);
}
//附加depth纹理
void Framebuffer::attachDepthTexture(GLuint textureID, GLenum target)
{
    bind(target);
    glFramebufferTexture(target, GL_DEPTH_ATTACHMENT, textureID, 0);
    unbind(target);
}
//生成color纹理
void Framebuffer::generateColorTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    safeDelete(colorTexture);
    colorTexture = new Texture();
    colorTexture->createColorTexture(width, height, magFilter, minFilter);
    attachColorTexture(colorTexture->id);
}
//生成depth纹理
void Framebuffer::generateDepthTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    safeDelete(depthTexture);
    depthTexture = new Texture();
    depthTexture->createDepthTexture(width, height, magFilter, minFilter);//创建具有给定宽度和高度的depth纹理
    attachDepthTexture(depthTexture->id);//附加depth纹理
}
//生成渲染缓冲对象附件，并附加到缓冲区，用于depth缓存
void Framebuffer::generateDepthBuffer(int width, int height)
{
    bind();
    glGenRenderbuffers(1, &m_depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);
    unbind();
}
//调整depth缓存
void Framebuffer::resizeDepthBuffer(int width, int height)
{
    bind();
    glDeleteRenderbuffers(1, &m_depthBufferID);
    generateDepthBuffer(width, height);
    unbind();
}

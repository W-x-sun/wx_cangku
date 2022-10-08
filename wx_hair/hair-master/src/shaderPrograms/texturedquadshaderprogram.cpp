#include "texturedquadshaderprogram.h"

#include "resourceloader.h"
//纹理着色器
GLuint TexturedQuadShaderProgram::createShaderProgram()
{
    return ResourceLoader::createBasicShaderProgram(
                ":/shaders/texturedquad.vert", ":/shaders/texturedquad.frag");
}

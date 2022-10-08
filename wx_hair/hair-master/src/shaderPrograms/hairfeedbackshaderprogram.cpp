#include "hairfeedbackshaderprogram.h"

#include "resourceloader.h"
//头发反馈着色器程序
GLuint HairFeedbackShaderProgram::createShaderProgram()
{
    const GLchar* varyings[] = {"position_g", "tangent_g", "colorVariation_g", "tessx_g"};
    return ResourceLoader::createFullFeedbackShaderProgram(
                ":/shaders/hair.vert",
                ":/shaders/hairFeedback.geom",
                ":/shaders/hair.tcs",
                ":/shaders/hairFeedback.tes",
                varyings, 4);
}

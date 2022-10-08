#ifndef SHADER_H
#define SHADER_H

#include "GL/glew.h"
#include "hairCommon.h"
#include <vector>

class ResourceLoader
{
public:
    ResourceLoader();
    static GLuint createBasicShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path);

    static GLuint createGeomShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path,
            const char * geom_file_path);

    static GLuint createTessShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path);

    static GLuint createFullShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path,
            const char * geom_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path);

    static GLuint createTessFeedbackShaderProgram(
            const char * vertex_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path,
            const char ** varyings, int numVaryings);

    static GLuint createFullFeedbackShaderProgram(
            const char * vertex_file_path,
            const char * geom_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path,
            const char ** varyings, int numVaryings);

    static void initializeGlew();

private:
    static std::string _readShaderFile(std::string filepath, int &additionalLines);//读取着色器文件
    static GLuint _createShader(GLenum shaderType, const char *filepath);//创建单个着色器并编译 返回值为创建的着色器  （参数： 1，着色器类型   2，着色器源代码 ）
    static GLuint _createFeedbackProgramFromShaders(std::vector<GLuint> &shaders, const GLchar **varyings, int numVaryings);
    static GLuint _createProgramFromShaders(std::vector<GLuint> &shaders);//创建着色器程序并完成链接

    static void _attachShaders(GLuint program, std::vector<GLuint> &shaders);//给着色器程序里添加单个着色器（参数1.着色器程序     参数2.着色器容器）
    static void _linkProgram(GLuint program);//链接着色器程序
    static void _deleteShaders(std::vector<GLuint> &shaders);//链接完成后delete 单个着色器
};

#endif // SHADER_H

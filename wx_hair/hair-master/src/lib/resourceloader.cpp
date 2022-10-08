#include "resourceloader.h"
#include "errorchecker.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "QStringList"

ResourceLoader::ResourceLoader()
{
}
//着色器（参数：加载相关文件路径　　　）
GLuint ResourceLoader::createFullFeedbackShaderProgram(
        const char *vertexFilePath,
        const char *geomFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath,
        const char **varyings,
        int numVaryings)
{
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),//创建顶点着色器并完成编译
        _createShader(GL_GEOMETRY_SHADER, geomFilePath),//创建几何着色器并完成编译
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),//创建曲面细分着色器（镶嵌器）
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    return _createFeedbackProgramFromShaders(shaders, varyings, numVaryings);
}

GLuint ResourceLoader::createTessFeedbackShaderProgram(
        const char *vertexFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath,
        const char **varyings,
        int numVaryings)
{
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    return _createFeedbackProgramFromShaders(shaders, varyings, numVaryings);
}
//创建完整的着色的程序（参数：表示传入的着色器源代码的字符指针）
GLuint ResourceLoader::createFullShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *geomFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath)
{
    // 创建每个所需的着色器并完成编译
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        _createShader(GL_GEOMETRY_SHADER, geomFilePath),
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    //创建着色器程序并完成链接（返回值为着色器程序对应句柄，参数为着色器容器，里边包含多个着色器）
    return _createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createGeomShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *geomFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        _createShader(GL_GEOMETRY_SHADER, geomFilePath)
    };
    return _createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createTessShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    return _createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createBasicShaderProgram(const char *vertexFilePath,const char *fragmentFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
    };
    return _createProgramFromShaders(shaders);
}
//给着色器程序里添加单个着色器（参数1.着色器程序     参数2.着色器容器）
void ResourceLoader::_attachShaders(GLuint program, std::vector<GLuint> &shaders)
{
    for (std::vector<GLuint>::iterator iter = shaders.begin(); iter != shaders.end(); iter++) {
        GLuint shaderID = (*iter);
        glAttachShader(program, shaderID);
    }
}
//链接着色器程序
void ResourceLoader::_linkProgram(GLuint program)
{
    printf("Linking shaders... \n");

    // 链接
    glLinkProgram(program);

    // 打印信息日志。
    GLint result;
    GLint infoLogLength;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog( std::max(infoLogLength, int(1)) );
        glGetProgramInfoLog(program, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);

        // 如果着色器程序未链接，请退出。
        if (result == GL_FALSE) exit(1);
    }
}
//链接完成后delete 单个着色器
void ResourceLoader::_deleteShaders(std::vector<GLuint> &shaders)
{
    for (std::vector<GLuint>::iterator iter = shaders.begin(); iter != shaders.end(); iter++) {
        GLuint shaderID = (*iter);
        glDeleteShader(shaderID);
    }
}
//创建着色器程序并完成链接（返回值为着色器程序对应句柄，参数为着色器容器，里边包含多个着色器）
GLuint ResourceLoader::_createProgramFromShaders(std::vector<GLuint> &shaders)
{
    GLuint programId = glCreateProgram();//创建着色器程序
    _attachShaders(programId, shaders);//将传进来的着色器添加到着色器程序里
    _linkProgram(programId);//链接着色器程序
    _deleteShaders(shaders);//链接完成后delete
    return programId;
}

GLuint ResourceLoader::_createFeedbackProgramFromShaders(
        std::vector<GLuint> &shaders, const GLchar **varyings, int numVaryings)
{
    GLuint programId = glCreateProgram();
    _attachShaders(programId, shaders);
    glTransformFeedbackVaryings(programId, numVaryings, varyings, GL_INTERLEAVED_ATTRIBS);
    _linkProgram(programId);
    _deleteShaders(shaders);
    return programId;
}
//读取着色器文件
std::string ResourceLoader::_readShaderFile(std::string filepath, int &additionalLines)
{
    additionalLines = 0;
    std::string text;
    QFile file(QString::fromStdString(filepath));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);

        while(!stream.atEnd())
        {
            QString line = stream.readLine();

            if (line.startsWith("#include "))
            {
                QString includeFile = line.split(" ").at(1);
                includeFile = includeFile.remove( QRegExp("^[\"]*") ).remove( QRegExp("[\"]*$") );
                includeFile = ":/shaders/" + includeFile;
                int throwaway;
                line = QString::fromStdString(_readShaderFile(includeFile.toStdString(), throwaway));
                additionalLines += line.split("\n").size() - 1;
            }

            text += line.toStdString() + "\n";
        }
    } else {
        std::cout << "Could not open file: " << filepath << std::endl;
        exit(1);
    }
    return text;
}
//创建单个着色器并编译 返回值为创建的着色器对象句柄  （参数： 1，着色器类型   2，着色器源代码 ）
GLuint ResourceLoader::_createShader(GLenum shaderType, const char *filepath)
{
    GLuint shaderID = glCreateShader(shaderType);//创建成功返回一个正整数，代表着色器对象句柄

    // 编译着色器源代码
    printf("Compiling shader: %s\n", filepath);
    int additionalLines;
    std::string code = _readShaderFile(filepath, additionalLines);
    const char *codePtr = code.c_str();
    glShaderSource(shaderID, 1, &codePtr, NULL);//要编译的着色器对象作为第一个参数。第二参数指定了传递的源码字符串数量，这里只有一个 第三个参数是片段着色器真正的源码，
    glCompileShader(shaderID);

    // 打印日志
    GLint result;
    GLint infoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog(infoLogLength);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);

        // Exit if shader not compiled.
        if (result == GL_FALSE)
        {
            fprintf(stdout, "%d additional lines from included files\n", additionalLines);
            exit(1);
        }
    }

    return shaderID;
}
//初始化ｇｌew
void ResourceLoader::initializeGlew()
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // 检查OpenGL错误，但忽略GL_INVALID_ENUM，这是由glewExperimental引起的。
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        if (err == GL_INVALID_ENUM) continue;
        printf("OpenGL error in ResourceLoader::initializeGlew(): %d\n", err);
    }}

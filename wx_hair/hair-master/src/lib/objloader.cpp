#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <QFile>
#include <QTextStream>

#include <glm/glm.hpp>

#include "objloader.hpp"
//加载obj模型
bool OBJLoader::loadOBJ(
    const char * path, //加载的文件路径
        //（输出）得到的模型数据
    std::vector<glm::vec3> & out_vertices, //顶点坐标
    std::vector<glm::vec2> & out_uvs,//纹理坐标
    std::vector<glm::vec3> & out_normals//顶点法向量
){
	printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;//临时存储量
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

    // 读取目标文件
    QString filepathStr = QString(path);
    QFile file(filepathStr);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
         //读取文件里的数据到临时存储量里
        while (!stream.atEnd()) {
            QString line = stream.readLine();//遇到\n \r 或者是\r\n的时候 此方法返回这前面的字符串，然后内部的指针往后移一位下次从新的地方开始读，直到遇到数据的结尾处返回null，
            QStringList parts = line.split(" ", QString::SkipEmptyParts);//以空格作为分隔符进行字符串的分割，返回值为list 结果里不保留空格
            if (line.startsWith("v ")) {//以v开头  -> 处理vertex
                glm::vec3 vertex = glm::vec3(
                            parts.at(1).toFloat(),
                            parts.at(2).toFloat(),
                            parts.at(3).toFloat());
                temp_vertices.push_back(vertex);
            }

            else if (line.startsWith("vt ")) {//vt -> 处理uv
                glm::vec2 uv = glm::vec2(parts.at(1).toFloat(), parts.at(2).toFloat());
                temp_uvs.push_back(uv);
            }

            else if (line.startsWith("vn ")) {//vn -> 处理normal
                glm::vec3 normal = glm::vec3(
                            parts.at(1).toFloat(),
                            parts.at(2).toFloat(),
                            parts.at(3).toFloat());
                temp_normals.push_back(normal);
            }

            else if (line.startsWith("f ")) {  //f开头 ->（两步处理）
                for (int i = 1; i <= 3; i++) {
                    //1.对part每个元素以/作为分隔符进行分割  并存储到对应indices里
                    QStringList v = parts.at(i).split("/");
                    vertexIndices.push_back(v.at(0).toInt());
                    uvIndices.push_back(v.at(1).toInt());
                    normalIndices.push_back(v.at(2).toInt());
                }
                // 2.如果面是四边形，则制作另一个三角形
                if (parts.size() > 4) {
                    std::vector<int> indices = {3, 4, 1};
                    for (unsigned int i = 0; i < indices.size(); i++) {
                        QStringList v = parts.at(indices[i]).split("/");
                        vertexIndices.push_back(v.at(0).toInt());
                        uvIndices.push_back(v.at(1).toInt());
                        normalIndices.push_back(v.at(2).toInt());
                    }
                }
            }
        }

        // 对每个三角形的每个顶点
        for (unsigned int i = 0; i < vertexIndices.size(); i++) {

            // 获取其属性的索引
            unsigned int vertexIndex = vertexIndices[i];
            unsigned int uvIndex = uvIndices[i];
            unsigned int normalIndex = normalIndices[i];

            // 通过索引获取属性
            glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
            glm::vec2 uv = temp_uvs[ uvIndex-1 ];
            glm::vec3 normal = temp_normals[ normalIndex-1 ];

            // 将属性放入缓冲区
            out_vertices.push_back(vertex);
            out_uvs     .push_back(uv);
            out_normals .push_back(normal);
        }
    }

    return true;
}

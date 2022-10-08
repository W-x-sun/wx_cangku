#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;//值为0-1，水平方向的第U个像素/图片宽度，垂直方向的第V个像素/图片高度
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;//转换视角矩阵
uniform mat4 projection;

//以下输出可用于mesh.frag网格着色器的输入
out vec4 position_v;
out vec4 normal_v;
out vec2 uv_v;

void main() {
    position_v = model * vec4(position, 1.0);//世界坐标
    normal_v = model * vec4(normal, 0.0);//世界坐标中的法向量
    uv_v = uv;
    gl_Position = projection * view * position_v;//该顶点着色器处理后的输出坐标（变换后的裁剪坐标）
}

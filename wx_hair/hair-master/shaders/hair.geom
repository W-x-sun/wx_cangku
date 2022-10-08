#version 400 core

layout(lines) in;//这个输入布局修饰符可以从顶点着色器接收图元类型为lines （提供给glDrawArrays渲染）几何着色器的输入是一个图元的所有顶点
layout(triangle_strip, max_vertices=4) out;//triangle输出图元修饰符，用来输出创建的任意形状图元，几何着色器设置一个图元最大能够输出的顶点数量

in vec3 tangent_te[]; // Per-vertex, eye-space tangent vector.
in float tessx_te[];
in float colorVariation_te[];

out vec4 position_g;
out vec3 tangent_g;
out float colorVariation_g;
out float tessx_g;

//全局数据， 应用程序中也能读取和写入的变量，用于着色器和应用程序之间互通数据
uniform mat4 projection;
uniform float hairRadius;
uniform float taperExponent;//（头发）变尖指数   控制头发在末端开始逐渐变细的距离

void main() {
    for(int i = 0; i < gl_in.length(); i++)    //以某种方式来获取前一着色器阶段的输出，opengl提供了内建变量gl_in[]，里边应该包含的是 一个 输入图元的所有顶点
    {
        // Eye-space 顶点位置
        vec4 position = gl_in[i].gl_Position;//输入的这一个图元包含的第i个顶点的坐标

// 通过 tangent and eye vectors 以获取 billboarding 的偏移方向
        vec3 offsetDir = cross(normalize(tangent_te[i]), normalize(position.xyz));
        vec4 offset = hairRadius * vec4(offsetDir, 0.0);

// 逐渐变细头发，使其末端更细。
        offset *= (1 - pow(tessx_te[i], taperExponent));
        tangent_g = tangent_te[i];
        colorVariation_g = colorVariation_te[i];
        tessx_g = tessx_te[i];
//更新坐标（附加偏移量效果）并发送
        position_g = (position + offset);
        gl_Position = projection * position_g;
        EmitVertex();
        //每次我们调用EmitVertex时，gl_Position中的向量会被添加到图元中来。当EndPrimitive被调用时，所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元
        position_g = (position - offset);
        gl_Position = projection * position_g;
        EmitVertex();//
    }
}

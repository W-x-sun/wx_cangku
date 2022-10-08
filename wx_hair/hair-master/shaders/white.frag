#version 400 core
//这里用作光源（即使修改其他着色器也不影响光源的颜色，白色）
out vec3 fragColor;

void main(){
    fragColor = vec3(1.0);//三个分量都是1.0表示白色
}

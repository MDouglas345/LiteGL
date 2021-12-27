#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 VertColor;

void main(){
    gl_Position = vec4(aPos, 1.0);
    VertColor = vec4(0.2,0.22,0.333, 1.0);
}
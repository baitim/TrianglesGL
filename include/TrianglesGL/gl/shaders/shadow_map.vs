#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 depth_MVP;

void main() {
    gl_Position =  depth_MVP * vec4(position, 1);
}
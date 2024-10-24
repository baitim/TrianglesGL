#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 MVP;
uniform mat4 depth_bias_MVP;

out vec3 v_color;
out vec4 shadow_coord;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    v_color = color;
    shadow_coord = depth_bias_MVP * vec4(position, 1);
}
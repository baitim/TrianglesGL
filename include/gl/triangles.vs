#version 330 core

const int NORMALS_SIZE = 2000;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform vec3 user_dir;
uniform vec3 light_dir;
uniform vec3 normals[NORMALS_SIZE];
uniform mat4 MVP;
uniform mat4 depth_bias_MVP;

out vec3  v_color;
out vec4  shadow_coord_;
out float is_dark_side;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    v_color = color;
    shadow_coord_ = depth_bias_MVP * vec4(position, 1.0);

    vec3 normal = normals[gl_VertexID / 3];
    if(dot(user_dir, normal) * dot(light_dir, normal) < 0)
        is_dark_side = 1.0;
    else
        is_dark_side = 0.0;
}
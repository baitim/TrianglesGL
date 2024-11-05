#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in int  color;
layout(location = 3) in int  is_dark;

uniform vec3 user_dir;
uniform vec3 light_dir;
uniform vec3 colors[2]; 
uniform mat4 MVP;
uniform mat4 depth_bias_MVP;

out vec3  v_color;
out vec4  shadow_coord_;
out float is_dark_side;
out float light_angle;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    v_color = colors[color];
    shadow_coord_ = depth_bias_MVP * vec4(position, 1.0);

    light_angle = dot(light_dir, normal);
    is_dark_side = is_dark;
}
#version 330 core

in vec3 v_color;

uniform float time;

void main(){
    gl_FragColor = vec4(v_color.x + time, v_color.y + time, v_color.z + time, 1.0);
}

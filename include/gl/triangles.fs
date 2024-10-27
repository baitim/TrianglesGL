#version 330 core

in vec3  v_color;
in vec4  shadow_coord_;
in float is_dark_side;

uniform float time;
uniform sampler2D shadow_map;

void main() {
    vec3 shadow_coord = shadow_coord_.xyz / shadow_coord_.w;
    float visability = 1.0f;
    float bias = 0.05f;
    if (shadow_coord.x > 1.0 || shadow_coord.x < 0.0 ||
        shadow_coord.y > 1.0 || shadow_coord.y < 0.0 ||
        shadow_coord.z > 1.0 || shadow_coord.z < 0.0)
        visability = 0.1;
    else if((texture(shadow_map, shadow_coord.xy).r < shadow_coord.z - bias) ||
            (is_dark_side > 0.5))
        visability = 0.4;

    gl_FragColor = vec4((v_color.x + time) * visability,
                        (v_color.y + time) * visability,
                        (v_color.z + time) * visability, 
                        1.0);
}
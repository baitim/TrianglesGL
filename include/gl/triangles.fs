#version 330 core

in vec3  v_color;
in vec4  shadow_coord_;
in float is_dark_side;
in float light_angle;

uniform float time;
uniform sampler2D shadow_map;

void main() {
    vec3 shadow_coord = shadow_coord_.xyz / shadow_coord_.w;
    float visability  = 1.0;
    float visability_from_another = 0.3;
    float bias = 0.05f;
    if((texture(shadow_map, shadow_coord.xy).r < shadow_coord.z - bias) || (is_dark_side > 0.5))
        visability = visability_from_another;
    else
        visability = abs(light_angle) * (visability - visability_from_another) / visability + visability_from_another;

    gl_FragColor = vec4((v_color.x + time) * visability,
                        (v_color.y + time) * visability,
                        (v_color.z + time) * visability, 
                        1.0);
}
#include <iostream>

#include "shaders.hpp"
#include "window.hpp"

int main() {
    window::window_t window(sf::VideoMode(800, 600), "Triangle");

    const int count_vertexes = 9;
    const GLfloat vertex_buffer_data[count_vertexes] = {-1.0f, -1.0f, 0.0f,
                                                         1.0f, -1.0f, 0.0f,
                                                         0.0f,  1.0f, 0.0f,};

    shaders::gl_shaders_program_t gl_program("include/gl/vertexshader.vert",
                                             "include/gl/fragmentshader.frag",
                                             count_vertexes, vertex_buffer_data);

    window.main_cycle();

    return 0;
}
#include <iostream>

#include "shaders.hpp"
#include "window.hpp"

static const GLfloat vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

int main() {
    window::window_t window(sf::VideoMode(800, 600), "Triangle");

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Невозможно инициализировать GLEW\n";
        return - 1;
    }

    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    shaders::gl_shaders_program_t gl_program("include/gl/vertexshader.vert",
                                             "include/gl/fragmentshader.frag");

    glUseProgram(gl_program.get_program_id());
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    window.main_cycle();

    return 0;
}
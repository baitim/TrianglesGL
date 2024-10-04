#include <iostream>

#include "window.hpp"

int main() {
    window::window_t window(sf::VideoMode(800, 600), "Triangle");

    const int size_vertices = 18;
    const GLfloat vertices[size_vertices] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                              1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                              0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    shaders::gl_shaders_program_t gl_program("include/gl/vertexshader.vert",
                                             "include/gl/fragmentshader.frag",
                                             size_vertices, vertices);

    user::user_t user(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.3f, 0.3f, 0.3f), 0.2 * M_PI, 1.2 * M_PI);

    window.main_cycle(gl_program, user);

    return 0;
}
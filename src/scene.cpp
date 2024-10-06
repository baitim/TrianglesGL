#include <iostream>
#include "window.hpp"
#include "scene.hpp"

int main() {
    scene::scene_t<float> scene;
    std::cin >> scene;

    window::window_t window(sf::VideoMode(800, 600), "Triangle", sf::ContextSettings(24, 8, 0, 3, 3));

    vertices::vertices_t vertices = scene.get_vertices();
    shaders::gl_shaders_program_t gl_program("include/gl/vertexshader.vert",
                                             "include/gl/fragmentshader.frag",
                                             vertices.count_, vertices.vertices_);

    user::user_t user(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.03f, 0.03f, 0.03f), 0, 0, 0.1f, 20.0f);

    window.main_cycle(gl_program, user);

    return 0;
}
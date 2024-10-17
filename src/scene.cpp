#include <iostream>
#include "window.hpp"
#include "scene.hpp"

int main() {
    scene::scene_t<float> scene;
    std::cin >> scene;

    window::window_t window(sf::VideoMode(800, 600), "Triangles", sf::ContextSettings(24, 8, 0, 3, 3));

    vertices::vertices_t vertices = scene.get_vertices();
    shaders::gl_shaders_program_t gl_program("include/gl/triangles.vert",
                                             "include/gl/triangles.frag",
                                             "include/gl/shadow_map.vert",
                                             "include/gl/shadow_map.frag",
                                             vertices.count_, vertices.vertices_);

    user::user_t user(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.07f, 0.07f, 0.07f), 0, 0, 0.1f, 20.0f);

    window.main_cycle(gl_program, user);

    return 0;
}
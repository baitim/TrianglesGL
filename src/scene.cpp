#include "window.hpp"
#include "scene.hpp"

int main() {
    user::user_t user(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.07f, 0.07f, 0.07f), 0, 0, 0.1f, 20.0f);
    window::window_t window(sf::VideoMode(800, 600), "Triangles", sf::ContextSettings(24, 8, 0, 3, 3));

    scene::scene_t<float> scene;
    std::cin >> scene;
    renderer::shaders_t shaders = {{{"include/gl/triangles.vs",  GL_VERTEX_SHADER},
                                    {"include/gl/triangles.fs",  GL_FRAGMENT_SHADER}},
                                   {{"include/gl/shadow_map.vs", GL_VERTEX_SHADER},
                                    {"include/gl/shadow_map.fs", GL_FRAGMENT_SHADER}}};
    renderer::renderer_t renderer(shaders, scene.get_vertices());


    window.main_cycle(renderer, user);

    return 0;
}
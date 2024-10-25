#include "window.hpp"

int main() {
    std::string file{__FILE__};
    std::filesystem::path dir = file.substr(0, file.rfind("/"));

    int w_width  = 800;
    int w_height = 600;
    window::window_t window(sf::VideoMode(w_width, w_height), "Triangles", sf::ContextSettings(24, 8, 0, 3, 3));
    
    std::vector<scene::scene_t<float>> scenes;
    try {
        scenes = scene::get_scenes(dir / "../scenes/scenes_in");
    } catch (const char* error_message) {
        std::cout << print_red(error_message) << "\n";
        return 1;
    }

    renderer::shaders_t shaders = {{{dir / "../include/gl/triangles.vs",  GL_VERTEX_SHADER},
                                    {dir / "../include/gl/triangles.fs",  GL_FRAGMENT_SHADER}},
                                   {{dir / "../include/gl/shadow_map.vs", GL_VERTEX_SHADER},
                                    {dir / "../include/gl/shadow_map.fs", GL_FRAGMENT_SHADER}}};
    renderer::renderer_t renderer(shaders, scenes[0].get_vertices(), w_width, w_height);

    glm::vec3 user_start_position = glm::vec3(0.f, 0.f, -7.f);
    glm::vec3 user_speed          = glm::vec3(.07f, .07f, .07f);
    float     user_zfar           = 70;
    user::user_t user(user_start_position, user_speed, 0, 0, 0.1f, user_zfar, scenes.size());

    window.main_cycle(renderer, user, scenes);

    return 0;
}
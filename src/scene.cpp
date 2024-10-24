#include "window.hpp"

int main() {
    std::string file{__FILE__};
    std::filesystem::path dir = file.substr(0, file.rfind("/"));

    int w_width  = 800;
    int w_height = 600;
    window::window_t window(sf::VideoMode(w_width, w_height), "Triangles", sf::ContextSettings(24, 8, 0, 3, 3));
    
    std::vector<scene::scene_t<float>> scenes = scene::get_scenes(dir / "../scenes/scenes_in");
    if (scenes.size() == 0) {
        std::cerr << "zero scenes uploaded\n";
        return 0;
    }

    renderer::shaders_t shaders = {{{dir / "../include/gl/triangles.vs",  GL_VERTEX_SHADER},
                                    {dir / "../include/gl/triangles.fs",  GL_FRAGMENT_SHADER}},
                                   {{dir / "../include/gl/shadow_map.vs", GL_VERTEX_SHADER},
                                    {dir / "../include/gl/shadow_map.fs", GL_FRAGMENT_SHADER}}};
    renderer::renderer_t renderer(shaders, scenes[0].get_vertices(), w_width, w_height);

    user::user_t user(glm::vec3(0.0f, 0.0f, -7.0f), glm::vec3(0.07f, 0.07f, 0.07f), 0, 0, 0.1f, 20.0f,
                      scenes.size());

    window.main_cycle(renderer, user, scenes);

    return 0;
}
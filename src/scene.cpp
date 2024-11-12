#include "window.hpp"

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << print_red("Invalid argument.\nUse one of these options:\n" \
                               "\t1) argc = 2, argv[1] = name of file\n"        \
                               "\t2) default scenes without your file\n");
        return 1;
    }

    int w_width  = 800;
    int w_height = 600;
    window::window_t window(sf::VideoMode(w_width, w_height),
                            "Triangles",
                            sf::ContextSettings(24, 8, 0, 3, 3));    

    std::string file{__FILE__};
    std::filesystem::path dir = file.substr(0, file.rfind("/"));
    std::vector<scene::scene_t<float>> scenes;
    try {
        scenes = scene::get_scenes(argc, argv, dir / "../scenes/scenes_in");
    } catch (const scene::error_t& error) {
        std::cout << print_red(error.what()) << "\n";
        return 1;
    }

    renderer::shaders_t shaders = {{{dir / "../include/gl/triangles.vs",  GL_VERTEX_SHADER},
                                    {dir / "../include/gl/triangles.fs",  GL_FRAGMENT_SHADER}},
                                   {{dir / "../include/gl/shadow_map.vs", GL_VERTEX_SHADER},
                                    {dir / "../include/gl/shadow_map.fs", GL_FRAGMENT_SHADER}}};
    renderer::renderer_t renderer(shaders, scenes[0].get_data(), w_width, w_height);

    glm::vec3 user_start_position = glm::vec3(0.f, 0.f, -7.f);
    glm::vec3 user_speed          = glm::vec3(.07f, .07f, .07f);
    float     user_zfar           = 70;
    user::user_t user(user_start_position, user_speed, 0, 0, 0.1f, user_zfar, scenes.size());

    window.main_cycle(renderer, user, scenes);

    return 0;
}
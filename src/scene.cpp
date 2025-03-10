#include "TrianglesGL/window.hpp"

using namespace triangles_gl;

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << print_red("Invalid argument.\nUse one of these options:\n" \
                               "\t1) argc = 2, argv[1] = name of file\n"        \
                               "\t2) default scenes without your file\n");
        return 1;
    }

    int w_width  = 800;
    int w_height = 600;
    window_t window(sf::VideoMode(w_width, w_height), "Triangles");

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + "/";
    try {
        init_gl();
        std::vector<scene_t<float>> scenes;
        scenes = get_scenes(argc, argv, dir + "scenes_in");

        std::string path2shaders = dir + "shaders/";
        std::vector<shader_t> triangles_shaders{{path2shaders + "triangles.vs", GL_VERTEX_SHADER},
                                                {path2shaders + "triangles.fs", GL_FRAGMENT_SHADER}};
        std::vector<shader_t> shadow_shaders{{path2shaders + "shadow_map.vs", GL_VERTEX_SHADER},
                                             {path2shaders + "shadow_map.fs", GL_FRAGMENT_SHADER}};
        renderer_t renderer(triangles_shaders, shadow_shaders,
                                    scenes[0].get_data(), w_width, w_height);

        glm::vec3 user_start_position = glm::vec3(0.f, 0.f, -7.f);
        glm::vec3 user_speed          = glm::vec3(.07f, .07f, .07f);
        float     user_zfar           = 70;
        user_t user(user_start_position, user_speed, 0, 0, 0.1f, user_zfar, scenes.size());

        window.main_cycle(renderer, user, scenes);

    } catch (const triangles_gl::error_t& error) {
        std::cout << error.what() << "\n";
        return 1;
    }
    return 0;
}
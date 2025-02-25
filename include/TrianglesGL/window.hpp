#pragma once

#include "TrianglesGL/user.hpp"
#include "TrianglesGL/scene.hpp"

namespace triangles_gl {

    class window_t final {
        sf::Window window_;

        void update_on_event(const std::vector<scene_t<float>>& scenes,
                             window_event_e response,
                             renderer_t& renderer,
                             user_t& user) {

            sf::Vector2u size = window_.getSize();
            switch (response) {
                case window_event_e::RESIZED:
                        renderer.resize(size.x, size.y);
                        user.set_aspect(size.x, size.y);
                    break;

                case window_event_e::NEW_SCENE:
                        renderer.rebind(
                            scenes[user.get_number_scene()].get_data(),
                            size.x, size.y
                        );
                    break;

                default:
                    break;
            }
        }

        void update_default(user_t& user) {
            user.update_default();
        }

    public:
        window_t(const sf::VideoMode& window_video_mode, const std::string& window_name) {
            window_.create(window_video_mode, window_name, sf::Style::Default,
                           sf::ContextSettings(24, 8, 0, 3, 3));
        }

        void main_cycle(renderer_t& renderer, user_t& user,
                        const std::vector<scene_t<float>>& scenes) {
            while (window_.isOpen()) {
                sf::Event event;
                while (window_.pollEvent(event)) {
                    window_event_e response = user.event_callback(event, window_);
                    if (response == window_event_e::EXIT)
                        return;
                    update_on_event(scenes, response, renderer, user);
                }
                update_default(user);

                renderer.render(user.get_perspective(), user.get_lookat());
                window_.display();
            }
        }
    };
}
#pragma once

#include "user.hpp"
#include "scene.hpp"

namespace window {

    class window_t final {
        sf::Window window_;

        void on_event(const std::vector<scene::scene_t<float>>& scenes,
                      user::window_event_e  response,
                      renderer::renderer_t& renderer,
                      user::user_t&         user) {

            sf::Vector2u size = window_.getSize();
            switch (response) {
                case user::window_event_e::RESIZED:
                        renderer.resize(size.x, size.y);
                        user.set_aspect(size.x, size.y);
                    break;

                case user::window_event_e::NEW_SCENE:
                        renderer.rebind_scene(scenes[user.get_number_scene()].get_data(),
                                              size.x, size.y);
                    break;

                default:
                    break;
            }
        }

    public:
        window_t(const sf::VideoMode& window_video_mode, const std::string& window_name,
                 const sf::ContextSettings& context) {
            window_.create(window_video_mode, window_name, sf::Style::Default, context);
            window_.setMouseCursorVisible(false);
        }

        void main_cycle(renderer::renderer_t& renderer, user::user_t& user,
                        const std::vector<scene::scene_t<float>>& scenes) {
            while (window_.isOpen()) {
                sf::Event event;
                while (window_.pollEvent(event)) {
                    user::window_event_e response = user.event_callback(event, window_);
                    if (response == user::window_event_e::EXIT)
                        return;
                    on_event(scenes, response, renderer, user);
                }

                renderer.render(user.get_user_direction(), user.get_perspective(), user.get_lookat());
                window_.display();
            }
        }

        ~window_t() {
            window_.close();
        }
    };
}
#pragma once

#include "user.hpp"

namespace window {

    class window_t final {
        sf::Window window_;

    public:
        window_t(const sf::VideoMode& window_video_mode, const std::string& window_name,
                 const sf::ContextSettings& context) {
            window_.create(window_video_mode, window_name, sf::Style::Default, context);
        }

        void main_cycle(renderer::renderer_t& renderer, user::user_t& user) {
            while (window_.isOpen()) {
                sf::Event event;
                while (window_.pollEvent(event)) {
                    user::window_event_e response = user.event_callback(event, window_);
                    if (response == user::window_event_e::WINDOW_EVENT_EXIT) {
                        return;
                    } else if (response == user::window_event_e::WINDOW_EVENT_RESIZED) {
                        sf::Vector2u size = window_.getSize();
                        renderer.resize(size.x, size.y);
                        user.set_aspect(size.x, size.y);
                    }
                }

                renderer.render(user.get_perspective(), user.get_lookat());
                window_.display();
            }
        }

        ~window_t() {
            window_.close();
        }
    };
}
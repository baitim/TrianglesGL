#pragma once

#include "user.hpp"

namespace window {

    class window_t final {
        sf::Window window_;

    public:
        window_t(const sf::VideoMode& window_video_mode, const char* window_name) {
            window_.create(window_video_mode, window_name);
        }

        void main_cycle(shaders::gl_shaders_program_t& gl_program, user::user_t& user) {
            glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
            while (window_.isOpen()) {
                sf::Event event;
                while (window_.pollEvent(event)) {
                    if (user.event_callback(event, window_) == user::window_event_e::WINDOW_EVENT_EXIT)
                        return;
                }

                gl_program.update_vertices(user.get_perspective(), user.get_lookat());
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLES, 0, gl_program.get_count_vertices());
                window_.display();
            }
        }

        ~window_t() {
            window_.close();
        }
    };
}
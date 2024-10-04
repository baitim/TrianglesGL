#pragma once

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

namespace window {

    class window_t final {
        sf::Window window_;

    public:
        window_t(const sf::VideoMode& window_video_mode, const char* window_name) {
            window_.create(window_video_mode, window_name);
        }

        void main_cycle() {
            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
            while (window_.isOpen()) {
                sf::Event event;
                while (window_.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        return;
                }
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                window_.display();
            }
        }

        ~window_t() {
            window_.close();
        }
    };
}
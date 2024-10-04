#pragma once

#include <SFML/Window.hpp>
#include "real_numbers.hpp"
#include "shaders.hpp"

namespace user {

    enum class window_event_e {
        WINDOW_EVENT_NO_EXIT = 0,
        WINDOW_EVENT_EXIT    = 1
    };

    class user_t {
        glm::vec3 user_position_;
        glm::vec3 user_view_;
        glm::vec3 user_lookup_;
        glm::vec3 speed_;
        const float speed_rotate_ = 1.0f;

        float aspect_     = 16.0f / 9.0f;
        float z_near_     = 0.1f;
        float z_far_      = 100.0f;
        float view_angle_ = 45.0f;
        const float aspect_zoom_speed_ = 1.1f;

    public:
        user_t(const glm::vec3& user_position, const glm::vec3& user_view,
               const glm::vec3& user_lookup, const glm::vec3& speed) :
               user_position_(user_position), user_view_(user_view),
               user_lookup_  (user_lookup),   speed_(speed) {}

        window_event_e user_event_callback(const sf::Event& event) {
            switch (event.type) {
                case sf::Event::Closed:
                    return window_event_e::WINDOW_EVENT_EXIT;

                case sf::Event::MouseWheelScrolled:
                    switch (real_numbers::is_real_gt(event.mouseWheelScroll.delta, 0.0f)) {
                        case true:
                            view_angle_ = std::min(150.0f, view_angle_ * aspect_zoom_speed_);
                            break;

                        case false:
                            view_angle_ /= aspect_zoom_speed_;
                            break;
                    }
                    break;

                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::A:
                            user_position_.x -= speed_.x;
                            break;
                        case sf::Keyboard::D:
                            user_position_.x += speed_.x;
                            break;
                        case sf::Keyboard::W:
                            user_position_.y -= speed_.y;
                            break;
                        case sf::Keyboard::S:
                            user_position_.y += speed_.y;
                            break;
                        case sf::Keyboard::Q:
                            user_position_.z -= speed_.z;
                            break;
                        case sf::Keyboard::E:
                            user_position_.z += speed_.z;
                            break;

                        case sf::Keyboard::Left:
                            user_lookup_.x -= speed_rotate_;
                            break;
                        case sf::Keyboard::Right:
                            user_lookup_.x += speed_rotate_;
                            break;
                        case sf::Keyboard::Down:
                            user_lookup_.y -= speed_rotate_;
                            break;
                        case sf::Keyboard::Up:
                            user_lookup_.y += speed_rotate_;
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            return window_event_e::WINDOW_EVENT_NO_EXIT;
        }

        inline glm::highp_mat4 get_lookat() const {
            return glm::lookAt(user_position_, user_view_, user_lookup_);
        }

        inline glm::highp_mat4 get_perspective() const {
            return glm::perspective(glm::radians(view_angle_), aspect_, z_near_, z_far_);
        }
    };
}
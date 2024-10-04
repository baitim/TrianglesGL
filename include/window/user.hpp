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
        glm::vec3 speed_;

        float horizontal_angle_   = 0.2 * M_PI;
        float vertical_angle_     = 1.2 * M_PI;
        const float speed_rotate_ = M_PI / 100.f;

        float aspect_     = 16.0f / 9.0f;
        float z_near_     = 0.1f;
        float z_far_      = 100.0f;
        float view_angle_ = 45.0f;
        const float aspect_zoom_speed_ = 1.1f;

    public:
        user_t(const glm::vec3& user_position, const glm::vec3& speed,
               float horizontal_angle, float vertical_angle) :
               user_position_(user_position), speed_(speed),
               horizontal_angle_(horizontal_angle), vertical_angle_(vertical_angle) {}

        window_event_e user_event_callback(const sf::Event& event) {
            switch (event.type) {
                case sf::Event::Closed:
                    return window_event_e::WINDOW_EVENT_EXIT;

                case sf::Event::MouseWheelScrolled:
                    switch (real_numbers::is_real_gt(event.mouseWheelScroll.delta, 0.0f)) {
                        case true:
                            view_angle_ = std::min(80.0f, view_angle_ * aspect_zoom_speed_);
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
                            horizontal_angle_ -= speed_rotate_;
                            break;
                        case sf::Keyboard::Right:
                            horizontal_angle_ += speed_rotate_;
                            break;
                        case sf::Keyboard::Down:
                            vertical_angle_ -= speed_rotate_;
                            break;
                        case sf::Keyboard::Up:
                            vertical_angle_ += speed_rotate_;
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
            glm::vec3 direction(
                cos(vertical_angle_) * sin(horizontal_angle_),
                sin(vertical_angle_),
                cos(vertical_angle_) * cos(horizontal_angle_)
            );

            glm::vec3 right = glm::vec3(
                sin(horizontal_angle_ - M_PI / 2.0f),
                0,
                cos(horizontal_angle_ - M_PI / 2.0f)
            );

            glm::vec3 lookup = glm::cross(right, direction);

            return glm::lookAt(user_position_, user_position_ + direction, lookup);
        }

        inline glm::highp_mat4 get_perspective() const {
            return glm::perspective(glm::radians(view_angle_), aspect_, z_near_, z_far_);
        }
    };
}
#pragma once

#include "real_numbers.hpp"
#include "renderer.hpp"
#include <SFML/Window.hpp>

namespace user {

    enum class window_event_e {
        DEFAULT,
        EXIT,
        RESIZED,
        NEW_SCENE
    };

    class user_t final {
        glm::vec3 user_position_;
        glm::vec3 speed_;

        float horizontal_angle_   = M_PI;
        float vertical_angle_     = 0.0f;
        const float speed_rotate_ = 0.005f;

        float aspect_     = 16.0f / 9.0f;
        float z_near_     = 0.1f;
        float z_far_      = 1000.0f;
        float view_angle_ = 45.0f;
        float min_view_angle_ = 30.0f;
        float max_view_angle_ = 60.0f;
        const float aspect_zoom_speed_ = 1.1f;

        bool is_mouse_active = false;
        sf::Vector2i mouse_position_{0, 0};

        glm::vec3 direction_;
        glm::vec3 right_;

        int number_scene_  = 0;
        int count_scenes_ = 0;

        void update_direction_() {
            direction_ = glm::vec3{
                cos(vertical_angle_) * sin(horizontal_angle_),
                sin(vertical_angle_),
                cos(vertical_angle_) * cos(horizontal_angle_)
            };
        }

        void update_right_() {
            right_ = glm::vec3{
                sin(horizontal_angle_ - M_PI / 2.0f),
                0,
                cos(horizontal_angle_ - M_PI / 2.0f)
            };
        }

    public:
        user_t(const glm::vec3& user_position, const glm::vec3& speed,
               float horizontal_angle, float vertical_angle,
               float z_near, float z_far, int count_scenes) :
               user_position_(user_position), speed_(speed),
               horizontal_angle_(horizontal_angle), vertical_angle_(vertical_angle),
               z_near_(z_near), z_far_(z_far), count_scenes_(count_scenes) {}

        window_event_e event_callback(const sf::Event& event, const sf::Window& window) {
            update_direction_();
            update_right_();

            bool is_mouse_was_active = is_mouse_active;
            switch (event.type) {
                case sf::Event::Closed:
                    return window_event_e::EXIT;

                case sf::Event::Resized:
                    return window_event_e::RESIZED;

                case sf::Event::MouseWheelScrolled:
                    switch (real_numbers::is_real_gt(event.mouseWheelScroll.delta, 0.0f)) {
                        case true:
                            view_angle_ = std::min(max_view_angle_, view_angle_ * aspect_zoom_speed_);
                            break;
                        case false:
                            view_angle_ = std::max(min_view_angle_, view_angle_ / aspect_zoom_speed_);
                            break;
                    }
                    break;

                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::A:
                            user_position_ -= right_ * speed_;
                            break;
                        case sf::Keyboard::D:
                            user_position_ += right_ * speed_;
                            break;
                        case sf::Keyboard::W:
                            user_position_ += direction_ * speed_;
                            break;
                        case sf::Keyboard::S:
                            user_position_ -= direction_ * speed_;
                            break;
                        case sf::Keyboard::Q:
                            is_mouse_active = !is_mouse_active;
                            break;
                        case sf::Keyboard::E:
                            number_scene_ = (number_scene_ + 1) % count_scenes_;
                            return window_event_e::NEW_SCENE;
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            if (is_mouse_active) {
                sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
                if (is_mouse_was_active) {
                    horizontal_angle_ -= speed_rotate_ * (mouse_position.x - mouse_position_.x);
                    vertical_angle_   -= speed_rotate_ * (mouse_position.y - mouse_position_.y);
                }
                mouse_position_ = mouse_position;
            }

            update_direction_();
            update_right_();
            return window_event_e::DEFAULT;
        }

        void set_aspect(int width, int height) {
            aspect_ = static_cast<float>(width) / static_cast<float>(height);
        }

        inline glm::highp_mat4 get_lookat() const {
            glm::vec3 lookup = glm::cross(right_, direction_);
            return glm::lookAt(user_position_, user_position_ + direction_, lookup);
        }

        inline glm::highp_mat4 get_perspective() const {
            return glm::perspective(glm::radians(view_angle_), aspect_, z_near_, z_far_);
        }

        int get_number_scene() const {
            return number_scene_;
        }
    };
}
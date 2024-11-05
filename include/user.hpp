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

        float aspect_     = 16.0f / 9.0f;
        float z_near_     = 0.1f;
        float z_far_      = 1000.0f;
        float view_angle_ = 45.0f;
        float min_view_angle_ = 30.0f;
        float max_view_angle_ = 60.0f;
        const float aspect_zoom_speed_ = 1.1f;

              float     speed_rotate_ = 0.f;
        const float   d_speed_rotate_ = 0.001f;
        const float max_speed_rotate_ = 0.005f;
        int  last_step_ = 0;
        bool is_mouse_active_           = false;
        bool is_mouse_setted_in_center_ = false;
        sf::Vector2i mouse_position_ {0, 0};
        sf::Vector2i mouse_direction_{0, 0};
        int mouse_borders_ = 20;

        glm::vec3 direction_;
        glm::vec3 right_;

        int number_scene_ = 0;
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

        void set_in_borders(const sf::Vector2i& mouse_position, const sf::Window& window) {
            sf::Vector2u window_size = window.getSize();
            sf::Vector2i window_center = {static_cast<int>(window_size.x / 2),
                                          static_cast<int>(window_size.y / 2)};
                                        
            int dx = abs(mouse_position.x - window_center.x);
            int dy = abs(mouse_position.y - window_center.y);
            if (dx + dy > mouse_borders_) {
                sf::Mouse::setPosition(window_center, window);
                is_mouse_setted_in_center_ = true;
            }
        }

        void update_mouse(const sf::Window& window) {
            sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
            if (is_mouse_active_) {
                if (!is_mouse_setted_in_center_) {
                    horizontal_angle_ -= speed_rotate_ * (mouse_position.x - mouse_position_.x);
                    vertical_angle_   -= speed_rotate_ * (mouse_position.y - mouse_position_.y);

                    last_step_ =   abs(mouse_position.x - mouse_position_.x)
                                 + abs(mouse_position.y - mouse_position_.y);

                    if (mouse_position.x > mouse_position_.x) mouse_direction_.x =  1;
                    if (mouse_position.y > mouse_position_.y) mouse_direction_.y =  1;
                    if (mouse_position.x < mouse_position_.x) mouse_direction_.x = -1;
                    if (mouse_position.y < mouse_position_.y) mouse_direction_.y = -1;

                    if (last_step_)
                        speed_rotate_ = std::min(max_speed_rotate_, speed_rotate_ + d_speed_rotate_);
                }
                is_mouse_setted_in_center_ = false;
                mouse_position_ = mouse_position;
                set_in_borders(mouse_position, window);
            }
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
                        case sf::Keyboard::Escape:
                            return window_event_e::EXIT;

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
                            is_mouse_active_ = !is_mouse_active_;
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
            update_mouse(window);

            update_direction_();
            update_right_();
            return window_event_e::DEFAULT;
        }

        void update_default() {
            if (!is_mouse_active_) {
                float normilize_by_last_step = static_cast<float>(last_step_) / static_cast<float>(mouse_borders_);
                horizontal_angle_ -= normilize_by_last_step * speed_rotate_ * mouse_direction_.x;
                vertical_angle_   -= normilize_by_last_step * speed_rotate_ * mouse_direction_.y;
                speed_rotate_ = std::max(0.f, speed_rotate_ - d_speed_rotate_);
                if (real_numbers::is_real_eq(speed_rotate_, 0.f)) {
                    mouse_direction_ = {0, 0};
                    last_step_ = 0;
                }
            }
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

        glm::vec3 get_user_direction() const {
            return direction_;
        }

        int get_number_scene() const {
            return number_scene_;
        }
    };
}
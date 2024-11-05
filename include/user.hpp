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

    struct mouse_t final {
              float     speed_rotate_ = 0.f;
        const float   d_speed_rotate_ = 0.001f;
        const float max_speed_rotate_ = 0.005f;
        bool is_mouse_active_           = false;
        bool is_mouse_setted_in_center_ = false;
        sf::Vector2i mouse_position_{0, 0};
        sf::Vector2i mouse_step_    {0, 0};
        int mouse_borders_ = 20;

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

        void update_mouse(float& horizontal_angle, float& vertical_angle, const sf::Window& window) {
            sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
            if (is_mouse_active_) {
                if (!is_mouse_setted_in_center_) {
                    horizontal_angle -= speed_rotate_ * (mouse_position.x - mouse_position_.x);
                    vertical_angle   -= speed_rotate_ * (mouse_position.y - mouse_position_.y);

                    mouse_step_.x = mouse_position.x - mouse_position_.x;
                    mouse_step_.y = mouse_position.y - mouse_position_.y;

                    if (abs(mouse_step_.x) + abs(mouse_step_.y))
                        speed_rotate_ = std::min(max_speed_rotate_, speed_rotate_ + d_speed_rotate_);
                }
                is_mouse_setted_in_center_ = false;
                mouse_position_ = mouse_position;
                set_in_borders(mouse_position, window);
            }
        }

        void update_default(float& horizontal_angle, float& vertical_angle) {
            if (!is_mouse_active_) {
                float normilized_speed_rotate = speed_rotate_ / static_cast<float>(mouse_borders_);
                horizontal_angle -= static_cast<float>(mouse_step_.x) * normilized_speed_rotate;
                vertical_angle   -= static_cast<float>(mouse_step_.y) * normilized_speed_rotate;
                speed_rotate_ = std::max(0.f, speed_rotate_ - d_speed_rotate_);
                if (real_numbers::is_real_eq(speed_rotate_, 0.f))
                    mouse_step_ = {0, 0};
            }
        }
    };

    struct viewer_t final {
        float horizontal_angle_   = M_PI;
        float vertical_angle_     = 0.0f;

        float aspect_     = 16.0f / 9.0f;
        float z_near_     = 0.1f;
        float z_far_      = 1000.0f;
        float view_angle_ = 45.0f;
        float min_view_angle_ = 30.0f;
        float max_view_angle_ = 60.0f;
        const float aspect_zoom_speed_ = 1.1f;

        glm::vec3 direction_;
        glm::vec3 right_;

        viewer_t(float horizontal_angle, float vertical_angle, float z_near, float z_far) :
                horizontal_angle_(horizontal_angle), vertical_angle_(vertical_angle),
                z_near_(z_near), z_far_(z_far) {}
        
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
    };

    class user_t final {
        glm::vec3 user_position_;
        glm::vec3 speed_;
        mouse_t  m;
        viewer_t v;

        int number_scene_ = 0;
        int count_scenes_ = 0;

    public:
        user_t(const glm::vec3& user_position, const glm::vec3& speed,
               float horizontal_angle, float vertical_angle,
               float z_near, float z_far, int count_scenes) :
               user_position_(user_position), speed_(speed),
               v(horizontal_angle, vertical_angle, z_near, z_far), count_scenes_(count_scenes) {}

        window_event_e event_callback(const sf::Event& event, const sf::Window& window) {
            v.update_direction_();
            v.update_right_();

            switch (event.type) {
                case sf::Event::Closed:
                    return window_event_e::EXIT;

                case sf::Event::Resized:
                    return window_event_e::RESIZED;

                case sf::Event::MouseWheelScrolled:
                    switch (real_numbers::is_real_gt(event.mouseWheelScroll.delta, 0.0f)) {
                        case true:
                            v.view_angle_ = std::min(v.max_view_angle_, v.view_angle_ * v.aspect_zoom_speed_);
                            break;
                        case false:
                            v.view_angle_ = std::max(v.min_view_angle_, v.view_angle_ / v.aspect_zoom_speed_);
                            break;
                    }
                    break;

                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            return window_event_e::EXIT;

                        case sf::Keyboard::A:
                            user_position_ -= v.right_ * speed_;
                            break;
                        case sf::Keyboard::D:
                            user_position_ += v.right_ * speed_;
                            break;
                        case sf::Keyboard::W:
                            user_position_ += v.direction_ * speed_;
                            break;
                        case sf::Keyboard::S:
                            user_position_ -= v.direction_ * speed_;
                            break;
                        case sf::Keyboard::Q:
                            m.is_mouse_active_ = !m.is_mouse_active_;
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
            m.update_mouse(v.horizontal_angle_, v.vertical_angle_, window);

            v.update_direction_();
            v.update_right_();
            return window_event_e::DEFAULT;
        }

        void update_default() {
            m.update_default(v.horizontal_angle_, v.vertical_angle_);
        }

        void set_aspect(int width, int height) {
            v.aspect_ = static_cast<float>(width) / static_cast<float>(height);
        }

        inline glm::highp_mat4 get_lookat() const {
            glm::vec3 lookup = glm::cross(v.right_, v.direction_);
            return glm::lookAt(user_position_, user_position_ + v.direction_, lookup);
        }

        inline glm::highp_mat4 get_perspective() const {
            return glm::perspective(glm::radians(v.view_angle_), v.aspect_, v.z_near_, v.z_far_);
        }

        glm::vec3 get_user_direction() const {
            return v.direction_;
        }

        int get_number_scene() const {
            return number_scene_;
        }
    };
}
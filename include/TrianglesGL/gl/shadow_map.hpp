#pragma once

#include "TrianglesGL/scene.hpp"
#include "TrianglesGL/gl/shader.hpp"

namespace triangles_gl {
    class shadow_map_t final {

        program_t program_;
        GLuint shadow_map_id_;

        int width_ = 0;
        int height_ = 0;
        
        glm::mat4 depth_MVP_;
        glm::vec3 light_direction_;

    private:
        void init_texture(int width, int height) {
            width_  = width;
            height_ = height;

            gl_handler(glGenTextures, 1, &shadow_map_id_);
            gl_handler(glBindTexture, GL_TEXTURE_2D, shadow_map_id_);
            gl_handler(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width,
                       height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            gl_handler(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl_handler(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            gl_handler(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            gl_handler(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        void set_buffer(GLuint& VBO) const {
            gl_handler(glGenFramebuffers, 1, &VBO);
            gl_handler(glBindFramebuffer, GL_FRAMEBUFFER, VBO);
            gl_handler(glDrawBuffer, GL_NONE);
            gl_handler(glReadBuffer, GL_NONE);
            gl_handler(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                       GL_TEXTURE_2D, shadow_map_id_, 0);
        }

        void set_uniform_depth_MVP(GLuint program_id, const light_t& l) {
            glm::mat4 depth_view_matrix = glm::lookAt(l.light_position, l.light_direction, l.light_up);
            glm::mat4 depth_model_matrix = glm::mat4(1.0);
            depth_MVP_ = l.depth_projection_matrix * depth_view_matrix * depth_model_matrix;
            gl_handler(glUniformMatrix4fv, glGetUniformLocation(program_id, "depth_MVP"),
                       1, GL_FALSE, &depth_MVP_[0][0]);
        }

        void draw_shadows(const light_t& light, int count_vertices) {
            gl_handler(glUseProgram, program_.id());
            init_texture(light.width, light.height);

            GLuint VBO;
            set_buffer(VBO);
            set_uniform_depth_MVP(program_.id(), light);

            gl_handler(glViewport, 0, 0, light.width, light.height);
            gl_handler(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gl_handler(glDrawArrays, GL_TRIANGLES, 0, count_vertices);
            gl_handler(glBindFramebuffer, GL_FRAMEBUFFER, 0);
            gl_handler(glDeleteFramebuffers, 1, &VBO);
        }

        void clear() {
            gl_handler(glDeleteTextures, 1, &shadow_map_id_);
        }

        void init(const light_t& light, int count_vertices) {
            light_direction_ = light.light_direction;
            draw_shadows(light, count_vertices);
        }

    public:
        shadow_map_t(const light_t& light, int count_vertices, const std::vector<shader_t>& shaders)
            : program_(shaders) {
            init(light, count_vertices);
        }

        shadow_map_t(const shadow_map_t& rhs) : program_(rhs.program_),
                                                depth_MVP_(rhs.depth_MVP_),
                                                light_direction_(rhs.light_direction_) {
            gl_handler(glGenTextures, 1, &shadow_map_id_);
            gl_handler(glBindTexture, GL_TEXTURE_2D, shadow_map_id_);
            gl_handler(glCopyImageSubData, rhs.shadow_map_id_, GL_TEXTURE_2D, 0, 0, 0, 0,
                       shadow_map_id_, GL_TEXTURE_2D, 0, 0, 0, 0, rhs.width_, rhs.height_, 1);
        }

        shadow_map_t& operator=(const shadow_map_t& rhs) {
            if (this == &rhs)
                return *this;

            shadow_map_t new_shadow_map{rhs};
            std::swap(*this, new_shadow_map);
            return *this;
        }

        shadow_map_t(shadow_map_t&& rhs) noexcept : program_(std::move(rhs.program_)), 
                                                    shadow_map_id_(std::exchange(rhs.shadow_map_id_, 0)),
                                                    depth_MVP_(std::move(rhs.depth_MVP_)),
                                                    light_direction_(std::move(rhs.light_direction_)) {}

        shadow_map_t& operator=(shadow_map_t&& rhs) noexcept {
            shadow_map_id_ = std::exchange(rhs.shadow_map_id_, 0);
            std::swap(program_, rhs.program_);
            std::swap(depth_MVP_, rhs.depth_MVP_);
            std::swap(light_direction_, rhs.light_direction_);
            return *this;
        }

        void rebind(const light_t& light, int count_vertices) {
            clear();
            init(light, count_vertices);
        }

        void set_uniform_shadow_map(GLuint program_id) const {
            gl_handler(glActiveTexture, GL_TEXTURE0);
            gl_handler(glBindTexture, GL_TEXTURE_2D, shadow_map_id_);
            gl_handler(glUniform1i, glGetUniformLocation(program_id, "shadow_map"), 0);
        }

        void set_uniform_depth_bias_MVP(GLuint program_id) const {
            glm::mat4 bias_matrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
            );
            glm::mat4 depth_bias_MVP = bias_matrix * depth_MVP_;
            gl_handler(glUniformMatrix4fv, glGetUniformLocation(program_id, "depth_bias_MVP"),
                       1, GL_FALSE, &depth_bias_MVP[0][0]);
        }

        void set_uniform_light_direction(GLuint program_id) const {
            glm::vec3 light_dir = glm::normalize(light_direction_);
            gl_handler(glUniform3f, glGetUniformLocation(program_id, "light_dir"),
                       light_dir.x, light_dir.y, light_dir.z);
        }

        ~shadow_map_t() {
            clear();
        }
    };
}
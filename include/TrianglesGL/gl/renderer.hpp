#pragma once

#include "TrianglesGL/gl/program.hpp"
#include "TrianglesGL/gl/shadow_map.hpp"
#include "TrianglesGL/gl/vertex_array.hpp"
#include <algorithm>
#include <chrono>

namespace triangles_gl {
    class renderer_t final {

        using chrono_time_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
        chrono_time_t start_time_;

        int count_vertices_ = 0;

        program_t      program_;
        vertex_array_t vertex_array_;
        shadow_map_t   shadow_map_;

    private:
        void set_uniform_time() const {
            auto elapsed_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_seconds = elapsed_time - start_time_;
            float normalized_time = std::fabs(std::sin(1.5f * elapsed_seconds.count()) / 5.f);
            float valid_time      = std::clamp(normalized_time, 0.0f, 0.2f);
            glUniform1f(glGetUniformLocation(program_.id(), "time"), valid_time);
        }

        void set_uniform_MVP(const glm::highp_mat4& user_perspective,
                             const glm::highp_mat4& user_lookat) const {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 MVP   = user_perspective * user_lookat * model;
            glUniformMatrix4fv(glGetUniformLocation(program_.id(), "MVP"),
                               1, GL_FALSE, &MVP[0][0]);
        }

        void set_uniform_colors() const {
            glm::vec3 colors[2] = {{0.f, 0.f, 1.f}, {1.f, 0.f, 0.f}};
            glUniform3fv(glGetUniformLocation(program_.id(), "colors"), 2, &colors[0][0]);
        }

        void set_uniform_is_cw(bool is_cw) const {
            glUniform1i(glGetUniformLocation(program_.id(), "is_cw"), is_cw);
        }

        void init(const data2render_t& data2render, int w_width, int w_height) {
            glUseProgram(program_.id());

            resize(w_width, w_height);

            shadow_map_.set_uniform_shadow_map(program_.id());
            shadow_map_.set_uniform_light_direction(program_.id());
            set_uniform_colors();
        }

        void render_cw() const {
            glFrontFace(GL_CW);
            set_uniform_is_cw(true);
            glDrawArrays(GL_TRIANGLES, 0, count_vertices_);
        }

        void render_ccw() const {
            glFrontFace(GL_CCW);
            set_uniform_is_cw(false);
            glDrawArrays(GL_TRIANGLES, 0, count_vertices_);
        }

    public:
        renderer_t(const std::vector<shader_t>& triangles_shaders,
            const std::vector<shader_t>& shadow_shaders,
                   const data2render_t& data2render,
                   int w_width, int w_height)
                   : start_time_(std::chrono::high_resolution_clock::now()),
                     count_vertices_(data2render.vertices_.size()),
                     program_(triangles_shaders),
                     vertex_array_(data2render.vertices_),
                     shadow_map_(data2render.light_, count_vertices_, shadow_shaders) {
            init(data2render, w_width, w_height);
        }

        void render(const glm::highp_mat4& user_perspective,
                    const glm::highp_mat4& user_lookat) const {

            set_uniform_time();
            set_uniform_MVP(user_perspective, user_lookat);
            shadow_map_.set_uniform_depth_bias_MVP(program_.id());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render_cw ();
            render_ccw();
        }

        void rebind(const data2render_t& data2render, int w_width, int w_height) {
            count_vertices_ = data2render.vertices_.size();
            vertex_array_.rebind(data2render.vertices_);
            shadow_map_.rebind(data2render.light_, count_vertices_);

            init(data2render, w_width, w_height);
        }

        void resize(int width, int height) const {
            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, 0, height, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
    };
}
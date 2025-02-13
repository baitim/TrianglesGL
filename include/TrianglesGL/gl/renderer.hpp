#pragma once

#include "TrianglesGL/gl/program.hpp"
#include "TrianglesGL/gl/shadow_map.hpp"
#include "TrianglesGL/gl/vertex_array.hpp"

namespace renderer {
    class renderer_t final {
        shader::shaders_pack_t triangles_shaders_;

        program::program_t           program_;
        shadow_map::shadow_map_t     shadow_map_;
        vertex_array::vertex_array_t vertex_array_;
        int count_vertices_ = 0;

    private:
        void init_gl() const {
            glewExperimental = true;
            if (glewInit() != GLEW_OK)
                throw common::error_t{str_red("Unable to initialize GLEW")};

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
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

        void start_program(const scene::data2render_t& data2render, 
                           int w_width, int w_height) {
            count_vertices_ = data2render.vertices_.size();
            vertex_array_.bind_vertices(data2render.vertices_);
            shadow_map_.init(program_.id(), data2render.light_, count_vertices_);

            triangles_shaders_.load_shaders(program_.id());
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
        renderer_t(const shader::shaders_pack_t& triangles_shaders,
                   const shader::shaders_pack_t& shadow_shaders,
                   const scene::data2render_t& data2render,
                   int w_width, int w_height)
                   : triangles_shaders_(triangles_shaders), shadow_map_(shadow_shaders) {
            init_gl();
            start_program(data2render, w_width, w_height);
        }

        void render(const glm::highp_mat4& user_perspective,
                    const glm::highp_mat4& user_lookat) const {

            program_.set_uniform_time();
            set_uniform_MVP(user_perspective, user_lookat);
            shadow_map_.set_uniform_depth_bias_MVP(program_.id());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render_cw ();
            render_ccw();
        }

        void rebind_scene(const scene::data2render_t& data2render, int w_width, int w_height) {
            start_program(data2render, w_width, w_height);
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
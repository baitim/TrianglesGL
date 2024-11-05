#pragma once

#include "shader.hpp"
#include "scene.hpp"
#include <bit>
#include <chrono>
#include <cstddef>

namespace renderer {
    struct shaders_t final {
        using container_type = std::vector<std::pair<std::string, GLenum>>;
        container_type triangles_;
        container_type shadows_;
        shaders_t(const container_type& triangles, const container_type& shadows) :
                  triangles_(triangles), shadows_(shadows) {}
    };

    class shadow_map_t final {
        GLuint id_;
        glm::mat4 depth_MVP_;
        glm::vec3 light_direction_;

    private:
        void init_texture(int width, int height) {
            glGenTextures(1, &id_);
            glBindTexture(GL_TEXTURE_2D, id_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width,
                         height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        void set_buffer(GLuint VBO) const {
            glGenFramebuffers(1, &VBO);
            glBindFramebuffer(GL_FRAMEBUFFER, VBO);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id_, 0);
        }

        void set_uniform_depth_MVP(const scene::light_t& l, GLuint program_id) {
            glm::mat4 depth_view_matrix = glm::lookAt(l.light_position_, l.light_direction_, l.light_up_);
            glm::mat4 depth_model_matrix = glm::mat4(1.0);
            depth_MVP_ = l.depth_projection_matrix_ * depth_view_matrix * depth_model_matrix;
            glUniformMatrix4fv(glGetUniformLocation(program_id, "depth_MVP"),
                               1, GL_FALSE, &depth_MVP_[0][0]);
        }

    public:
        void init(const scene::light_t& light, GLuint program_id, int count_vertices) {
            light_direction_ = light.light_direction_;

            init_texture(light.width_, light.height_);
            GLuint shadows_frame_buffer;
            set_buffer(shadows_frame_buffer);
            set_uniform_depth_MVP(light, program_id);

            glViewport(0, 0, light.width_, light.height_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, count_vertices);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint    get_id()        const { return id_; }
        glm::vec3 get_light_dir() const { return light_direction_; }
        glm::mat4 get_depth_MVP() const { return depth_MVP_; }
    };

    class renderer_t final {
        shaders_t shaders_;
        GLuint program_id_;
        shadow_map_t shadow_map_;
        int count_vertices_ = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;

    private:
        void init_gl() const {
            glewExperimental = true;
            if (glewInit() != GLEW_OK) {
                std::cerr << "Unable to initialize GLEW\n";
                return;
            }

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        }

        void process_creation_result(GLint result) const {
            if (result)
                return;

            std::cerr << "Error in create shaders program\n";
            int info_log_length;
            glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0) {
                std::vector<char> error_message(info_log_length + 1);
                glGetProgramInfoLog(program_id_, info_log_length, NULL, &error_message[0]);
                std::cerr <<  &error_message[0] << "\n";
            }
        }

        void delete_attached_shaders() {
            GLsizei max_count = 2;
            GLsizei count;
            GLuint  attached_shaders[max_count];
            glGetAttachedShaders(program_id_, max_count, &count, attached_shaders);
            for (int i = 0; i < count; ++i)
                glDeleteShader(attached_shaders[i]);
        }

        void load_shaders(const std::vector<std::pair<std::string, GLenum>>& shaders) {
            program_id_ = glCreateProgram();
            for (int i = 0, end = shaders.size(); i < end; ++i)
                glAttachShader(program_id_, shader::shader_t{shaders[i].first, shaders[i].second}.get_id());
            glLinkProgram(program_id_);

            GLint result;
            glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
            process_creation_result(result);

            delete_attached_shaders();
        }

        void bind_vertices(const vertices::vertices_t& vertices) const {
            GLuint VAO, VBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.count_ * sizeof(*vertices.vertices_.get()),
                         vertices.vertices_.get(), GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);

            size_t vertex_size = sizeof(*vertices.vertices_.get());
            void* coord_offset   = std::bit_cast<void*>(offsetof(vertices::vertex2render_t, coord));
            void* normal_offset  = std::bit_cast<void*>(offsetof(vertices::vertex2render_t, normal));
            void* color_offset   = std::bit_cast<void*>(offsetof(vertices::vertex2render_t, color));
            void* is_dark_offset = std::bit_cast<void*>(offsetof(vertices::vertex2render_t, is_dark));
            glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, vertex_size, coord_offset);
            glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, vertex_size, normal_offset);
            glVertexAttribIPointer(2, 1, GL_BYTE,            vertex_size, color_offset);
            glVertexAttribIPointer(3, 1, GL_BYTE,            vertex_size, is_dark_offset);
        }

        void set_uniform_time() const {
            auto elapsed_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_seconds = elapsed_time - start_time_;
            float normalized_time = std::max(0.0f, std::min(0.2f, std::fabs(std::sin(1.5f * elapsed_seconds.count()) / 5.f)));
            glUniform1f(glGetUniformLocation(program_id_, "time"), normalized_time);
        }

        void set_uniform_MVP(const glm::highp_mat4& user_perspective,
                             const glm::highp_mat4& user_lookat) const {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 MVP   = user_perspective * user_lookat * model;
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "MVP"),
                               1, GL_FALSE, &MVP[0][0]);
        }

        void set_uniform_depth_bias_MVP(const glm::highp_mat4& user_perspective,
                                        const glm::highp_mat4& user_lookat) const {
            glm::mat4 bias_matrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
            );
            glm::mat4 depth_bias_MVP = bias_matrix * shadow_map_.get_depth_MVP();
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "depth_bias_MVP"),
                               1, GL_FALSE, &depth_bias_MVP[0][0]);
        }

        void set_uniform_shadow_map() const {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadow_map_.get_id());
            glUniform1i(glGetUniformLocation(program_id_, "shadow_map"), 0);
        }

        void set_uniform_colors() const {
            glm::vec3 colors[2] = {{0.f, 0.f, 1.f}, {1.f, 0.f, 0.f}};
            glUniform3fv(glGetUniformLocation(program_id_, "colors"), 2, &colors[0][0]);
        }

        void set_uniform_light_direction() const {
            glm::vec3 light_dir = glm::normalize(shadow_map_.get_light_dir());
            glUniform3f(glGetUniformLocation(program_id_, "light_dir"),
                        light_dir.x, light_dir.y, light_dir.z);
        }

        void set_uniform_user_direction(const glm::vec3& user_direction) const {
            glm::vec3 user_dir = glm::normalize(user_direction);
            glUniform3f(glGetUniformLocation(program_id_, "user_dir"),
                        user_dir.x, user_dir.y, user_dir.z);
        }

        void start_program(const scene::data2render_t& data2render, 
                           int w_width, int w_height) {
            count_vertices_ = data2render.vertices_.count_;
            bind_vertices(data2render.vertices_);

            load_shaders(shaders_.shadows_);
            glUseProgram(program_id_);
            shadow_map_.init(data2render.light_, program_id_, count_vertices_);

            load_shaders(shaders_.triangles_);
            glUseProgram(program_id_);

            resize(w_width, w_height);
            start_time_ = std::chrono::high_resolution_clock::now();

            set_uniform_shadow_map();
            set_uniform_colors();
            set_uniform_light_direction();
        }

    public:
        renderer_t(const shaders_t& shaders, const scene::data2render_t& data2render,
                   int w_width, int w_height) : shaders_(shaders) {
            init_gl();
            start_program(data2render, w_width, w_height);
        }

        void render(const glm::vec3&       user_direction,
                    const glm::highp_mat4& user_perspective,
                    const glm::highp_mat4& user_lookat) const {

            set_uniform_time();
            set_uniform_MVP(user_perspective, user_lookat);
            set_uniform_depth_bias_MVP(user_perspective, user_lookat);
            set_uniform_user_direction(user_direction);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, count_vertices_);
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
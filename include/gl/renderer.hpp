#pragma once

#include "shader.hpp"
#include "vertices.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bit>
#include <chrono>

namespace renderer {
    struct shaders_t final {
        using container_type = std::vector<std::pair<std::string, GLenum>>;
        container_type triangles_;
        container_type shadows_;
        shaders_t(container_type triangles, container_type shadows) : triangles_(triangles), shadows_(shadows) {}
    };

    class shadow_map_t final {
        GLuint id_;
        int width_ = 2048;
        int height_ = 2048;
        glm::mat4 depth_MVP_;

    private:
        void init_texture() {
            glGenTextures(1, &id_);
            glBindTexture(GL_TEXTURE_2D, id_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_,
                         height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        void set_buffer(GLuint VBO) {
            glGenFramebuffers(1, &VBO);
            glBindFramebuffer(GL_FRAMEBUFFER, VBO);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id_, 0);
        }

        void set_uniform_depth_MVP(GLuint program_id) {
            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-1.4, 1.4, -1.4, 1.4, 0.1, 5);
            glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(2,2,2), glm::vec3(-1,-1,-1), glm::vec3(0,1,0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);
            depth_MVP_ = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
            GLuint depth_MVP_id = glGetUniformLocation(program_id, "depth_MVP");
            glUniformMatrix4fv(depth_MVP_id, 1, GL_FALSE, &depth_MVP_[0][0]);
        }

    public:
        void init(GLuint program_id, int count_vertices) {
            init_texture();
            GLuint shadows_frame_buffer;
            set_buffer(shadows_frame_buffer);
            set_uniform_depth_MVP(program_id);

            glViewport(0, 0, width_, height_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, count_vertices);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint get_id() const { return id_; }

        glm::mat4 get_depth_MVP() const { return depth_MVP_; }
    };

    class renderer_t final {
        GLuint program_id_;
        shadow_map_t shadow_map_;
        int count_vertices_ = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;

    private:
        void init_gl() {
            glewExperimental = true;
            if (glewInit() != GLEW_OK) {
                std::cerr << "Unable to initialize GLEW\n";
                return;
            }

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        }

        void process_creation_result(GLint result) {
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

        void create_shaders_program(const std::vector<GLuint>& shaders_id)  {
            program_id_ = glCreateProgram();
            for (int i = 0, end = shaders_id.size(); i < end; ++i)
                glAttachShader(program_id_, shaders_id[i]);
            glLinkProgram(program_id_);

            GLint result;
            glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
            process_creation_result(result);
        }

        void delete_shaders(const std::vector<GLuint>& shaders_id)  {
            for (int i = 0, end = shaders_id.size(); i < end; ++i)
                glDeleteShader(shaders_id[i]);
        }

        void load_shaders(const std::vector<std::pair<std::string, GLenum>>& shaders) {
            std::vector<GLuint> shaders_id;
            for (int i = 0, end = shaders.size(); i < end; ++i) {
                shader::shader_t shader{shaders[i].first, shaders[i].second};
                shaders_id.push_back(shader.get_id());
            }
            create_shaders_program(shaders_id);
            delete_shaders(shaders_id);
        }

        void bind_vertices(const vertices::vertices_t& vertices) {
            GLuint VAO, VBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, count_vertices_ * sizeof(*vertices.vertices_.get()),
                         vertices.vertices_.get(), GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            size_t glfloat_sz = sizeof(GLfloat);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * glfloat_sz, std::bit_cast<void*>(0 * glfloat_sz));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * glfloat_sz, std::bit_cast<void*>(3 * glfloat_sz));
        }

        void set_uniform_time() {
            auto elapsed_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_seconds = elapsed_time - start_time_;
            float normalized_time = std::max(0.0f, std::min(0.2f, std::fabs(std::sin(1.5f * elapsed_seconds.count()) / 5.f)));
            GLint location_time = glGetUniformLocation(program_id_, "time");
            glUniform1f(location_time, normalized_time);
        }

        void set_uniform_MVP(const glm::highp_mat4& user_perspective,
                             const glm::highp_mat4& user_lookat) {
            glm::mat4 Model = glm::mat4(1.0f);
            glm::mat4 MVP   = user_perspective * user_lookat * Model;
            GLuint MVP_id   = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
        }

        void set_uniform_depth_bias_MVP(const glm::highp_mat4& user_perspective,
                                        const glm::highp_mat4& user_lookat) {
            glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
            );
            glm::mat4 depth_bias_MVP = biasMatrix * shadow_map_.get_depth_MVP();
            GLuint depth_bias_MVP_id = glGetUniformLocation(program_id_, "depth_bias_MVP");
            glUniformMatrix4fv(depth_bias_MVP_id, 1, GL_FALSE, &depth_bias_MVP[0][0]);
        }

        void set_uniform_shadow_map() {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadow_map_.get_id());
            glUniform1i(glGetUniformLocation(program_id_, "shadow_map"), 0);
        }

    public:
        renderer_t(const shaders_t& shaders, const vertices::vertices_t& vertices) : count_vertices_(vertices.count_) {
            init_gl();

            bind_vertices(vertices);

            load_shaders(shaders.shadows_);
            glUseProgram(program_id_);
            shadow_map_.init(program_id_, vertices.count_);

            load_shaders(shaders.triangles_);
            glUseProgram(program_id_);

            start_time_ = std::chrono::high_resolution_clock::now();
        }

        void render(const glm::highp_mat4& user_perspective,
                    const glm::highp_mat4& user_lookat) {

            set_uniform_time();
            set_uniform_MVP(user_perspective, user_lookat);
            set_uniform_depth_bias_MVP(user_perspective, user_lookat);
            set_uniform_shadow_map();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, count_vertices_);
        }

        void resize(int width, int height) {
            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, 0, height, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
    };
}
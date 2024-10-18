#pragma once

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"

namespace renderer {
    struct shaders_t final {
        using container_type = std::vector<std::pair<std::string, GLenum>>;
        container_type triangles_;
        container_type shadows_;
        shaders_t(container_type triangles, container_type shadows) : triangles_(triangles), shadows_(shadows) {}
    };

    class renderer_t final {
        GLuint VAO_;
        GLuint VBO_;
        GLuint program_id_;
        GLuint shadows_frame_buffer_;
        GLuint shadow_map_;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;

        int size_vertices_ = 0;
        const GLfloat* vertices_;

    private:
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

        void load_shaders(const std::vector<std::pair<std::string, GLenum>>& shaders_configs) {
            std::vector<GLuint> shaders_id;
            for (int i = 0, end = shaders_configs.size(); i < end; ++i) {
                shader::shader_t shader{shaders_configs[i].first, shaders_configs[i].second};
                shaders_id.push_back(shader.get_id());
            }
            create_shaders_program(shaders_id);
            delete_shaders(shaders_id);
        }

    public:
        renderer_t(const shaders_t& shaders, const int size_vertices, const GLfloat* vertices) :
                   size_vertices_(size_vertices), vertices_(vertices) {

            glewExperimental = true;
            if (glewInit() != GLEW_OK) {
                std::cerr << "Unable to initialize GLEW\n";
                return;
            }

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_TEXTURE_2D);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, size_vertices_ * sizeof(*vertices_), vertices_, GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

            load_shaders(shaders.shadows_);
            glUseProgram(program_id_);

            glGenTextures(1, &shadow_map_);
            glBindTexture(GL_TEXTURE_2D, shadow_map_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glGenFramebuffers(1, &shadows_frame_buffer_);
            glBindFramebuffer(GL_FRAMEBUFFER, shadows_frame_buffer_);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-1.4, 1.4, -1.4, 1.4, 0.1, 5);
            glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(2,2,2), glm::vec3(-1,-1,-1), glm::vec3(0,1,0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);
            glm::mat4 depth_MVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
            GLuint depth_MVP_id = glGetUniformLocation(program_id_, "depth_MVP");
            glUniformMatrix4fv(depth_MVP_id, 1, GL_FALSE, &depth_MVP[0][0]);

            glViewport(0, 0, 2048, 2048);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, size_vertices_);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            load_shaders(shaders.triangles_);
            glUseProgram(program_id_);

            start_time_ = std::chrono::high_resolution_clock::now();
        }

        void update_vertices(const glm::highp_mat4& user_perspective,
                             const glm::highp_mat4& user_lookat) {

            auto elapsed_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_seconds = elapsed_time - start_time_;
            float normalized_time = std::max(0.0f, std::min(0.2f, std::fabs(std::sin(1.5f * elapsed_seconds.count()) / 5.f)));
            GLint location_time = glGetUniformLocation(program_id_, "time");
            glUniform1f(location_time, normalized_time);

            glm::mat4 Model = glm::mat4(1.0f);
            glm::mat4 MVP   = user_perspective * user_lookat * Model;
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);

            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-1.4, 1.4, -1.4, 1.4, 0.1, 5);
            glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(2,2,2), glm::vec3(-1,-1,-1), glm::vec3(0,1,0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);
            glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
            glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
            );
            glm::mat4 depth_bias_MVP = biasMatrix * depthMVP;
            GLuint depth_bias_MVP_id = glGetUniformLocation(program_id_, "depth_bias_MVP");
            glUniformMatrix4fv(depth_bias_MVP_id, 1, GL_FALSE, &depth_bias_MVP[0][0]);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadow_map_);
            glUniform1i(glGetUniformLocation(program_id_, "shadow_map"), 0);
        }

        void resize(int width, int height) {
            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, 0, height, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }

        inline int get_count_vertices() const { return size_vertices_ / 6; }
    };
}
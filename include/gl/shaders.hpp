#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace shaders {

    class gl_shader_t final {
        std::string shader_code_;
        GLenum shader_type_;
        GLuint shader_id_;

    private:
        void get_shader_code(const char* file_path) {
            std::ifstream shader_stream(file_path, std::ios::in);
            if (shader_stream.is_open()) {
                std::stringstream sstr;
                sstr << shader_stream.rdbuf();
                shader_code_ = sstr.str();
                shader_stream.close();
            }
        }

        void install_shader()  {
            shader_id_ = glCreateShader(shader_type_);
            char const* code = shader_code_.c_str();
            glShaderSource(shader_id_, 1, &code , NULL);
            glCompileShader(shader_id_);

            GLint result = GL_FALSE;
            glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &result);
            if (!result) {
                std::cerr << "Error in install shader\n";
                int info_log_length;
                glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &info_log_length);
                if (info_log_length > 0) {
                    std::vector<char> error_message(info_log_length + 1);
                    glGetShaderInfoLog(shader_id_, info_log_length, NULL, &error_message[0]);
                    std::cerr << &error_message[0] << "\n";
                }
            }
        }

    public:
        gl_shader_t(const char* file_path, GLenum shader_type) : shader_type_(shader_type) {
            get_shader_code(file_path);
            install_shader();
        }

        GLuint get_shader_id() const { return shader_id_; }
    };


    class gl_shaders_program_t final {
        GLuint VAO_;
        GLuint VBO_;
        GLuint program_id_;
        GLuint shadows_frame_buffer_;
        GLuint shadow_map_;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;

        int size_vertices_ = 0;
        const GLfloat* vertices_;

    private:
        void create_shaders_program(GLuint vertex_shader_id, GLuint fragment_shader_id)  {
            program_id_ = glCreateProgram();
            glAttachShader(program_id_, vertex_shader_id);
            glAttachShader(program_id_, fragment_shader_id);
            glLinkProgram(program_id_);

            GLint result = GL_FALSE;
            glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
            if (!result) {
                std::cerr << "Error in create shaders program\n";
                int info_log_length;
                glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &info_log_length);
                if (info_log_length > 0) {
                    std::vector<char> error_message(info_log_length + 1);
                    glGetProgramInfoLog(program_id_, info_log_length, NULL, &error_message[0]);
                    std::cerr <<  &error_message[0] << "\n";
                }
            }
        }

        void load_shaders(const char* vertex_file_path, const char* fragment_file_path) {
            gl_shader_t vertex_shader(vertex_file_path, GL_VERTEX_SHADER);
            gl_shader_t fragment_shader(fragment_file_path, GL_FRAGMENT_SHADER);

            GLuint vertex_shader_id   = vertex_shader.get_shader_id();
            GLuint fragment_shader_id = fragment_shader.get_shader_id();

            create_shaders_program(vertex_shader_id, fragment_shader_id);

            glDeleteShader(vertex_shader_id);
            glDeleteShader(fragment_shader_id);
        }

    public:
        gl_shaders_program_t(const char* triangles_vertex_shader,  const char* triangles_fragment_shader,
                             const char* shadow_map_vertex_shader, const char* shadow_map_fragment_shader,
                             const int size_vertices, const GLfloat* vertices) :
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

            load_shaders(shadow_map_vertex_shader, shadow_map_fragment_shader);
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

            load_shaders(triangles_vertex_shader, triangles_fragment_shader);
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
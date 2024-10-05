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
        gl_shaders_program_t(const char* vertex_file_path, const char* fragment_file_path,
                             const int size_vertices, const GLfloat* vertices) :
                             size_vertices_(size_vertices), vertices_(vertices) {

            glewExperimental = true;
            if (glewInit() != GLEW_OK) {
                std::cerr << "Unable to initialize GLEW\n";
                return;
            }

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LESS);

            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, size_vertices_ * sizeof(*vertices_), vertices_, GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

            start_time_ = std::chrono::high_resolution_clock::now();

            load_shaders(vertex_file_path, fragment_file_path);
            glUseProgram(program_id_);
        }

        void update_vertices(const glm::highp_mat4& user_perspective,
                             const glm::highp_mat4& user_lookat) {

            auto elapsed_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_seconds = elapsed_time - start_time_;
            float normalized_time = std::max(0.0f, std::min(0.2f, std::fabs(std::sin(1.5f * elapsed_seconds.count()) / 5.f)));
            GLint location_time = glGetUniformLocation(program_id_, "time");
            glUniform1f(location_time, normalized_time);

            glm::mat4 Projection = user_perspective;
            glm::mat4 View  = user_lookat;
            glm::mat4 Model = glm::mat4(1.0f);
            glm::mat4 MVP   = Projection * View * Model;
            GLuint matrix_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &MVP[0][0]);

            glBindVertexArray(VAO_);
        }

        inline int get_count_vertices() const { return size_vertices_ / 6; }
    };
}
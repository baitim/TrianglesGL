#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/glut.h>

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
            if (!result)
                std::cerr << "Error in install shader\n";
        }

    public:
        gl_shader_t(const char* file_path, GLenum shader_type) : shader_type_(shader_type) {
            get_shader_code(file_path);
            install_shader();
        }

        GLuint get_shader_id() const { return shader_id_; }
    };


    class gl_shaders_program_t final {
        GLuint vertex_array_id_;
        GLuint vertex_buffer_;
        GLuint program_id_;

    private:
        void create_shaders_program(GLuint vertex_shader_id, GLuint fragment_shader_id)  {
            program_id_ = glCreateProgram();
            glAttachShader(program_id_, vertex_shader_id);
            glAttachShader(program_id_, fragment_shader_id);
            glLinkProgram(program_id_);

            GLint result = GL_FALSE;
            glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
            if (!result)
                std::cerr << "Error in create shaders program\n";
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
                             const int count_vertexes, const GLfloat* vertex_buffer_data) {
            glewExperimental = true;
            if (glewInit() != GLEW_OK) {
                std::cerr << "Невозможно инициализировать GLEW\n";
                return;
            }
            
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            glGenBuffers(1, &vertex_buffer_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
            glBufferData(GL_ARRAY_BUFFER, count_vertexes * sizeof(*vertex_buffer_data),
                         vertex_buffer_data, GL_STATIC_DRAW);

            load_shaders(vertex_file_path, fragment_file_path);

            glUseProgram(program_id_);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }
    };
}
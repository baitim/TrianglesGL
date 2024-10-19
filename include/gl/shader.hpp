#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

namespace shader {
    class shader_t final {
        std::string shader_code_;
        GLenum shader_type_;
        GLuint shader_id_;

    private:
        void get_code(const std::string& file_path) {
            std::ifstream shader_stream(file_path, std::ios::in);
            if (shader_stream.is_open()) {
                std::stringstream sstr;
                sstr << shader_stream.rdbuf();
                shader_code_ = sstr.str();
                shader_stream.close();
            }
        }

        void process_installation_result(GLint result) const {
            if (result)
                return;

            std::cerr << "Error in install shader\n";
            int info_log_length;
            glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0) {
                std::vector<char> error_message(info_log_length + 1);
                glGetShaderInfoLog(shader_id_, info_log_length, NULL, &error_message[0]);
                std::cerr << &error_message[0] << "\n";
            }
        }

        void install() {
            shader_id_ = glCreateShader(shader_type_);
            char const* code = shader_code_.c_str();
            glShaderSource(shader_id_, 1, &code , NULL);
            glCompileShader(shader_id_);

            GLint result;
            glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &result);
            process_installation_result(result);
        }

    public:
        shader_t(const std::string& file_path, GLenum shader_type) : shader_type_(shader_type) {
            get_code(file_path);
            install();
        }

        GLuint get_id() const { return shader_id_; }
    };
}
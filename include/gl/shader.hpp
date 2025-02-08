#pragma once

#include <GL/glew.h>
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

        void install(const std::string& file_path) {
            get_code(file_path);

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
            install(file_path);
        }

        GLuint id() const noexcept { return shader_id_; }

        ~shader_t() {
            glDeleteShader(shader_id_);
        }
    };

    class shaders_pack_t final {
        using shaders_container_t = std::vector<std::pair<std::string, GLenum>>;
        shaders_container_t shaders_;

    private:
        void process_creation_result(GLuint program_id, GLint result) const {
            if (result)
                return;

            std::cerr << "Error in create shaders program\n";
            int info_log_length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0) {
                std::vector<char> error_message(info_log_length + 1);
                glGetProgramInfoLog(program_id, info_log_length, NULL, &error_message[0]);
                std::cerr <<  &error_message[0] << "\n";
            }
        }

        void delete_attached_shaders(GLuint program_id) {
            GLsizei max_count = 2;
            GLsizei count;
            GLuint  attached_shaders[max_count];
            glGetAttachedShaders(program_id, max_count, &count, attached_shaders);
            for (int i = 0; i < count; ++i)
                glDeleteShader(attached_shaders[i]);
        }

    public:
        shaders_pack_t(const shaders_container_t& shaders) : shaders_(shaders) {}

        void load_shaders(GLuint& program_id) {
            program_id = glCreateProgram();
            for (int i = 0, end = shaders_.size(); i < end; ++i)
                glAttachShader(program_id, shader::shader_t{shaders_[i].first, shaders_[i].second}.id());
            glLinkProgram(program_id);

            GLint result;
            glGetProgramiv(program_id, GL_LINK_STATUS, &result);
            process_creation_result(program_id, result);

            delete_attached_shaders(program_id);
        }

        void clear_memory() const {
            for (auto shader : shaders_)
                glDeleteShader(shader.second);
        }

        ~shaders_pack_t() {
            clear_memory();
        }
    };
}
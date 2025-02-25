#pragma once

#include "TrianglesGL/common.hpp"
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>

namespace triangles_gl {
    class shader_t final {
        std::string shader_code_;
        GLenum shader_type_;
        GLuint shader_id_;

    private:
        void process_installation_result(GLint result) const {
            if (result)
                return;

            std::string error_str = "Error in install shader\n";
            int info_log_length;
            glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0) {
                std::vector<char> error_message(info_log_length + 1);
                glGetShaderInfoLog(shader_id_, info_log_length, NULL, &error_message[0]);
                error_str += &error_message[0];
            }
            throw error_t{str_red(error_str)};
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
        shader_t(const std::string& file_path, GLenum shader_type)
        : shader_code_(file2str(file_path)), shader_type_(shader_type) {
            install();
        }

        shader_t(const shader_t& rhs) : shader_code_(rhs.shader_code_),
                                        shader_type_(rhs.shader_type_),
                                        shader_id_(glCreateShader(rhs.shader_type_)) {
            install();
        }

        shader_t& operator=(const shader_t& rhs) {
            if (this == &rhs)
                return *this;

            shader_t new_shader{rhs};
            std::swap(*this, new_shader);
            return *this;
        }

        shader_t(shader_t&& rhs) noexcept : shader_code_(std::move(rhs.shader_code_)),
                                            shader_type_(rhs.shader_type_),
                                            shader_id_(std::exchange(rhs.shader_id_, 0)) {}

        shader_t& operator=(shader_t&& rhs) noexcept {
            shader_id_ = std::exchange(rhs.shader_id_, 0);
            std::swap(shader_code_, rhs.shader_code_);
            std::swap(shader_type_, rhs.shader_type_);
            return *this;
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

            std::string error_str = "Error in create shaders program\n";
            int info_log_length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0) {
                std::vector<char> error_message(info_log_length + 1);
                glGetProgramInfoLog(program_id, info_log_length, NULL, &error_message[0]);
                error_str += &error_message[0];
            }
            throw error_t{str_red(error_str)};
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
                glAttachShader(program_id, shader_t{shaders_[i].first, shaders_[i].second}.id());
            glLinkProgram(program_id);

            GLint result;
            glGetProgramiv(program_id, GL_LINK_STATUS, &result);
            process_creation_result(program_id, result);

            delete_attached_shaders(program_id);
        }
    };
}
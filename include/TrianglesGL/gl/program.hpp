#pragma once

#include "TrianglesGL/gl/shader.hpp"

namespace triangles_gl {
    class program_t final {
        GLuint program_id_;
        std::vector<shader_t> shaders_;

    private:
        static void process_creation_result(GLuint program_id, GLint result) {
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

            detach_shaders();
            glDeleteProgram(program_id_);
            throw error_t{str_red(error_str)};
        }

        void detach_shaders() {
            GLsizei max_count = 2;
            GLsizei count;
            GLuint  attached_shaders[max_count];
            glGetAttachedShaders(program_id_, max_count, &count, attached_shaders);
            for (int i = 0; i < count; ++i)
                glDetachShader(program_id_, attached_shaders[i]);
        }

    public:
        program_t(std::vector<shader_t> shaders) : shaders_(shaders) {
            program_id_ = glCreateProgram();
            for (auto shader : shaders_)
                glAttachShader(program_id_, shader.id());
            glLinkProgram(program_id_);

            GLint result;
            glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
            process_creation_result(program_id_, result);
        }

        program_t(const program_t& rhs) : program_t(rhs.shaders_) {}

        program_t& operator=(const program_t& rhs) {
            if (this == &rhs) 
                return *this;

            program_t new_program{rhs};
            std::swap(*this, new_program);
            return *this;
        }

        program_t(program_t&& rhs) noexcept : program_id_(std::exchange(rhs.program_id_, 0)),
                                              shaders_(std::move(rhs.shaders_)) {}

        program_t& operator=(program_t&& rhs) noexcept {
            program_id_ = std::exchange(rhs.program_id_, 0);
            shaders_ = std::move(rhs.shaders_);
            return *this;
        }

        GLuint id() const noexcept { return program_id_; }

        ~program_t() {
            detach_shaders();
            glDeleteProgram(program_id_);
        }
    };
}
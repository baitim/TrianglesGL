#pragma once

#include "TrianglesGL/gl/shader.hpp"

namespace triangles_gl {
    class program_t final {
        GLuint program_id_;
        std::vector<shader_t> shaders_;

        struct program_guard_deleter {
            void operator()(GLuint* ptr_program) const {
                glDeleteProgram(*ptr_program);
            }
        };

    private:
        static void detach_shaders(GLuint program_id) {
            GLsizei max_count = 2;
            GLsizei count;
            std::vector<GLuint> attached_shaders(max_count);
            gl_handler(glGetAttachedShaders, program_id, max_count, &count, attached_shaders.data());
            for (int i = 0; i < count; ++i)
                gl_handler(glDetachShader, program_id, attached_shaders[i]);
        }

        static void process_creation_result(GLuint program_id, GLint result) {
            if (result)
                return;

            std::string error_str = "Error in create shaders program\n";
            int info_log_length;
            gl_handler(glGetProgramiv, program_id, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0) {
                std::vector<char> error_message(info_log_length + 1);
                gl_handler(glGetProgramInfoLog, program_id, info_log_length, nullptr, &error_message[0]);
                error_str += &error_message[0];
            }

            detach_shaders(program_id);
            gl_handler(glDeleteProgram, program_id);
            throw error_t{str_red(error_str)};
        }

    public:
        program_t(std::vector<shader_t> shaders) : shaders_(shaders) {
            program_id_ = gl_handler(glCreateProgram);
            std::unique_ptr<GLuint, program_guard_deleter> program_guard(&program_id_);

            for (auto shader : shaders_)
                gl_handler(glAttachShader, program_id_, shader.id());
            gl_handler(glLinkProgram, program_id_);

            GLint result;
            gl_handler(glGetProgramiv, program_id_, GL_LINK_STATUS, &result);
            process_creation_result(program_id_, result);

            program_guard.release();
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
            detach_shaders(program_id_);
            glDeleteProgram(program_id_);
        }
    };
}
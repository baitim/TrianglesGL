#pragma once

#include "shader.hpp"
#include <chrono>
#include <cmath>

namespace program {
    class program_t final {
        GLuint program_id_;

        using chrono_time_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
        chrono_time_t start_time_;

    public:
        program_t() {
            start_time_ = std::chrono::high_resolution_clock::now();
        }

        void set_uniform_time() const {
            auto elapsed_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_seconds = elapsed_time - start_time_;
            float normalized_time = std::fabs(std::sin(1.5f * elapsed_seconds.count()) / 5.f);
            float valid_time      = std::clamp(normalized_time, 0.0f, 0.2f);
            glUniform1f(glGetUniformLocation(program_id_, "time"), valid_time);
        }

        GLuint  id() const noexcept { return program_id_; }
        GLuint& id()       noexcept { return program_id_; }

        void clear_memory() const {
            glDeleteProgram(program_id_);
        }

        ~program_t() {
            clear_memory();
        }
    };
}
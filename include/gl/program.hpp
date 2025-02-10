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

        program_t(const program_t& rhs) : program_id_(glCreateProgram()),
                                          start_time_(rhs.start_time_) {}

        program_t& operator=(const program_t& rhs) {
            if (this == &rhs) 
                return *this;

            program_t new_program{rhs};
            std::swap(*this, new_program);
            return *this;
        }

        program_t(program_t&& rhs) noexcept : program_id_(std::exchange(rhs.program_id_, 0)),
                                              start_time_(std::move(rhs.start_time_)) {}

        program_t& operator=(program_t&& rhs) noexcept {
            program_id_ = std::exchange(rhs.program_id_, 0);
            std::swap(start_time_, rhs.start_time_);
            return *this;
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

        ~program_t() {
            glDeleteProgram(program_id_);
        }
    };
}
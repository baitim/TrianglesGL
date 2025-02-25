#pragma once

#include "TrianglesGL/common.hpp"
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>

namespace triangles_gl {

    inline void init_gl() {
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
            throw error_t{str_red("Unable to initialize GLEW")};

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    }

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

        // need to glCreateShader() before
        void install() {
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
            shader_id_ = glCreateShader(shader_type_);
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
}
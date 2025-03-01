#pragma once

#include "TrianglesGL/common.hpp"
#include <GL/glew.h>

namespace triangles_gl {
    inline void check_gl_error(std::string_view func_name, const char* file, const char* callerFunction) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            throw error_t{str_red(
              "OpenGL error in " + std::string(func_name)
            + " called from " + callerFunction + " in " + file + "\n"
            + "OpenGL error code: " + std::to_string(error)
            )};
        }
    }

    #define gl_handler(func, ...) gl_handler_(func, #func, __FILE__, __func__ __VA_OPT__(,) __VA_ARGS__)

    template <typename Func, typename... Args>
    inline auto gl_handler_(Func func, std::string_view func_name, const char* file,
                            const char* caller_func, Args&&... args)
                            -> decltype(func(std::forward<Args>(args)...)) {
        if constexpr(std::is_void_v<decltype(func(std::forward<Args>(args)...))>) {
            func(std::forward<Args>(args)...);
            check_gl_error(func_name, file, caller_func);
        } else {
            auto result = func(std::forward<Args>(args)...);
            check_gl_error(func_name, file, caller_func);
            return result;
        }
    }

    inline void init_gl() {
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
            throw error_t{str_red("Unable to initialize GLEW")};

        gl_handler(glEnable, GL_DEPTH_TEST);
        gl_handler(glEnable, GL_CULL_FACE);
        gl_handler(glCullFace, GL_BACK);
        gl_handler(glDepthFunc, GL_LESS);
        gl_handler(glDepthMask, GL_TRUE);
        gl_handler(glClearColor, 0.3f, 0.3f, 0.3f, 0.0f);
    }
}
#pragma once

#include "TrianglesGL/scene.hpp"
#include <bit>

namespace triangles_gl {
    class vertex_array_t final {
        GLuint VAO_ = 0;
        GLuint VBO_ = 0;

    private:
        void clear() {
            gl_handler(glDeleteVertexArrays, 1, &VAO_);
            gl_handler(glDeleteBuffers, 1, &VBO_);
        }
        
        void bind_VAO_VBO() {
            gl_handler(glGenVertexArrays, 1, &VAO_);
            gl_handler(glGenBuffers, 1, &VBO_);
            
            gl_handler(glBindVertexArray, VAO_);
            gl_handler(glBindBuffer, GL_ARRAY_BUFFER, VBO_);
        }

        void set_VAO() {
            gl_handler(glEnableVertexAttribArray, 0);
            gl_handler(glEnableVertexAttribArray, 1);
            gl_handler(glEnableVertexAttribArray, 2);
            
            using data = vertex2render_t;
            size_t vertex_size = sizeof(data);
            void* coord_offset   = reinterpret_cast<void*>(offsetof(data, coord));
            void* normal_offset  = reinterpret_cast<void*>(offsetof(data, normal));
            void* color_offset   = reinterpret_cast<void*>(offsetof(data, color));
            
            gl_handler(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, vertex_size, coord_offset);
            gl_handler(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, vertex_size, normal_offset);
            gl_handler(glVertexAttribIPointer, 2, 1, GL_BYTE, vertex_size, color_offset);
        }

        void init(const std::vector<vertex2render_t>& vertices) {
            bind_VAO_VBO();
            
            size_t vertex_size = sizeof(vertex2render_t);
            gl_handler(glBufferData, GL_ARRAY_BUFFER, vertices.size() * vertex_size,
                    vertices.data(), GL_STATIC_DRAW);

            set_VAO();
        }

    public:
        vertex_array_t(const std::vector<vertex2render_t>& vertices) {
            init(vertices);
        }

        vertex_array_t(const vertex_array_t& rhs) {
            bind_VAO_VBO();

            gl_handler(glBindVertexArray, rhs.VAO_);
            gl_handler(glBindBuffer, GL_ARRAY_BUFFER, rhs.VBO_);
        
            GLint buffer_size = 0;
            gl_handler(glGetBufferParameteriv, GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
        
            gl_handler(glBufferData, GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);
            gl_handler(glCopyBufferSubData, GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, buffer_size);
        
            set_VAO();
        }

        vertex_array_t& operator=(const vertex_array_t& rhs) {
            if (this == &rhs)
                return *this;

            vertex_array_t new_vertex_array{rhs};
            std::swap(*this, new_vertex_array);
            return *this;
        }

        vertex_array_t(vertex_array_t&& rhs) noexcept : VAO_(std::exchange(rhs.VAO_, 0)),
                                                        VBO_(std::exchange(rhs.VBO_, 0)) {}

        vertex_array_t& operator=(vertex_array_t&& rhs) noexcept {
            std::swap(VAO_, rhs.VAO_);
            std::swap(VBO_, rhs.VBO_);
            return *this;
        }

        void rebind(const std::vector<vertex2render_t>& vertices) {
            clear();
            init(vertices);
        }

        ~vertex_array_t() {
            glDeleteVertexArrays(1, &VAO_);
            glDeleteBuffers(1, &VBO_);
        }
    };
}
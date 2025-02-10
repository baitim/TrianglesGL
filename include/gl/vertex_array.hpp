#pragma once

#include "scene.hpp"
#include <bit>

namespace vertex_array {
    class vertex_array_t final {
        GLuint VAO_ = 0;
        GLuint VBO_ = 0;

    private:
        void clear_memory() {
            glDeleteVertexArrays(1, &VAO_);
            glDeleteBuffers(1, &VBO_);
        }

    public:
        vertex_array_t() {}

        vertex_array_t(const vertex_array_t& rhs) {
            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
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

        void bind_vertices(const std::vector<scene::vertex2render_t>& vertices) {
            clear_memory();

            size_t vertex_size = sizeof(scene::vertex2render_t);

            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * vertex_size,
                         vertices.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            using data = scene::vertex2render_t;
            void* coord_offset   = std::bit_cast<void*>(offsetof(data, coord));
            void* normal_offset  = std::bit_cast<void*>(offsetof(data, normal));
            void* color_offset   = std::bit_cast<void*>(offsetof(data, color));
            glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, vertex_size, coord_offset);
            glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, vertex_size, normal_offset);
            glVertexAttribIPointer(2, 1, GL_BYTE,            vertex_size, color_offset);
        }

        ~vertex_array_t() {
            clear_memory();
        }
    };
}
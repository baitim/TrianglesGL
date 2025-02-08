#pragma once

#include "scene.hpp"
#include <bit>

namespace vertex_array {
    class vertex_array_t final {
        GLuint VAO_, VBO_;

    public:
        void bind_vertices(const std::vector<scene::vertex2render_t>& vertices) {
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

        void clear_memory() const {
            glDeleteVertexArrays(1, &VAO_);
            glDeleteBuffers(1, &VBO_);
        }

        ~vertex_array_t() {
            clear_memory();
        }
    };
}
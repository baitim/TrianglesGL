#pragma once

#include "triangle.hpp"
#include <GL/glew.h>
#include <memory>

namespace vertices {

    const int TRIANGLE2VERTEX_SIZE = 27; // 3 * 3 coord & 3 * 3 color & 3 * 3 normal

    template <typename T = double>
    struct vertex_coords_t final {
        T x_, y_, z_;
        vertex_coords_t(T x, T y, T z) : x_(x), y_(y), z_(z) {}
    };

    template <typename T = double>
    struct vertex_color_t final {
        T x_, y_, z_;
        vertex_color_t() {}
        vertex_color_t(T x, T y, T z) : x_(x), y_(y), z_(z) {}
        void set(T x, T y, T z) {
            x_ = x;
            y_ = y;
            z_ = z;
        }
    };

    struct vertices_t final {
        int count_ = 0;
        std::unique_ptr<GLfloat[]> vertices_;

    public:
        vertices_t(int count) : count_(count) {
            vertices_ = std::make_unique<GLfloat[]>(count);
        }

        template <typename T>
        void set_vertex(int v_ind, vertex_coords_t<T>& coords,
                        vertex_color_t<T>& color, vertex_coords_t<T>& normal) {
            vertices_[v_ind + 0] = coords.x_;
            vertices_[v_ind + 1] = coords.y_;
            vertices_[v_ind + 2] = coords.z_;

            vertices_[v_ind + 3] = color.x_;
            vertices_[v_ind + 4] = color.y_;
            vertices_[v_ind + 5] = color.z_;

            vertices_[v_ind + 6] = normal.x_;
            vertices_[v_ind + 7] = normal.y_;
            vertices_[v_ind + 8] = normal.z_;
        }
    };
}
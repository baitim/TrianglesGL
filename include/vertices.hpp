#pragma once

#include "triangle.hpp"
#include <GL/glew.h>
#include <memory>

namespace vertices {

    enum class triangle_type_e {
        TRIANGLE_TYPE_NOT_INTERSECT,
        TRIANGLE_TYPE_INTERSECT
    };

    const int TRIANGLE2VERTEX_SIZE = 18; // 9 coord & 9 color

    template <typename T = double>
    struct vertex_coords_t final {
        T x_, y_, z_;

        vertex_coords_t(T x, T y, T z) : x_(x), y_(y), z_(z) {}
    };

    struct vertices_t final {
        int count_ = 0;
        std::unique_ptr<GLfloat[]> vertices_;

    public:
        vertices_t(int count) : count_(count) {
            vertices_ = std::make_unique<GLfloat[]>(count);
        }

        template <typename T>
        void set_vertex(int v_ind, triangle_type_e type, const vertex_coords_t<T>& coords) {
            vertices_[v_ind + 0] = coords.x_;
            vertices_[v_ind + 1] = coords.y_;
            vertices_[v_ind + 2] = coords.z_;

            vertices_[v_ind + 3] = 0.0f;
            vertices_[v_ind + 4] = 0.0f;
            vertices_[v_ind + 5] = 0.0f;

            if (type == triangle_type_e::TRIANGLE_TYPE_INTERSECT)
                vertices_[v_ind + 3] = 1.0f;
            else if (type == triangle_type_e::TRIANGLE_TYPE_NOT_INTERSECT)
                vertices_[v_ind + 5] = 1.0f;
        }
    };
}
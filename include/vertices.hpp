#pragma once

#include <GL/glew.h>
#include <memory>

namespace vertices {

    const int VERTEX_SIZE          = 3;
    const int COLOR_SIZE           = 3;
    const int TRIANGLE2VERTEX_CNT  = 3;
    const int TRIANGLE2COLORS_CNT  = 3;
    const int TRIANGLE2VERTEX_SIZE =   VERTEX_SIZE * TRIANGLE2VERTEX_CNT
                                     + COLOR_SIZE  * TRIANGLE2COLORS_CNT;
    const int TRIANGLE2NORMAL_SIZE = 3;

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
        void set_vertex(int ind, vertex_coords_t<T>& coords, vertex_color_t<T>& color) {
            vertices_[ind + 0] = coords.x_;
            vertices_[ind + 1] = coords.y_;
            vertices_[ind + 2] = coords.z_;

            vertices_[ind + 3] = color.x_;
            vertices_[ind + 4] = color.y_;
            vertices_[ind + 5] = color.z_;
        }
    };

    struct normals_t final {
        int count_ = 0;
        std::unique_ptr<GLfloat[]> normals_;

    public:
        normals_t(int count) : count_(count) {
            normals_ = std::make_unique<GLfloat[]>(count);
        }

        template <typename T>
        void set_normal(int ind, vertex_coords_t<T>& coords) {
            normals_[ind + 0] = coords.x_;
            normals_[ind + 1] = coords.y_;
            normals_[ind + 2] = coords.z_;
        }
    };
}
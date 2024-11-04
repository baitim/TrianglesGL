#pragma once

#include <GL/glew.h>
#include <memory>

namespace vertices {

    const int VERTEX_SIZE          = 3;
    const int NORMAL_SIZE          = 3;
    const int COLOR_SIZE           = 1;
    const int TRIANGLE2VERTEX_CNT  = 3;
    const int TRIANGLE2NORMAL_CNT  = 3;
    const int TRIANGLE2COLORS_CNT  = 3;
    const int TRIANGLE2VERTEX_SIZE =   VERTEX_SIZE * TRIANGLE2VERTEX_CNT
                                     + NORMAL_SIZE * TRIANGLE2NORMAL_CNT
                                     + COLOR_SIZE  * TRIANGLE2COLORS_CNT;

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
        void set_vertex(int ind, T color,
                        const vertex_coords_t<T>& coords,
                        const vertex_coords_t<T>& normal) {
            vertices_[ind + 0] = coords.x_;
            vertices_[ind + 1] = coords.y_;
            vertices_[ind + 2] = coords.z_;

            vertices_[ind + 3] = normal.x_;
            vertices_[ind + 4] = normal.y_;
            vertices_[ind + 5] = normal.z_;

            vertices_[ind + 6] = color;
        }
    };
}
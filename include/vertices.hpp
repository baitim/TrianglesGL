#pragma once

#include <GL/glew.h>
#include <memory>

namespace vertices {

    template <typename T = double>
    struct vertex_coords_t final {
        T x_, y_, z_;
        vertex_coords_t(T x, T y, T z) : x_(x), y_(y), z_(z) {}
    };

    struct vertex2render_t {
        GLfloat coord [3];
        GLfloat normal[3];
        GLbyte  color;
        GLbyte  is_dark;
    };

    struct vertices_t final {
        int count_ = 0;
        std::unique_ptr<vertex2render_t[]> vertices_;

    public:
        vertices_t(int count) : count_(count) {
            vertices_ = std::make_unique<vertex2render_t[]>(count);
        }

        template <typename T>
        void set_vertex(int ind, char color, char is_dark,
                        const vertex_coords_t<T>& coords,
                        const vertex_coords_t<T>& normal) {
            vertex2render_t& vertex = vertices_[ind];
            vertex.coord[0] = coords.x_;
            vertex.coord[1] = coords.y_;
            vertex.coord[2] = coords.z_;

            vertex.normal[0] = normal.x_;
            vertex.normal[1] = normal.y_;
            vertex.normal[2] = normal.z_;

            vertex.color   = color;
            vertex.is_dark = is_dark;
        }
    };
}
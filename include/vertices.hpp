#pragma once

#include <GL/glew.h>
#include <vector>

namespace vertices {

    struct vertex2render_t {
        point::point_t<GLfloat> coord;
        point::point_t<GLfloat> normal;
        GLbyte color;
        GLbyte is_dark;
    };

    struct vertices_t final {
        int count_ = 0;
        std::vector<vertex2render_t> vertices_;

    public:
        vertices_t(int count) : count_(count), vertices_(count) {}

        template <typename T>
        void set_vertex(int ind, char color, char is_dark,
                        const point::point_t<T>& coord,
                        const point::point_t<T>& normal) {
            vertex2render_t& vertex = vertices_[ind];
            vertex.coord   = coord;
            vertex.normal  = normal;
            vertex.color   = color;
            vertex.is_dark = is_dark;
        }
    };
}
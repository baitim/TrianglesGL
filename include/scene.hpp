#pragma once

#include "octree.hpp"
#include "vertices.hpp"

namespace scene {

    template <typename T = double>
    struct scene_t final {
        int count_ = 0;
        std::vector<triangle::triangle_t<T>> triangles_;
        std::vector<vertices::triangle_type_e> triangles_types_;

    public:
        void set_types() {
            octree::octree_t<T> octree(count_, triangles_.begin(), triangles_.end());
            std::set<int> ans = octree.get_set_intersecting_triangles();
            for (auto it : ans)
                triangles_types_[it] = vertices::triangle_type_e::TRIANGLE_TYPE_INTERSECT;
        }

        vertices::vertices_t get_vertices() {
            vertices::vertices_t vertices{count_ * vertices::TRIANGLE2VERTEX_SIZE};
            int vertex_index_shift = vertices::TRIANGLE2VERTEX_SIZE / 3;
            
            for (int i = 0, v_index = 0; i < count_; ++i, v_index += vertices::TRIANGLE2VERTEX_SIZE) {
                vertices::vertex_coords_t<T> a_coords{triangles_[i].a_.x_, triangles_[i].a_.y_, triangles_[i].a_.z_};
                vertices::vertex_coords_t<T> b_coords{triangles_[i].b_.x_, triangles_[i].b_.y_, triangles_[i].b_.z_};
                vertices::vertex_coords_t<T> c_coords{triangles_[i].c_.x_, triangles_[i].c_.y_, triangles_[i].c_.z_};

                vertices.set_vertex(v_index + 0 * vertex_index_shift, triangles_types_[i], a_coords);
                vertices.set_vertex(v_index + 1 * vertex_index_shift, triangles_types_[i], b_coords);
                vertices.set_vertex(v_index + 2 * vertex_index_shift, triangles_types_[i], c_coords);
            }
            return vertices;
        }
    };

    template <typename T>
    std::istream& operator>>(std::istream& is, scene_t<T>& sc) {
        is >> sc.count_;
        sc.triangles_.resize(sc.count_);
        for (int i = 0; i < sc.count_; ++i)
            is >> sc.triangles_[i];

        sc.triangles_types_.resize(sc.count_, vertices::triangle_type_e::TRIANGLE_TYPE_NOT_INTERSECT);
        sc.set_types();

        return is;
    }
}
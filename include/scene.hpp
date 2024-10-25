#pragma once

#include "octree.hpp"
#include "vertices.hpp"
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>

namespace scene {

    enum class triangle_type_e {
        TRIANGLE_TYPE_NOT_INTERSECT,
        TRIANGLE_TYPE_INTERSECT
    };

    template <typename T = double>
    struct scene_t final {
        int count_ = 0;
        T shift_shadows_ = 0.005;
        std::vector<triangle::triangle_t<T>> triangles_;
        std::vector<triangle_type_e> triangles_types_;

    public:
        void set_types() {
            octree::octree_t<T> octree(count_, triangles_.begin(), triangles_.end());
            std::set<int> ans = octree.get_set_intersecting_triangles();
            for (auto it : ans)
                triangles_types_[it] = triangle_type_e::TRIANGLE_TYPE_INTERSECT;
        }

        vertices::vertices_t get_vertices() const {
            vertices::vertices_t vertices{count_ * vertices::TRIANGLE2VERTEX_SIZE};
            int vertex_index_shift = vertices::TRIANGLE2VERTEX_SIZE / 3;
            
            for (int i = 0, v_index = 0; i < count_; ++i, v_index += vertices::TRIANGLE2VERTEX_SIZE) {

                vertices::vertex_color_t<T> color;
                if (triangles_types_[i] == triangle_type_e::TRIANGLE_TYPE_INTERSECT)
                    color.set(1.f, 0.f, 0.f);
                else if (triangles_types_[i] == triangle_type_e::TRIANGLE_TYPE_NOT_INTERSECT)
                    color.set(0.f, 0.f, 1.f);

                vertices::vertex_coords_t<T> a_coords{triangles_[i].a_.x_, triangles_[i].a_.y_, triangles_[i].a_.z_};
                vertices::vertex_coords_t<T> b_coords{triangles_[i].b_.x_, triangles_[i].b_.y_, triangles_[i].b_.z_};
                vertices::vertex_coords_t<T> c_coords{triangles_[i].c_.x_, triangles_[i].c_.y_, triangles_[i].c_.z_};

                point::point_t<T> normal_ = triangles_[i].normal().norm();
                vertices::vertex_coords_t<T> normal{normal_.x_, normal_.y_, normal_.z_};

                vertices.set_vertex(v_index + 0 * vertex_index_shift, a_coords, color, normal);
                vertices.set_vertex(v_index + 1 * vertex_index_shift, b_coords, color, normal);
                vertices.set_vertex(v_index + 2 * vertex_index_shift, c_coords, color, normal);
            }
            return vertices;
        }
    };

    template <typename T>
    std::istream& operator>>(std::istream& is, scene_t<T>& sc) {
        int count;
        is >> count;
        if (!is.good())
            throw "invalid count of scenes";
        if (count <= 0)
            throw "le count of scenes";

        sc.count_ = count;
        sc.triangles_.resize(count);
        sc.triangles_types_.resize(count, triangle_type_e::TRIANGLE_TYPE_NOT_INTERSECT);
        for (int i = 0; i < count; ++i)
            is >> sc.triangles_[i];
        sc.set_types();

        return is;
    }

    std::vector<std::string> get_sorted_files(const std::filesystem::path& path) {
        std::vector<std::string> files;

        for (const auto& entry : std::filesystem::directory_iterator(path))
            files.push_back(entry.path().string());

        std::sort(files.begin(), files.end());
        return files;
    }

    std::vector<scene::scene_t<float>> get_scenes(const std::filesystem::path& relative_path) {
        std::string file{__FILE__};
        std::filesystem::path dir = file.substr(0, file.rfind("/"));
        std::filesystem::path scenes_path = dir / relative_path;

        std::vector<std::string> scenes_data = get_sorted_files(scenes_path);
        std::vector<scene::scene_t<float>> scenes(scenes_data.size());
        for (int i = 0, end = scenes_data.size(); i < end; ++i) {
            std::ifstream data(scenes_data[i]);
            data >> scenes[i];
            data.close();
        }
        return scenes;
    }
}
#pragma once

#include "octree.hpp"
#include "vertices.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>

namespace scene {

    int COUNT_VERTEXES_IN_TRIANGLE = 3;

    enum class triangle_type_e {
        TRIANGLE_TYPE_NOT_INTERSECT,
        TRIANGLE_TYPE_INTERSECT
    };

    struct light_t final {
        int width_  = 2048;
        int height_ = 2048;
        glm::mat4 depth_MVP_;
        glm::vec3 light_direction_{-1, -1, -1};
        glm::vec3 light_position_ { 2,  2,  2};
        glm::vec3 light_up_       { 0,  1,  0};
        glm::mat4 depth_projection_matrix_ = glm::ortho<float>(-1.4, 1.4, -1.4, 1.4, 0.1, 5);
    };

    struct data2render_t final {
        vertices::vertices_t vertices_;
        light_t light_;
        data2render_t(int count) : vertices_(count * COUNT_VERTEXES_IN_TRIANGLE) {}
    };

    template <typename T = double>
    struct scene_t final {
        int count_ = 0;
        std::vector<triangle::triangle_t<T>> triangles_;
        std::vector<triangle_type_e> triangles_types_;

        char check_is_dark_side(const glm::vec3& light_dir, const triangle::triangle_t<T>& t) const {
            point::point_t<T> normal_by_cross = point::cross_product(t.b_ - t.a_, t.c_ - t.a_);
            point::point_t<T> light_direction = {light_dir[0], light_dir[1], light_dir[2]};
            return point::dot(light_direction, normal_by_cross) < 0;
        }

    public:
        void set_types() {
            octree::octree_t<T> octree(count_, triangles_.begin(), triangles_.end());
            std::set<int> ans = octree.get_set_intersecting_triangles();
            for (auto it : ans)
                triangles_types_[it] = triangle_type_e::TRIANGLE_TYPE_INTERSECT;
        }

        data2render_t get_data() const {
            data2render_t data{count_};
            auto& vertices = data.vertices_;
        
            for (int i = 0, v_index = 0; i < count_; ++i, v_index += COUNT_VERTEXES_IN_TRIANGLE) {
                auto& triangle = triangles_[i];

                char color = 0;
                if (triangles_types_[i] == triangle_type_e::TRIANGLE_TYPE_INTERSECT)
                    color = 1;

                char is_dark = check_is_dark_side(data.light_.light_direction_, triangle);

                point::point_t normal = triangle.normal().norm();

                vertices.set_vertex(v_index + 0, color, is_dark, triangle.a_, normal);
                vertices.set_vertex(v_index + 1, color, is_dark, triangle.b_, normal);
                vertices.set_vertex(v_index + 2, color, is_dark, triangle.c_, normal);
            }

            return data;
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

        count *= 2;
        sc.count_ = count;
        sc.triangles_.resize(count);
        sc.triangles_types_.resize(count, triangle_type_e::TRIANGLE_TYPE_NOT_INTERSECT);

        int count_half = count / 2;
        for (int i = 0; i < count_half; ++i) {
            int shift_i = count_half + i;
            sc.triangles_      [shift_i] = sc.triangles_      [i];
            sc.triangles_types_[shift_i] = sc.triangles_types_[i];
            std::swap(sc.triangles_[shift_i].b_, sc.triangles_[shift_i].c_);
        }

        return is;
    }

    std::vector<std::string> get_sorted_files(const std::filesystem::path& path) {
        std::vector<std::string> files;

        for (const auto& entry : std::filesystem::directory_iterator(path))
            files.push_back(entry.path().string());

        std::sort(files.begin(), files.end());
        return files;
    }

    std::vector<scene::scene_t<float>> get_scenes_default(const std::filesystem::path& scenes_path) {
        std::vector<std::string> scenes_data = get_sorted_files(scenes_path);
        std::vector<scene::scene_t<float>> scenes(scenes_data.size());
        for (int i = 0, end = scenes_data.size(); i < end; ++i) {
            std::ifstream data(scenes_data[i]);
            data >> scenes[i];
            data.close();
        }
        return scenes;
    }

    std::vector<scene::scene_t<float>> get_scenes_cmd(const std::string& file) {
        std::vector<scene::scene_t<float>> scenes(1);
        std::ifstream data(file);
        data >> scenes[0];
        data.close();
        return scenes;
    }

    std::vector<scene::scene_t<float>> get_scenes(int argc, char* argv[],
                                                  const std::filesystem::path& scenes_path) {
        std::vector<scene::scene_t<float>> scenes;
        if (argc == 2) {
            scenes = scene::get_scenes_cmd(argv[1]);
        } else {
            scenes = scene::get_scenes_default(scenes_path);
        }
        return scenes;
    }
}
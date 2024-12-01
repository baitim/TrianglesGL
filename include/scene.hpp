#pragma once

#include "octree.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>
#include <vector>

namespace scene {

    class error_t : public std::runtime_error {
    public:
        error_t(const char*        msg) : std::runtime_error(msg) {}
        error_t(const std::string& msg) : std::runtime_error(msg) {}
    };

    enum class triangle_type_e {
        TRIANGLE_TYPE_NOT_INTERSECT,
        TRIANGLE_TYPE_INTERSECT
    };

    struct light_t final {
        int width  = 2048;
        int height = 2048;
        glm::mat4 depth_MVP;
        glm::vec3 light_direction{-1, -1, -1};
        glm::vec3 light_position { 2,  2,  2};
        glm::vec3 light_up       { 0,  1,  0};
        glm::mat4 depth_projection_matrix = glm::ortho<float>(-1.4, 1.4, -1.4, 1.4, 0.1, 5);
    };

    inline const int COUNT_VERTEXES_IN_TRIANGLE = 3;

    struct vertex2render_t final {
        point::point_t<GLfloat> coord;
        point::point_t<GLfloat> normal;
        GLbyte color;
    };

    struct data2render_t final {
        light_t light_;
        std::vector<vertex2render_t> vertices_;

    public:
        data2render_t(int count) : vertices_(count * COUNT_VERTEXES_IN_TRIANGLE) {}

        template <typename T>
        void set_vertex(int ind, char color,
                        const point::point_t<T>& coord,
                        const point::point_t<T>& normal) noexcept {
            vertex2render_t& vertex = vertices_[ind];
            vertex.coord   = coord;
            vertex.normal  = normal;
            vertex.color   = color;
        }
    };

    template <typename T = double>
    struct scene_t final {
        int count_ = 0;
        std::vector<triangle::triangle_t<T>> triangles_;
        std::vector<triangle_type_e> triangles_types_;

        void set_ccw2light(const glm::vec3& light_dir, triangle::triangle_t<T>& t) const {
            point::point_t<T> normal_by_cross = point::cross_product(t.b_ - t.a_, t.c_ - t.a_);
            point::point_t<T> light_direction = {light_dir[0], light_dir[1], light_dir[2]};
            if (real_numbers::is_real_gt(point::dot(light_direction, normal_by_cross),
                                         static_cast<float>(0)))
                std::swap(t.b_, t.c_);
        }

        void preset() {
            octree::octree_t<T> octree(count_, triangles_.begin(), triangles_.end());
            std::set<int> ans = octree.get_set_intersecting_triangles();
            for (auto it : ans)
                triangles_types_[it] = triangle_type_e::TRIANGLE_TYPE_INTERSECT;

            glm::vec3 light_dir = light_t{}.light_direction;
            for (int i = 0; i < count_; ++i)
                set_ccw2light(light_dir, triangles_[i]);
        }

    public:
        data2render_t get_data() const {
            data2render_t data{count_};
        
            for (int i = 0, v_index = 0; i < count_; ++i, v_index += COUNT_VERTEXES_IN_TRIANGLE) {
                auto& triangle = triangles_[i];

                char color = 0;
                if (triangles_types_[i] == triangle_type_e::TRIANGLE_TYPE_INTERSECT)
                    color = 1;

                point::point_t normal = triangle.normal().norm();

                data.set_vertex(v_index + 0, color, triangle.a_, normal);
                data.set_vertex(v_index + 1, color, triangle.b_, normal);
                data.set_vertex(v_index + 2, color, triangle.c_, normal);
            }

            return data;
        }
    };

    template <typename T>
    inline std::istream& operator>>(std::istream& is, scene_t<T>& sc) {
        int count;
        is >> count;
        if (!is.good())
            throw error_t{"invalid count of scenes"};
        if (count <= 0)
            throw error_t{"count of scenes <= 0"};

        sc.count_ = count;
        sc.triangles_.resize(count);
        sc.triangles_types_.resize(count, triangle_type_e::TRIANGLE_TYPE_NOT_INTERSECT);
        for (int i = 0; i < count; ++i) {
            is >> sc.triangles_[i];
            if (!is.good())
                throw error_t{"triangle was entered incorrectly"};
        }
        sc.preset();

        return is;
    }

    inline std::vector<std::string> get_sorted_files(const std::filesystem::path& path) {
        std::vector<std::string> files;

        for (const auto& entry : std::filesystem::directory_iterator(path))
            files.push_back(entry.path().string());

        std::sort(files.begin(), files.end());
        return files;
    }

    inline std::vector<scene::scene_t<float>> get_scenes_default(const std::filesystem::path& scenes_path) {
        std::vector<std::string> scenes_data = get_sorted_files(scenes_path);
        std::vector<scene::scene_t<float>> scenes(scenes_data.size());
        for (int i = 0, end = scenes_data.size(); i < end; ++i) {
            std::ifstream data(scenes_data[i]);
            data >> scenes[i];
            data.close();
        }
        return scenes;
    }

    inline std::vector<scene::scene_t<float>> get_scenes_cmd(const std::string& file) {
        std::vector<scene::scene_t<float>> scenes(1);
        std::ifstream data(file);
        data >> scenes[0];
        data.close();
        return scenes;
    }

    inline std::vector<scene::scene_t<float>> get_scenes(int argc, char* argv[],
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
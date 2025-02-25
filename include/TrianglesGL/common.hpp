#pragma once

#include "Triangles3D/ANSI_colors.hpp"
#include <sstream>
#include <fstream>

namespace triangles_gl {

    template <typename MsgT>
    concept error_str =
    std::is_constructible_v<std::string, MsgT> &&
    requires(std::ostream& os, MsgT msg) {
        { os << msg } -> std::same_as<std::ostream&>;
    };

    class error_t : public std::runtime_error {
    public:
        template <error_str MsgT>
        error_t(MsgT msg) : std::runtime_error(msg) {}
    };

    inline std::string file2str(std::string_view file_name) {
        std::ifstream input_file{std::string(file_name)};
        if (input_file.is_open()) {
            std::stringstream sstr;
            sstr << input_file.rdbuf();
            input_file.close();
            return sstr.str();
        }
        throw error_t{"Can't open file"};
    }
}
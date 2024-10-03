#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

std::string get_shader_code(const char* file_path) {
    std::string shader_code;
    std::ifstream shader_stream(file_path, std::ios::in);
    if (shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << shader_stream.rdbuf();
        shader_code = sstr.str();
        shader_stream.close();
    }
    return shader_code;
}

GLuint install_shader(const std::string& shader_code, GLenum shader_type)  {
    GLuint shader_id = glCreateShader(shader_type);
    char const* code = shader_code.c_str();
    glShaderSource(shader_id, 1, &code , NULL);
    glCompileShader(shader_id);

    GLint result = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    if (!result)
        std::cerr << "Error in install shader\n";

    return shader_id;
}

GLuint create_shaders_program(GLuint vertex_shader_id, GLuint fragment_shader_id)  {
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    GLint result = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    if (!result)
        std::cerr << "Error in create shaders program\n";

    return program_id;
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path){
    std::string vertex_shader_code   = get_shader_code(vertex_file_path);
    std::string fragment_shader_code = get_shader_code(fragment_file_path);

    GLuint vertex_shader_id   = install_shader(vertex_shader_code, GL_VERTEX_SHADER);
    GLuint fragment_shader_id = install_shader(fragment_shader_code, GL_FRAGMENT_SHADER);

    GLuint program_id = create_shaders_program(vertex_shader_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

int main() {
    sf::Window window(sf::VideoMode(800, 600), "Triangle");

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Невозможно инициализировать GLEW\n";
        return -1;
    }

    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint program_id = LoadShaders("src/vertexshader", "src/fragmentshader");
    glUseProgram(program_id);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        window.display();
    }

    return 0;
}
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

    // Создаем шейдеры
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Загружаем код Вершинного Шейдера из файла
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        std::cerr << VertexShaderCode << "\n";
        VertexShaderStream.close();
    }

    // Загружаем код Фрагментного шейдера из файла
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        std::cerr << FragmentShaderCode << "\n";
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Компилируем Вершинный шейдер
    printf("Компиляция шейдера: %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Выполняем проверку Вершинного шейдера
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
      std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
      glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    }

    // Компилируем Фрагментный шейдер
    printf("Компиляция шейдера: %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Проверяем Фрагментный шейдер
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
      std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
      glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    }

    // Создаем шейдерную программу и привязываем шейдеры к ней
    fprintf(stdout, "Создаем шейдерную программу и привязываем шейдеры к ней\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Проверяем шейдерную программу
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
      std::vector<char> ProgramErrorMessage(InfoLogLength+1);
      glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
      fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

int main() {
    sf::Window window(sf::VideoMode(800, 600), "My window");

    glewExperimental=true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Невозможно инициализировать GLEW\n";
        return -1;
    
    }

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glVertexAttribPointer(posAttrID, 2, GL_FLOAT, GL_FALSE, 0, vertices); // 0);
    glEnableVertexAttribArray(posAttrID);

    GLuint programID = LoadShaders("src/vertexshader", "src/fragmentshader");
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glUseProgram(programID);
    GLint posAttrID = glGetAttribLocation(programID, "position");

    glVertexAttribPointer(posAttrID, 2, GL_FLOAT, GL_FALSE, 0, vertices); // 0);
    glEnableVertexAttribArray(posAttrID);

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
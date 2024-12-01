cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gl
    ${CMAKE_CURRENT_SOURCE_DIR}/../Triangles3D/include/
)

find_package(opengl_system REQUIRED)
find_package(FreeGLUT      REQUIRED)
find_package(GLEW          REQUIRED)
find_package(glm           REQUIRED)
find_package(SFML          REQUIRED COMPONENTS window)

add_executable(scene scene.cpp)
target_include_directories(scene PUBLIC ${INCLUDE_DIR})
target_link_libraries(scene opengl::opengl FreeGLUT::freeglut_static GLEW::GLEW glm::glm sfml-window)
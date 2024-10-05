cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gl
    ${CMAKE_CURRENT_SOURCE_DIR}/../Triangles3D/include/
)

find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)
find_package(SFML  COMPONENTS graphics REQUIRED)

add_executable(scene scene.cpp)
target_include_directories(scene PUBLIC ${INCLUDE_DIR})

target_link_libraries(scene OpenGL::GL GLEW)
target_link_libraries(scene sfml-graphics)
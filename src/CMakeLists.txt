cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/gl ${CMAKE_SOURCE_DIR}/include/window)

find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)
find_package(SFML COMPONENTS graphics REQUIRED)

add_executable(scene scene.cpp)
target_include_directories(scene PUBLIC ${INCLUDE_DIR})

target_link_libraries(scene OpenGL::GL OpenGL::GL GLEW)
target_link_libraries(scene sfml-graphics)
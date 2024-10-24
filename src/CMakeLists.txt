cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gl
    ${CMAKE_CURRENT_SOURCE_DIR}/../Triangles3D/include/
)

find_package(OpenGL   REQUIRED)
find_package(GLEW     REQUIRED)
find_package(FreeGLUT REQUIRED)
find_package(SFML     REQUIRED COMPONENTS graphics window)

add_executable(scene scene.cpp)
target_include_directories(scene PUBLIC ${INCLUDE_DIR})
target_link_libraries(scene OpenGL::GL GLEW ${FreeGLUT_LIBRARIES} ${SFML_LIBRARIES})

set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../scenes/generate.py")
add_custom_target(
    generate_scenes_target ALL
    COMMAND python3 ${PYTHON_SCRIPT}
    DEPENDS scene
)
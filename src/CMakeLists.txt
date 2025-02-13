cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(opengl_system REQUIRED)
find_package(FreeGLUT      REQUIRED)
find_package(glew          REQUIRED)
find_package(glm           REQUIRED)
find_package(SFML          REQUIRED COMPONENTS window)

find_package(Triangles3D   REQUIRED)

add_executable(scene scene.cpp)
target_sources(scene
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)
target_link_libraries(scene PRIVATE
    opengl::opengl
    FreeGLUT::freeglut_static
    GLEW::GLEW
    glm::glm
    sfml-window
    Triangles3D::Triangles3D
)

add_library(TrianglesGL scene.cpp)
target_include_directories(TrianglesGL PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(TrianglesGL PRIVATE
    opengl::opengl
    FreeGLUT::freeglut_static
    GLEW::GLEW
    glm::glm
    sfml-window
    Triangles3D::Triangles3D
)

install(TARGETS TrianglesGL EXPORT TrianglesGLTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${INCLUDE_DIR}/TrianglesGL DESTINATION include)

install(EXPORT TrianglesGLTargets
    FILE TrianglesGLConfig.cmake
    DESTINATION lib/cmake/TrianglesGL
)
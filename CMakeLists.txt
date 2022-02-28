cmake_minimum_required(VERSION 3.20)
project(OpenGLTemplate)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external)

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
file(GLOB_RECURSE SRC_HEADERS src/*.h)
set(HEADER_FILES pch.h)

add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${SRC_HEADERS} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        external
        ${OPENAL_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        # static link
        glfw
        glm
        glad
        stb

        # dynamic link
        ${OPENAL_LIBRARY}
        OpenGL::GL
        Freetype::Freetype
        assimp::assimp
        )

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})
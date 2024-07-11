cmake_minimum_required(VERSION 3.28)
project(TucanLightmapper)

set(CMAKE_CXX_STANDARD 23)

add_executable(TucanLightmapper
        main.cpp
        TucanGL.h
        Shader.h
        Scene.cpp
        Scene.h
        Texture.h
        Mesh.h
        Triangle.h
        Vertex.h
        Display.cpp
        Display.h
        Camera.h
        ThirdParty/lodepng.cpp
        ThirdParty/lodepng.h
)

include_directories(ThirdParty/glm)

target_link_libraries(TucanLightmapper opengl32)
target_link_libraries(TucanLightmapper -static)
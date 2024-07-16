#include <fstream>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "Display.h"
#include "Scene.h"
#include "Shader.h"

#define TITLE                             "Lightmapper Demo"
#define WIDTH                             800
#define HEIGHT                            600

#define SHADER_VERTEX_ATTRIB_TITLE        "origin"
#define SHADER_TEXTURE_COORD_ATTRIB_TITLE "uv"
#define VIEW_MATRIX_TITLE                 "view_mat"
#define PROJ_MATRIX_TITLE                 "proj_mat"
#define ALBEDO_MAP_TITLE                  "main_tex"
#define LIGHT_MAP_TITLE                   "lightmap_tex"

#define RESOURCES_FOLDER                  "resources\\"
#define VERTEX_SHADER_FILEPATH            RESOURCES_FOLDER "vert.glsl"
#define FRAGMENT_SHADER_FILEPATH          RESOURCES_FOLDER "frag.glsl"
#define MESH_FILENAME                     RESOURCES_FOLDER "mesh.bin"
#define ALBEDO_TEXTURE_FILENAME           RESOURCES_FOLDER "albedo.png"

#define GL_ENABLE_FLAGS                   glEnable(GL_DEPTH_TEST);                            \
                                          glEnable(GL_CULL_FACE);                             \
                                          glEnable(GL_TEXTURE_2D);                            \
                                          glCullFace(GL_BACK)

#define GL_CLEAN_UP                       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); \
                                          glClearColor(1.0F, 1.0F, 1.0F, 1.0F)

#define SAMPLES_NUM                       128

#define CAMERA_OFFSET                     5.0F
#define CAMERA_FOV                        45.0F

#define LIGHT_DIRECTION                   0.0F, -2.0F, -1.0F

std::string read_ascii(const std::string &file_name) {
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_name << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#pragma region [READ_MESH_BINARY]
template<typename T>
static T read_data(const char *buffer, int32_t &ptr) {
    T                data;
    constexpr size_t size = sizeof(T);
    std::memcpy(&data, &buffer[ptr], size);
    ptr += size;
    return data;
}

std::unique_ptr<Mesh> load_mesh(const std::string &file_name) {
    int32_t       ptr = 0;
    std::ifstream input_stream(file_name, std::ios::binary);

    if (!input_stream) {
        std::cerr << "Error opening file: " << file_name << std::endl;
        return nullptr;
    }

    input_stream.seekg(0, std::ios::end);
    const long long size = input_stream.tellg();
    input_stream.seekg(0, std::ios::beg);

    const auto buffer = new char[size];
    input_stream.read(buffer, static_cast<int32_t>(size));
    input_stream.close();

    const auto vertex_count = read_data<int32_t>(buffer, ptr);
    const auto index_count  = read_data<int32_t>(buffer, ptr);

    std::vector<float>    vertices;
    std::vector<float>    tex_coords;
    std::vector<uint32_t> indices;

    for (int32_t i = 0; i < vertex_count * 3; ++i) vertices.push_back(read_data<float>(buffer, ptr));
    for (int32_t i = 0; i < vertex_count * 2; ++i) tex_coords.push_back(read_data<float>(buffer, ptr));
    for (int32_t i = 0; i < index_count; ++i) indices.push_back(read_data<uint32_t>(buffer, ptr));

    auto mesh = std::make_unique<Mesh>();
    mesh->set_vertices(vertices);
    mesh->set_tex_coords(tex_coords);
    mesh->set_indices(indices);

    delete[] buffer;
    return mesh;
}
#pragma endregion

int main() {
    const auto display = new Display(TITLE, WIDTH, HEIGHT, 3, 3);
    GL_ENABLE_FLAGS;

    const ShaderAttrib shader_vertex_attrib{
        .location = GL_VERTEX_ATTRIB_ARRAY,
        .name = SHADER_VERTEX_ATTRIB_TITLE,
    };

    const ShaderAttrib shader_uv_attrib{
        .location = GL_TEXTURE_COORD_ATTRIB_ARRAY,
        .name = SHADER_TEXTURE_COORD_ATTRIB_TITLE
    };

    const auto shader = new Shader(read_ascii(VERTEX_SHADER_FILEPATH),
                                   read_ascii(FRAGMENT_SHADER_FILEPATH),
                                   shader_vertex_attrib,
                                   shader_uv_attrib);

    const auto  mesh            = load_mesh(MESH_FILENAME);
    const auto &mesh_bounds_min = mesh->min;
    const auto &mesh_bounds_max = mesh->max;

    const auto cam = new Camera(glm::radians(CAMERA_FOV), static_cast<float>(WIDTH) / HEIGHT);
    cam->location  = {0.0F, (mesh_bounds_min.y + mesh_bounds_max.y) * 0.5F, mesh_bounds_max.z + CAMERA_OFFSET};

    const auto    scene             = new Scene(mesh.get(), SAMPLES_NUM);
    const int32_t view_mat_location = shader->get_uniform_location(VIEW_MATRIX_TITLE);
    const int32_t proj_mat_location = shader->get_uniform_location(PROJ_MATRIX_TITLE);

    const int32_t albedo_location = shader->get_uniform_location(ALBEDO_MAP_TITLE);
    const int32_t lightmap_location     = shader->get_uniform_location(LIGHT_MAP_TITLE);

    scene->load_albedo_from_file(ALBEDO_TEXTURE_FILENAME);
    scene->bake_dir_light(glm::vec3 {LIGHT_DIRECTION});

    Texture::active(0);
    scene->albedo_texture.bind();

    Texture::active(1);
    scene->lightmap_texture.bind();

    while (display->running) {
        GL_CLEAN_UP;

        float delta_time;
        display->begin(delta_time);
        shader->use();

        scene->bake_step();

        glUniform1i(albedo_location, 0);
        glUniform1i(lightmap_location, 1);

        Shader::mat4(view_mat_location, cam->view());
        Shader::mat4(proj_mat_location, cam->projection);

        mesh->draw();
        display->flush();
    }

    delete cam;
    delete scene;
    delete shader;
    delete display;

    return 0;
}

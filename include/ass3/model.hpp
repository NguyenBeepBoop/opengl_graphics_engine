#ifndef COMP3421_MODEL_HPP
#define COMP3421_MODEL_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "ass3/mesh.hpp"

namespace model {
    struct material_t {
        GLuint diffuse_map = 0;
        GLuint specular_map = 0;
        GLuint cube_map = 0;
        GLuint normal_map = 0;
        GLuint height_map = 0;
        GLuint ambient_map = 0;
        GLuint roughness_map = 0;
        GLuint reflection_map = 0;
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec4 diffuse = glm::vec4(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        float phong_exp = 5.0f;
        float cube_map_factor = 1.0f;
        float reflection_map_factor = 1.0f;
    };

    struct model_t {
        std::vector<mesh::mesh_t> meshes;
        std::vector<material_t> materials;
    };

    model_t load(const std::string &path);

    void destroy(const model_t &model);
} // namespace model

#endif // COMP3421_MODEL_HPP

#include "ass3/model.hpp"
#include "ass3/texture_2d.hpp"

#include <tiny_obj_loader.h>
#include <chicken3421/chicken3421.hpp>

namespace model {
	model_t load(const std::string& path) {
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config{};
		config.triangulate = true;
		config.mtl_search_path = path.substr(0, path.find_last_of('/') + 1);

		bool did_load = reader.ParseFromFile(path, config);
		chicken3421::expect(did_load && reader.Error().empty() && reader.Warning().empty(),
		                    reader.Error() + reader.Warning());

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();
		auto model = model_t{};

		std::vector<material_t> mats;
		// initialise the materials
		for (const auto& m : materials) {
			auto mat = material_t{};
			mat.diffuse = glm::vec4{m.diffuse[0], m.diffuse[1], m.diffuse[2], 1.0f};
			mat.diffuse_map = m.diffuse_texname.empty()
			                     ? 0
			                     : texture_2d::init(config.mtl_search_path + m.diffuse_texname);
			mat.specular = glm::vec3{m.specular[0], m.specular[1], m.specular[2]};
			mat.specular_map = m.specular_texname.empty()
			                      ? 0
			                      : texture_2d::init(config.mtl_search_path + m.diffuse_texname);
			mats.push_back(mat);
		}

		// initialise the static meshes
		for (const auto& shape : shapes) {
			mesh::mesh_template_t mesh_template;
			for (const auto& index : shape.mesh.indices) {
				const float* pos = &attrib.vertices[3 * index.vertex_index];
				mesh_template.positions.emplace_back(pos[0], pos[1], pos[2]);
				if (!attrib.texcoords.empty()) {
					const float* tc = &attrib.texcoords[2 * index.texcoord_index];
					mesh_template.tex_coords.emplace_back(tc[0], tc[1]);
				}
				if (!attrib.normals.empty()) {
					const float* norm = &attrib.normals[3 * index.normal_index];
					mesh_template.normals.emplace_back(norm[0], norm[1], norm[2]);
				}
			}
			model.meshes.push_back(mesh::init(mesh_template));
			model.materials.push_back(mats[shape.mesh.material_ids[0]]);
		}
		return model;
	}

	void destroy(const model_t& model) {
		for (auto const& mesh : model.meshes) {
			mesh::destroy(mesh);
		}
		for (auto const& mat : model.materials) {
			texture_2d::destroy(mat.diffuse_map);
			texture_2d::destroy(mat.specular_map);
		}
	}
} // namespace model

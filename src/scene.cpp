#include "ass3/scene.hpp"
#include "ass3/shapes.hpp"
#include "ass3/cubemap.hpp"
#include "ass3/texture_2d.hpp"
#include <iostream>

const char* SKYBOX_BASE_PATH = "res/skybox/sky";

const char* SAND_DIFFUSE_MAP_PATH = "res/textures/sand/Sand_004_COLOR.png";
const char* SAND_HEIGHT_MAP_PATH = "res/textures/sand/Sand_004_Height.png";
const char* SAND_NORMAL_MAP_PATH = "res/textures/sand/Sand_004_Normal.png";
const char* SAND_AMBIENT_MAP_PATH = "res/textures/sand/Sand_004_OCC.png";
const char* SAND_ROUGH_MAP_PATH = "res/textures/sand/Sand_004_ROUGH.png";

const char* MARCCOIN_NORMAL_MAP = "res/textures/marccoin_normal_map.png";

namespace scene {
	node_t make_marccoin() {
		float coin_thickness = 0.1f;

		auto face1 = scene::node_t{};
		auto face1_template = shapes::make_circle(1.0f);
		shapes::calc_vertex_normals(face1_template);
		face1.model.meshes.push_back(mesh::init(face1_template));
		face1.model.materials.push_back({.cube_map = cubemap::make_cubemap(SKYBOX_BASE_PATH),
		                                 .normal_map = texture_2d::init(MARCCOIN_NORMAL_MAP),
		                                 .diffuse = glm::vec4(0.805, 0.64, 0.054, 1),
		                                 .specular = glm::vec3(1),
		                                 .phong_exp = 50.0});
		face1.translation.z = coin_thickness / 2.0f;

		auto face2 = face1;
		face2.translation.z = -coin_thickness / 2.0f;
		face2.rotation.y = glm::radians(180.0f);

		auto edge = scene::node_t{};
		auto edge_mesh_template = shapes::make_cylinder(1.0f, coin_thickness);
		shapes::calc_vertex_normals(edge_mesh_template);
		edge.model.meshes.push_back(mesh::init(edge_mesh_template));
		edge.model.materials.push_back({.cube_map = cubemap::make_cubemap(SKYBOX_BASE_PATH),
		                                .diffuse = glm::vec4(0.805, 0.64, 0.054, 1),
		                                .specular = glm::vec3(1),
		                                .phong_exp = 50.0});

		auto marccoin = scene::node_t{};
		marccoin.translation.y = 1.0f;

		marccoin.children.push_back(face1);
		marccoin.children.push_back(face2);
		marccoin.children.push_back(edge);

		marccoin.translation = glm::vec3(0, 2, 0);
		marccoin.rotation = glm::vec3(glm::radians(-70.0), glm::radians(-45.0f), 0);
		marccoin.scale = glm::vec3(5);

		return marccoin;
	}

	node_t make_volume(int width,
	                   int height,
	                   int depth,
	                   model::material_t top_material,
	                   model::material_t side_material,
	                   scene::node_t::KIND top_kind,
	                   scene::node_t::KIND side_kind) {
		auto volume = scene::node_t{};

		std::vector<std::pair<int, int>> dims = {
		   {depth, height},
		   {depth, height},
		   {width, depth},
		   {width, depth},
		};

		std::vector<glm::vec3> rotations = {
		   {0, glm::radians(-90.0), 0},
		   {0, glm::radians(90.0), 0},
		   {glm::radians(90.0), 0, 0},
		   {glm::radians(-90.0), 0, 0},
		};

		std::vector<glm::vec3> translations = {
		   {-width / 2.0f, 0, depth / 2.0f},
		   {width / 2.0f, 0, depth / 2.0f},
		   {0, -height / 2.0f, depth / 2.0f},
		   {0, height / 2.0f, depth / 2.0f},
		};

		for (auto i = size_t{0}; i < 4; ++i) {
			auto side = scene::node_t{};
			auto side_template = shapes::make_plane(dims[i].first, dims[i].second);
			shapes::calc_vertex_normals(side_template);
			side.kind = side_kind;
			side.model.meshes.push_back(mesh::init(side_template));
			side.model.materials.push_back(side_material);
			side.translation = translations[i];
			side.rotation = rotations[i];
			volume.children.push_back(side);
		}

		auto top = scene::node_t{};
		auto top_template = shapes::make_plane(width, height);
		shapes::calc_vertex_normals(top_template);
		top.kind = top_kind;
		top.model.meshes.push_back(mesh::init(top_template));
		top.model.materials.push_back(top_material);
		top.translation = glm::vec3(0, 0, depth);

		volume.children.push_back(top);

		return volume;
	}

	node_t make_sand_volume(int width, int height, int depth) {
		auto sand_top_material = model::material_t{
		   .diffuse_map = texture_2d::init(SAND_DIFFUSE_MAP_PATH),
		   .normal_map = texture_2d::init(SAND_NORMAL_MAP_PATH),
		   .height_map = texture_2d::init(SAND_HEIGHT_MAP_PATH),
		   .specular = glm::vec3(0.5),
		};

		auto sand_side_material = model::material_t{.height_map = sand_top_material.height_map,
		                                            .diffuse = glm::vec4(0.86, 0.73, 0.56, 1),
		                                            .specular = glm::vec3(1),
		                                            .phong_exp = 50.0f};

		auto sand_volume = make_volume(width,
		                               height,
		                               depth,
		                               sand_top_material,
		                               sand_side_material,
		                               scene::node_t::STATIC_MESH,
		                               scene::node_t::STATIC_MESH);

		sand_volume.rotation.x = glm::radians(-90.0);

		return sand_volume;
	}

	node_t
	make_water_volume(int width, int height, int depth, GLuint refraction_map, GLuint reflection_map) {
		auto water_surface_mat = model::material_t{
		   .diffuse_map = refraction_map,
		   .reflection_map = reflection_map,
		   .diffuse = glm::vec4(0.306, 0.69, 0.76, 0.3),
		   .specular = glm::vec3(0.5),
		   .phong_exp = 50.0f,
		   .cube_map_factor = 0.3f,
		   .reflection_map_factor = 0.4f,
		};
		auto water_side_mat = model::material_t{.diffuse = glm::vec4(0.306, 0.69, 0.76, 0.3)};
		node_t water_volume = make_volume(width,
		                                  height,
		                                  depth,
		                                  water_surface_mat,
		                                  water_side_mat,
		                                  scene::node_t::WATER_SURFACE,
		                                  scene::node_t::WATER);

		water_volume.rotation.x = glm::radians(-90.0);
		return water_volume;
	}

	model::model_t make_skybox() {
		auto skybox = model::model_t{};
		skybox.meshes.push_back(mesh::init(shapes::make_cube(1.0f)));
		skybox.materials.push_back({.cube_map = cubemap::make_cubemap(SKYBOX_BASE_PATH)});
		return skybox;
	}

} // namespace scene
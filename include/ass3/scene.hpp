#ifndef COMP3421_SCENE_HPP
#define COMP3421_SCENE_HPP

#include "ass3/model.hpp"
#include "ass3/euler_camera.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

namespace scene {
	struct node_t {
		enum KIND {
			EMPTY,
			STATIC_MESH,
			REFLECTIVE,
			WATER_SURFACE,
			WATER,
		} kind = EMPTY;
		model::model_t model;
		glm::vec3 translation = glm::vec3(0.0);
		glm::vec3 rotation = glm::vec3(0.0); // vec3 of euler angles
		glm::vec3 scale = glm::vec3(1.0);
		std::vector<node_t> children;
		glm::vec2 polygon_offset = glm::vec2(0.0); // (factor, units)
		bool invisible = false;
	};

	node_t make_marccoin();

	node_t make_sand_volume(int width, int height, int depth);

	node_t make_water_volume(int width,
	                         int height,
	                         int depth,
	                         GLuint refraction_map = 0,
	                         GLuint reflection_map = 0);

	model::model_t make_skybox();

} // namespace scene

#endif // COMP3421_SCENE_HPP

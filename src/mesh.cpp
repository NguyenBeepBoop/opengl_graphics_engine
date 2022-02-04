#include "ass3/mesh.hpp"

#include <iostream>

namespace mesh {

	// helper function - assumes ebo and vbo are already bound
	void init_data(const mesh_template_t& mesh_template, GLenum usage) {
		if (!mesh_template.indices.empty()) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, // target
			             (GLsizeiptr)(mesh_template.indices.size() * sizeof(GLuint)), // num bytes in
			                                                                          // the data
			             &mesh_template.indices[0], // pointer to the actual data
			             usage);
		}

		size_t positions_size = mesh_template.positions.size() * sizeof(glm::vec3);
		size_t colors_size = mesh_template.colors.size() * sizeof(glm::vec3);
		size_t tex_coords_size = mesh_template.tex_coords.size() * sizeof(glm::vec2);
		size_t normals_size = mesh_template.normals.size() * sizeof(glm::vec3);
		size_t data_size = positions_size + colors_size + tex_coords_size + normals_size;

		// create buffer and fill with our line data
		glBufferData(GL_ARRAY_BUFFER, data_size, nullptr, usage);

		size_t offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, positions_size, &mesh_template.positions[0].x);
		offset += positions_size;
		if (colors_size) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, colors_size, &mesh_template.colors[0].x);
			offset += colors_size;
		}
		if (tex_coords_size) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, tex_coords_size, &mesh_template.tex_coords[0].x);
			offset += tex_coords_size;
		}
		if (normals_size) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, normals_size, &mesh_template.normals[0].x);
			offset += normals_size;
		}
	}

	mesh_t init(const mesh_template_t& mesh_template, GLenum usage) {
		mesh_t mesh;

		glGenVertexArrays(1, &mesh.vao);
		glBindVertexArray(mesh.vao);

		bool has_indices = !mesh_template.indices.empty();
		mesh.indices_count =
		   (GLsizei)(has_indices ? mesh_template.indices.size() : mesh_template.positions.size());
		if (has_indices) {
			glGenBuffers(1, &mesh.ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		}

		glGenBuffers(1, &mesh.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

		init_data(mesh_template, usage);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		GLuint attrib_index = 1;
		size_t offset = mesh_template.positions.size() * sizeof(glm::vec3);

		if (!mesh_template.colors.empty()) {
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
			offset += mesh_template.colors.size() * sizeof(glm::vec3);
		}
		++attrib_index;

		if (!mesh_template.tex_coords.empty()) {
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
			offset += mesh_template.tex_coords.size() * sizeof(glm::vec2);
		}
		++attrib_index;

		if (!mesh_template.normals.empty()) {
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
			offset += mesh_template.normals.size() * sizeof(glm::vec3);
		}
		++attrib_index;

		glBindVertexArray(0);
		return mesh;
	}

	void draw(const mesh_t& mesh, GLenum draw_mode) {
		glBindVertexArray(mesh.vao);
		if (mesh.ebo) {
			glDrawElements(draw_mode, mesh.indices_count, GL_UNSIGNED_INT, nullptr);
		}
		else {
			glDrawArrays(draw_mode, 0, mesh.indices_count);
		}
		glBindVertexArray(0);
	}

	void dynamic_draw(mesh_t& mesh, const mesh_template_t& mesh_template, GLenum draw_mode) {
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);

		init_data(mesh_template, GL_DYNAMIC_DRAW);

		bool has_indices = !mesh_template.indices.empty();
		mesh.indices_count =
		   (GLsizei)(has_indices ? mesh_template.indices.size() : mesh_template.positions.size());

		if (mesh.ebo) {
			glDrawElements(draw_mode, mesh.indices_count, GL_UNSIGNED_INT, nullptr);
		}
		else {
			glDrawArrays(draw_mode, 0, mesh.indices_count);
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void destroy(const mesh_t& mesh) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
	}
} // namespace mesh

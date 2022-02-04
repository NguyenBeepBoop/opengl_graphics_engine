#include "ass3/renderer.hpp"
#include "ass3/texture_2d.hpp"
#include "ass3/euler_camera.hpp"
#include "ass3/mesh.hpp"

#include "chicken3421/chicken3421.hpp"

const char* VERT_PATH = "res/shaders/shader.vert";
const char* FRAG_PATH = "res/shaders/shader.frag";

const char* SKYBOX_VERT_PATH = "res/shaders/skybox.vert";
const char* SKYBOX_FRAG_PATH = "res/shaders/skybox.frag";

namespace renderer {
	int locate(const std::string& name) {
		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		int loc = glGetUniformLocation(program, name.c_str());
		chicken3421::expect(loc != -1, "uniform not found: " + name);
		return loc;
	}

	void set_uniform(const std::string& name, float value) {
		glUniform1f(locate(name), value);
	}

	void set_uniform(const std::string& name, int value) {
		glUniform1i(locate(name), value);
	}

	void set_uniform(const std::string& name, glm::vec4 value) {
		glUniform4fv(locate(name), 1, glm::value_ptr(value));
	}

	void set_uniform(const std::string& name, glm::vec3 value) {
		glUniform3fv(locate(name), 1, glm::value_ptr(value));
	}

	void set_uniform(const std::string& name, const glm::mat4& value) {
		glUniformMatrix4fv(locate(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	GLuint load_program(const std::string& vs_path, const std::string& fs_path) {
		GLuint vs = chicken3421::make_shader(vs_path, GL_VERTEX_SHADER);
		GLuint fs = chicken3421::make_shader(fs_path, GL_FRAGMENT_SHADER);
		GLuint handle = chicken3421::make_program(vs, fs);
		chicken3421::delete_shader(vs);
		chicken3421::delete_shader(fs);

		return handle;
	}

	renderer_t init(const glm::mat4& projection) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto renderer = renderer_t{};
		renderer.projection = projection;

		// make the render program
		renderer.program = load_program(VERT_PATH, FRAG_PATH);
		renderer.skybox_program = load_program(SKYBOX_VERT_PATH, SKYBOX_FRAG_PATH);

		return renderer;
	}

	void draw_skybox(const model::model_t& model, const renderer_t& renderer, const glm::mat4& view) {
		glUseProgram(renderer.skybox_program);
		glFrontFace(GL_CW);
		glDepthMask(GL_FALSE);

		set_uniform("uCubeMap", 0);
		set_uniform("uViewProj", renderer.projection * glm::mat4(glm::mat3(view)));
		for (auto i = size_t{0}; i < model.meshes.size(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, model.materials[i].cube_map);
			mesh::draw(model.meshes[i]);
		}
		glFrontFace(GL_CCW);
		glDepthMask(GL_TRUE);
		glUseProgram(0);
	}

	void draw(const scene::node_t& node,
	          const renderer_t& renderer,
	          glm::mat4 model,
	          glm::vec2 polygon_offset = glm::vec2(0)) {
		model *= glm::translate(glm::mat4(1.0), node.translation);
		model *= glm::rotate(glm::mat4(1.0), node.rotation.z, glm::vec3(0, 0, 1));
		model *= glm::rotate(glm::mat4(1.0), node.rotation.y, glm::vec3(0, 1, 0));
		model *= glm::rotate(glm::mat4(1.0), node.rotation.x, glm::vec3(1, 0, 0));
		model *= glm::scale(glm::mat4(1.0), node.scale);

		set_uniform("uModel", model);

		if (node.invisible)
			return;

		// TODO Part A: do glPolygonOffset with accumulated z-fighting offset from parents
		for (auto i = size_t{0}; i < node.model.meshes.size(); ++i) {
			set_uniform("uDiffuseMapFactor", node.model.materials[i].diffuse_map ? 1.0f : 0.0f);
			set_uniform("uSpecularMapFactor", node.model.materials[i].specular_map ? 1.0f : 0.0f);
			set_uniform("uCubeMapFactor",
			            node.model.materials[i].cube_map ? node.model.materials[i].cube_map_factor
			                                             : 0.0f);
			set_uniform("uNormalMapFactor", node.model.materials[i].normal_map ? 1.0f : 0.0f);
			
			set_uniform("uMat.ambient", node.model.materials[i].ambient);
			set_uniform("uMat.diffuse", node.model.materials[i].diffuse);
			set_uniform("uMat.specular", node.model.materials[i].specular);
			set_uniform("uMat.phongExp", node.model.materials[i].phong_exp);
			set_uniform("uIsWater",
			            node.kind == scene::node_t::WATER || node.kind == scene::node_t::WATER_SURFACE);
			set_uniform("uIsWaterSurface", node.kind == scene::node_t::WATER_SURFACE);
            
            //set_uniform("hdrBuffer", 0);
			glActiveTexture(GL_TEXTURE0);
			texture_2d::bind(node.model.materials[i].diffuse_map);
			glActiveTexture(GL_TEXTURE1);
			texture_2d::bind(node.model.materials[i].specular_map);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, node.model.materials[i].cube_map);
			glActiveTexture(GL_TEXTURE3);
			texture_2d::bind(node.model.materials[i].normal_map);
			glActiveTexture(GL_TEXTURE4);
			texture_2d::bind(node.model.materials[i].height_map);
			mesh::draw(node.model.meshes[i]);
		}
		for (auto const& child : node.children) {
			draw(child, renderer, model, polygon_offset);
		}
	}

	void render(const renderer_t& renderer,
	            const euler_camera::camera_t& camera,
	            const scene::node_t& scene,
	            const model::model_t& skybox) {
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);

		auto view = euler_camera::get_view(camera);
		draw_skybox(skybox, renderer, view);

		glUseProgram(renderer.program);
		set_uniform("uCameraPos", camera.pos);

		set_uniform("uSun.direction", renderer.sun_light_dir);
		set_uniform("uSun.diffuse", renderer.sun_light_diffuse);
		set_uniform("uSun.ambient", renderer.sun_light_ambient);
		set_uniform("uSun.specular", renderer.sun_light_specular);
        
		set_uniform("uSpot.position", renderer.spot_light_pos);
		set_uniform("uSpot.diffuse", renderer.spot_light_diffuse);
		set_uniform("uSpot.ambient", renderer.spot_light_ambient);
		set_uniform("uSpot.specular", renderer.spot_light_specular);

        std::vector<glm::vec3> point_light_pos;
        std::vector<glm::vec3> point_light_diffuse;
        std::vector<glm::vec3> point_light_ambient;
        std::vector<glm::vec3> point_light_specular;
        
        point_light_pos.push_back(glm::vec3(0,5,-14));
        point_light_pos.push_back(glm::vec3(25,5,-25));
        point_light_pos.push_back(glm::vec3(30,5,54));
        point_light_pos.push_back(glm::vec3(24,5,-39));
        point_light_pos.push_back(glm::vec3(46,5,-44));
        
		point_light_diffuse.push_back(glm::vec3(20.f, 18.f, 15.f));
        point_light_diffuse.push_back(glm::vec3(0.f, 15.f, 0.f));
        point_light_diffuse.push_back(glm::vec3(15.f, 0.f, 0.f));
        point_light_diffuse.push_back(glm::vec3(0.f, 0.f, 15.f));
        point_light_diffuse.push_back(glm::vec3(15.f, 15.f, 0.f));
        
        point_light_ambient.push_back(glm::vec3(0.25f));
        point_light_ambient.push_back(glm::vec3(0.25f));
        point_light_ambient.push_back(glm::vec3(0.25f));
        point_light_ambient.push_back(glm::vec3(0.25f));
        point_light_ambient.push_back(glm::vec3(0.25f));

		point_light_specular.push_back(glm::vec3(0.12f));
		point_light_specular.push_back(glm::vec3(0.12f));
        point_light_specular.push_back(glm::vec3(0.12f));
        point_light_specular.push_back(glm::vec3(0.12f));
        point_light_specular.push_back(glm::vec3(0.12f));
        
        set_uniform("uPoint[0].position", point_light_pos[0]);
		set_uniform("uPoint[0].diffuse", point_light_diffuse[0]);
		set_uniform("uPoint[0].ambient", point_light_ambient[0]);
		set_uniform("uPoint[0].specular", point_light_specular[0]);

        set_uniform("uPoint[1].position", point_light_pos[1]);
		set_uniform("uPoint[1].diffuse", point_light_diffuse[1]);
		set_uniform("uPoint[1].ambient", point_light_ambient[1]);
		set_uniform("uPoint[1].specular", point_light_specular[1]);

        set_uniform("uPoint[2].position", point_light_pos[2]);
		set_uniform("uPoint[2].diffuse", point_light_diffuse[2]);
		set_uniform("uPoint[2].ambient", point_light_ambient[2]);
		set_uniform("uPoint[2].specular", point_light_specular[2]);
	
        set_uniform("uPoint[3].position", point_light_pos[3]);
		set_uniform("uPoint[3].diffuse", point_light_diffuse[3]);
		set_uniform("uPoint[3].ambient", point_light_ambient[3]);
		set_uniform("uPoint[3].specular", point_light_specular[3]);

        set_uniform("uPoint[4].position", point_light_pos[4]);
		set_uniform("uPoint[4].diffuse", point_light_diffuse[4]);
		set_uniform("uPoint[4].ambient", point_light_ambient[4]);
		set_uniform("uPoint[4].specular", point_light_specular[4]);

		set_uniform("uDiffuseMap", 0);
		set_uniform("uSpecularMap", 1);
		set_uniform("uCubeMap", 2);
		set_uniform("uNormalMap", 3);
		set_uniform("uHeightMap", 4);
		set_uniform("uNow", (float) glfwGetTime());

		set_uniform("uClipPlane", renderer.clip_plane);
        set_uniform("hdrBuffer", 0);
		auto view_proj = renderer.projection * view;
		set_uniform("uViewProj", view_proj);

		draw(scene, renderer, glm::mat4(1.0f));
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
} // namespace renderer

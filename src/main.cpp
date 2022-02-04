#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>

#include <chicken3421/chicken3421.hpp>

#include "ass3/texture_2d.hpp"
#include "ass3/shapes.hpp"
#include "ass3/euler_camera.hpp"
#include "ass3/memes.hpp"
#include "ass3/renderer.hpp"
#include "ass3/framebuffer.hpp"

const char *MAIN_PATH = "res/obj/SnowTerrain/winter_house.obj";
const char *GT3_PATH = "res/obj/SPECTER_GT3_obj/SPECTER_GT3_.obj";
const char *SNOWMAN_PATH = "res/obj/snowman/snowman_finish.obj";
const char *REINDEER_PATH = "res/obj/reindeer/Charector_reindeer.obj";
const char *TOWER_PATH = "res/obj/tower/tower.obj";
const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

const char* WIN_TITLE = "Ass3";

namespace {
	double time_delta() {
		static double then = glfwGetTime();
		double now = glfwGetTime();
		double dt = now - then;
		then = now;
		return dt;
	}
} // namespace

std::pair<int, int> get_framebuffer_size(GLFWwindow *win);

void update_scene(GLFWwindow* window, float dt, scene::node_t& scene) {
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		scene.rotation.y += dt * 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		scene.rotation.y -= dt * 0.1;
	}
}

int main() {
#ifndef __APPLE__
	chicken3421::enable_debug_output();
#endif
	GLFWwindow* window = marcify(chicken3421::make_opengl_window(SCR_WIDTH, SCR_HEIGHT, WIN_TITLE));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    framebuffer::framebuffer_t framebuffer = framebuffer::make_framebuffer(fb_width, fb_height);

	auto camera = euler_camera::make_camera({0, 10, 20}, {0, 0, 0});
	auto renderer = renderer::init(
	   glm::perspective(glm::radians(60.0), (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.1, 1000.0));

	auto skybox = scene::make_skybox();

	int width = 1000;
	int height = 1000;
	int depth = 8;

	auto scene = scene::node_t{};
	scene.model = model::load(MAIN_PATH);
	scene.scale = glm::vec3(4,4,4);
	
	auto coin = scene::make_marccoin();
	coin.scale = glm::vec3(2.f, 2.f, 2.f);
	coin.translation = glm::vec3(-6,1,-6);
	
	auto car = scene::node_t{};
	car.model = model::load(GT3_PATH);
	car.translation = glm::vec3(7, 0.2, -7);
	car.scale = glm::vec3(2.f, 2.f, 2.f);
	
	auto snowman = scene::node_t{};
	snowman.model = model::load(SNOWMAN_PATH);
	snowman.translation = glm::vec3(-6, 0.5, 6);
	snowman.rotation = glm::vec3(0,3,0);
	snowman.scale = glm::vec3(2.f, 2.f, 2.f);

    auto reindeer = scene::node_t{};
	reindeer.model = model::load(REINDEER_PATH);
	reindeer.translation = glm::vec3(0, 0.2, -7);
	reindeer.scale = glm::vec3(1.f, 1.f, 1.f);
	
	auto sand_volume = scene::make_sand_volume(width, height, depth / 8);
	sand_volume.translation = glm::vec3(0,-0.97f,-3.f);

	scene.children.push_back(coin);
	scene.children.push_back(reindeer);
	scene.children.push_back(sand_volume);
	scene.children.push_back(car);
	scene.children.push_back(snowman);
	
	while (!glfwWindowShouldClose(window)) {
		auto dt = (float)time_delta();

		euler_camera::update_camera(camera, window, dt);
		update_scene(window, dt, scene);
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
        glEnable(GL_CLIP_DISTANCE0);
        renderer::render(renderer, camera, scene, skybox);
        glDisable(GL_CLIP_DISTANCE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        
		glEnable(GL_CLIP_DISTANCE0);
		renderer::render(renderer, camera, scene, skybox);
		glDisable(GL_CLIP_DISTANCE0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}

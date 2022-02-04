
#ifndef COMP3421_FRAMEBUFFER_HPP
#define COMP3421_FRAMEBUFFER_HPP

#include <glad/glad.h>

namespace framebuffer {
	struct framebuffer_t {
		GLuint fbo;
		GLuint texture;
		GLuint rbo;
	};

	/**
	 * Create a framebuffer
	 * @returns struct containing handle to FBO and the resulting texture
	 */
	framebuffer_t make_framebuffer(int width, int height);

	/**
	 * Destroy a framebuffer, releasing GPU resources
	 * @param framebuffer The framebuffer to destroy
	 */
	void delete_framebuffer(framebuffer_t& framebuffer);

} // namespace framebuffer

#endif // COMP3421_FRAMEBUFFER_HPP

#ifndef COMP3421_ASS3_CUBEMAP_HPP
#define COMP3421_ASS3_CUBEMAP_HPP

#include <glad/glad.h>
#include <string>

namespace cubemap {
	/**
	 * Create a cubemap from 6 textures
	 * @param base_path Path of cubemap textures, without extension
	 * @param extension File extension for cubemap textures, with dot
	 * @return OpenGL texture handle
	 */
	GLuint make_cubemap(const std::string& base_path, const std::string& extension = ".jpg");
} // namespace cubemap

#endif // COMP3421_ASS3_CUBEMAP_HPP

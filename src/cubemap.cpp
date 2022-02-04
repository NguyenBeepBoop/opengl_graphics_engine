#include <glad/glad.h>

#include <chicken3421/chicken3421.hpp>

#include <ass3/cubemap.hpp>

namespace {
	const char* side_suffices[] = {"_right", "_left", "_top", "_bottom", "_front", "_back"};
} // namespace

namespace cubemap {
	GLuint make_cubemap(const std::string& base_path, const std::string& extension) {
		GLuint cubemap;
		glGenTextures(1, &cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

		for (auto i = size_t{0}; i < 6; ++i) {
			chicken3421::image_t image =
			   chicken3421::load_image(base_path + side_suffices[i] + extension, false);
			GLenum format = image.n_channels == 3 ? GL_RGB : GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			             0,
			             format,
			             image.width,
			             image.height,
			             0,
			             format,
			             GL_UNSIGNED_BYTE,
			             image.data);
			chicken3421::delete_image(image);
		}

		// wrap options
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		// mag/min options
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return cubemap;
	}
} // namespace cubemap

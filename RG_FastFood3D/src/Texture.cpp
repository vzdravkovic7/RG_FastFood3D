#include "../include/Texture.h"
#include "../include/Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"


GLuint Texture::FromFile(const std::string& path) {
	return loadImageToTexture(path.c_str());
}

GLuint Texture::White()
{
    static GLuint whiteTex = 0;
    if (whiteTex != 0) return whiteTex;

    unsigned char data[3] = { 255, 255, 255 };

    glGenTextures(1, &whiteTex);
    glBindTexture(GL_TEXTURE_2D, whiteTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
        GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return whiteTex;
}

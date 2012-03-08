/*
 * textures.cpp
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#include "textures.h"

/* Texture constructor */
Texture::Texture(void) {
	widthi = 0;
	heighti = 0;
	mode = 0;
}

void Texture::Destroy() {
	printf("[%s] Deleting...", tName);
	glDeleteTextures(1,&gltexi);
	widthi = 0;
	heighti = 0;
	mode = 0;
	printf(" OK\n");
}

void Texture::Load(const char* fname) {
	/* Load Texture as SDLSurface and also GLTexture*/
	strcpy(tName, fname);
	mode = GL_RGB;
	SDL_Surface* sdltex = IMG_Load(fname);
	widthi = sdltex->w;
	heighti = sdltex->h;
	glGenTextures(1, &gltexi);
	glBindTexture(GL_TEXTURE_2D, gltexi);
	checkError("glBindTexture");
	if (sdltex->format->BytesPerPixel == 4) {
		mode = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, mode, widthi, heighti, 0, mode, GL_UNSIGNED_BYTE, sdltex->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	printf("Loaded Texture " "%s" " - Width: %i - Height: %i - BPP: %i\n", fname, widthi, heighti, sdltex->format->BytesPerPixel);
	SDL_FreeSurface(sdltex);
}

GLuint Texture::checkError(const char *context)
{
  GLuint err = glGetError();
  if (err > 0 )  {
    printf("ERROR LOADING TEXTURE%s\n", glGetError());
  }
  return err;
}

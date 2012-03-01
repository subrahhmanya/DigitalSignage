/*
 * textures.h
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#ifndef TEXTURES_H_
#define TEXTURES_H_

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

class Texture {
public:
	Texture();
	void Load(const char* fname);
	void Destroy();
	GLuint gltex() {
		return gltexi;
	}
	int width() {
		return widthi;
	}
	int height() {
		return heighti;
	}
private:
	SDL_Surface* sdltex;
	GLuint gltexi;
	int widthi;
	int heighti;
	int mode;
};

#endif /* TEXTURES_H_ */

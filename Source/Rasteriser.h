#ifndef COMPUTER_GRAPHICS_RASTERISER_H
#define COMPUTER_GRAPHICS_RASTERISER_H


#include <SDL_video.h>
#include "Renderer.h"
#include "TestModel.h"
#include <glm/glm.hpp>
#include "SDLauxiliary.h"

using glm::vec3;
class Rasteriser : Renderer {

public:
		Rasteriser(SDL_Surface* screen);
		void Draw(Camera &camera,vector<Triangle>& triangles);

private:
		SDL_Surface* screen;
		int width;
		int height;
		float * depthBuffer;
		inline vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon( Camera & camera, const Triangle& t );

};


#endif

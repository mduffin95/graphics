#ifndef RASTERISER_H
#define RASTERISER_H

#include "Renderer.h"
#include "TestModel.h"

using glm::vec3;
class Rasteriser : Renderer {

public:
		Rasteriser(SDL_Surface* screen);
		void Draw(Camera &camera,vector<Triangle>& triangles);

private:
		float * depthBuffer;
		vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon( Camera & camera, const Triangle& t );

};


#endif

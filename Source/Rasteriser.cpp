#include <iostream>
#include <glm/glm.hpp>
#include "TestModel.h"
#include "Camera.h"
#include "Lighting.h"
#include "Rasteriser.h"


using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec2;
using glm::ivec2;

#define C(x,y,width,height)  (x + y * width)

vec3 Rasteriser::getPoint(int x, int y, int w, int h)
{
  return vec3(
          (x - width/2)/ (float) width,
          -(y - height/2)/ (float) height,
           1);
}


Rasteriser::Rasteriser(SDL_Surface *screen) : Renderer(screen) {
	depthBufferCamera = (float*)malloc(sizeof(float)*height*width);
	depthBufferLight = (float*)malloc(sizeof(float)*height*width);
	colourBuffer = (vec3*)malloc(sizeof(vec3)*height*width);
}


void Rasteriser::DrawPolygon(const Camera &camera, const Lighting &lighting, const Triangle &t) {
	//Transform to camera coordinates
	vec3 v0_dash = camera.transform(t.v0);
	vec3 v1_dash = camera.transform(t.v1);
	vec3 v2_dash = camera.transform(t.v2);

	//Matrix of vertices
	mat3 M(v0_dash, v1_dash, v2_dash);

	mat3 M_i = glm::inverse(M);

	vec3 w = vec3(1, 1, 1) * M_i;
	//Get edge functions (rows of M_inv)

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			//vec3 p = getPoint(x, y);
			vec3 p = getPoint(x, y, width, height);
			vec3 E = M_i * p;
			//Check all edge functions
			if (E.x >= 0 &&
					E.y >= 0 &&
					E.z >= 0) {
				float W = 1 / glm::dot(w, p);
				if (depthBufferCamera[C(x, y, width, height)] > W) {
					depthBufferCamera[C(x, y, width, height)] = W;
					colourBuffer[C(x, y, width, height)] = t.color;
				}
			}
		}
	}

	//Transform to camera coordinates
	vec3 v0_dash_c = lighting.transform(v0_dash);
	vec3 v1_dash_c = lighting.transform(v1_dash);
	vec3 v2_dash_c = lighting.transform(v2_dash);

	//Matrix of vertices
	mat3 M_l(v0_dash_c, v1_dash_c, v2_dash_c);

	mat3 M_i_l = glm::inverse(M_l);

	vec3 w_l = vec3(1, 1, 1) * M_i_l;
	//Get edge functions (rows of M_inv)

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			//vec3 p = getPoint(x, y);
			vec3 p = getPoint(x, y, width, height);
			vec3 E = M_i_l * p;
			//Check all edge functions
			if (E.x >= 0 &&
					E.y >= 0 &&
					E.z >= 0) {
				float W = 1 / glm::dot(w_l, p);
				if (depthBufferLight[C(x, y, width, height)] > W) {
					depthBufferLight[C(x, y, width, height)] = W;
				}
			}
		}
	}

}


void Rasteriser::Draw(const Camera &camera, const Lighting &lighting, const vector<Triangle> &triangles)
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	for(int i = 0; i < height * width; i++){
		depthBufferCamera[i] = INFINITY;
		depthBufferLight[i] = INFINITY;
		colourBuffer[i] = vec3(0,0,0);
	}

	for( int i=0; i<triangles.size(); ++i )
	{
		DrawPolygon(camera, lighting, triangles[i]);
	}


	for(int y = 0; y < height ; y++){
		for(int x = 0; x < width; x++){
			if(depthBufferLight[C(x,y,width,height)] 
         < depthBufferCamera[C(x,y,width,height)]) {
				colourBuffer[C(x,y,width, height)] = vec3(0,0,0);
			}
			PutPixelSDL(screen, x, y, colourBuffer[C(x,y,width, height)]);
		}
	}


	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

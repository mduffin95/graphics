#include <iostream>
#include <glm/glm.hpp>
#include "TestModel.h"
#include "Camera.h"
#include "Rasteriser.h"


using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec2;

#define C(x,y,width,height)  (x + y * width)

vec3 Rasteriser::getPoint(int x, int y, int w, int h)
{
  return vec3(
          (x - width/2)/ (float) width,
          -(y - height/2)/ (float) height,
           1);
}


float Lighting (const Triangle& t){


}

Rasteriser::Rasteriser(SDL_Surface* screen) : Renderer(screen) {
	this->depthBuffer = (float*)malloc(sizeof(float)*height*width);
}

void Rasteriser::DrawPolygon( Camera & camera, const Triangle& t )
{
  //Transform to camera coordinates
  vec3 v0_dash = camera.transform(t.v0);
  vec3 v1_dash = camera.transform(t.v1);
  vec3 v2_dash = camera.transform(t.v2);
  
  //Matrix of vertices
  mat3 M(v0_dash, v1_dash, v2_dash);

  mat3 M_i = glm::inverse(M);

  vec3 w = vec3(1,1,1) * M_i;
  //Get edge functions (rows of M_inv)

  for (int y=0; y<height; y++)
  {
    for(int x=0; x<width; x++)
    {
      //vec3 p = getPoint(x, y);
      vec3 p = getPoint(x, y, width, height);
      vec3 E = M_i * p;
      //Check all edge functions
      if (E.x >= 0 &&
          E.y >= 0 &&
          E.z >= 0)
      {
        float W = 1/glm::dot(w, p);
				if(depthBuffer[C(x,y,width,height)] > W )
        {
					depthBuffer[C(x,y,width,height)] = W;
					PutPixelSDL(screen, x, y, t.color);
				}
      }
    }
  }
}

void Rasteriser::Draw(Camera& camera, vector<Triangle>& triangles)
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	for(int i = 0 ; i < height * width ; i ++){
		depthBuffer[i] = INFINITY;
	}

	for( int i=0; i<triangles.size(); ++i )
	{
		DrawPolygon(camera, triangles[i]);
	}


	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

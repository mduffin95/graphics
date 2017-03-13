#include "Raytracer.h"
#include "Light.h"
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include "Object.h"
#include "Intersection.h"
#include <SDL.h>
#include "Material.h"
using glm::vec3;


Raytracer::Raytracer(SDL_Surface* screen, Camera &camera, std::vector<Light>& lights, std::vector<std::shared_ptr<Object>>& objects, int dofSamples) : Renderer(screen, camera, lights, objects), focalLength(width), indirectLight( 0.5f, 0.5f, 0.5f), dofSamples(dofSamples)
{
  srand (static_cast <unsigned> (time(0)));
}



void Raytracer::ClosestIntersection(Intersection& closest)  
{
  Intersection tmp = closest;
  closest.distance = std::numeric_limits<float>::max();
  for (unsigned i=0; i<objects.size(); i++)
  {
    tmp.didIntersect = false;
    objects[i]->Intersect(tmp);
    if (tmp.didIntersect && (tmp.distance <= closest.distance))
    {
      closest = tmp;
    }
  }
}


void Raytracer::Draw()
{
  vec3 colour;
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for( int y=0; y<height; ++y )
	{
		for( int x=0; x<width; ++x )
		{
      colour = CastAtPixel(x, y);
      //vec3 colour( 1.0, 0.0, 0.0 );
      PutPixelSDL( screen, x, y, colour );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

vec3 Raytracer::CastRay(const Ray ray)
{
  float apertureRadius = 0.1f;
  float focalDepth = 3.0f;
  float sampleWeight = 1.0f / dofSamples;
  vec3 point = ray.direction * focalDepth;
  vec3 colour = vec3(0,0,0);
  float r1 = 0;
  float r2 = 0;
  
  for( int i=0; i<dofSamples; i++)
  {
    if( dofSamples > 1 )
    {
      r1 = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));
      r2 = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));
    }

    vec3 randomise = vec3(r1*apertureRadius, r2*apertureRadius, 0);
    Intersection isec;
    isec.ray.direction = camera.transform_c2w_rotate(point - randomise);
    isec.ray.origin = ray.origin + randomise;

    ClosestIntersection(isec);

    if (isec.didIntersect)
    {
        isec.renderer = this;
        vec3 tmp_colour = isec.material->Shade(isec, indirectLight);
        colour += tmp_colour * sampleWeight;
    }
  }
  return colour;
}

vec3 Raytracer::CastAtPixel(const int x, const int y)
{
  int superSamples = 3; //In each dimension
  float sampleSize = 1/(float)superSamples;
  //while(glm::length(delta) > threshold)
  float sampleWeight = 1.0f / (superSamples * superSamples);
  vec3 colour;
  for( int i=0; i<superSamples; i++)
  {
    for( int j=0; j<superSamples; j++)
    {
      float r1 = -(sampleSize/2) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/sampleSize));
      float r2 = -(sampleSize/2) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/sampleSize));
      vec3 d((x + sampleSize*(i+0.5f) + r1 - width / 2.0f), 
                -(y + sampleSize*(j+0.5f) + r2 - height / 2.0f), focalLength);

      //d = glm::normalize(d);
      d /= focalLength;

      //d = d*camera.r_y*camera.R_x;
      Ray ray = {camera.pos, d};
      colour += CastRay(ray) * sampleWeight;
    }
  }
  //return average;
  return colour;
}


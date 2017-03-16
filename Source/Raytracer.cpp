#include "Raytracer.h"
#include "Light.h"
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include "Object.h"
#include <SDL.h>
#include "Material.h"
using glm::vec3;


Raytracer::Raytracer(SDL_Surface* screen, Scene& scene, int dofSamples) : Renderer(screen, scene), focalLength(width), indirectLight( 0.5f, 0.5f, 0.5f), dofSamples(dofSamples)
{
  srand (static_cast <unsigned> (time(0)));
}

Intersection Raytracer::ClosestIntersection(Ray ray) 
{
  /*
  Intersection closest;
  closest.distance = std::numeric_limits<float>::max();
  for (unsigned i=0; i<scene.objects.size(); i++)
  {
    Intersection isec = scene.objects[i]->Intersect(ray);
    if (isec.didIntersect && (isec.distance <= closest.distance))
    {
      closest = isec;
    }
  }
  return closest;
  */
  return scene.tree->ClosestIntersection(ray);
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
  Ray new_ray = ray;
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
    new_ray.direction = scene.camera.transform_c2w_rotate(point - randomise);
    new_ray.origin = ray.origin + randomise;

    Intersection isec = ClosestIntersection(new_ray);

    if (isec.didIntersect)
    {
        //vec3 tmp_colour = inter.colour;
        //tmp_colour *= 0.75f*(DirectLight(inter, scene.objects)+indirectLight);
        vec3 tmp_colour = isec.material->Shade(isec, indirectLight, scene.lights, scene.tree);
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

      //d = d*scene.camera.r_y*scene.camera.R_x;
      Ray ray(scene.camera.pos, d);
      colour += CastRay(ray) * sampleWeight;
    }
  }
  //return average;
  return colour;
}


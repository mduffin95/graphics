#include "Raytracer.h"
#include "Light.h"
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
using glm::vec3;

#define PI 3.141592653f

Raytracer::Raytracer(SDL_Surface* screen, Camera &camera, vector<Light>& lights, vector<shared_ptr<IObject>>& objects, int dofSamples) : Renderer(screen, camera, lights, objects), focalLength(width), indirectLight( 0.5f, 0.5f, 0.5f), dofSamples(dofSamples)
{
  srand (static_cast <unsigned> (time(0)));
}


Intersection Raytracer::ShadowIntersection(
  Ray ray, //Needs to be a ray from the point to the light (not normalised) 
  const vector<shared_ptr<IObject>>& objects,
  const IObject *exclude)
{
  for (unsigned i=0; i<objects.size(); i++)
  {
    if (objects[i].get() == exclude) //Prevents self-intersections
      continue;
    Intersection isec = objects[i]->Intersect(ray);
    if (isec.didIntersect && (isec.distance <= 1))
      return isec;
  }
  return Intersection();
}

Intersection Raytracer::ClosestIntersection(
  Ray ray,  
  const vector<shared_ptr<IObject>>& objects)
{
  Intersection closest;
  closest.distance = numeric_limits<float>::max();
  for (unsigned i=0; i<objects.size(); i++)
  {
    Intersection isec = objects[i]->Intersect(ray);
    if (isec.didIntersect && (isec.distance <= closest.distance))
    {
      closest = isec;
    }
  }
  return closest;
}

vec3 perp_vec(vec3 in, float radius)
{
  float a = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));
  float b = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));

  float c = -(a*in.x + b*in.y) / in.z;

  vec3 v(a, b, c);
  v = normalize(v);
  v *= radius;
  return v;
}

vec3 Raytracer::DirectLight( const Intersection& isec, const vector<shared_ptr<IObject>>& objects )
{
  vec3 illuminationColour(0,0,0);
  for (unsigned i=0; i<lights.size(); i++)
  {
    vec3 r = lights[i].pos - isec.pos;

    //Number of feeler rays that don't hit obstacles
    int count = 0;
    for (int s=0; s<SHADOW_SAMPLES; s++)
    {
      vec3 p = perp_vec(r, lights[i].radius);       
      Ray ray = {isec.pos, r + p};
      
      Intersection shadow = ShadowIntersection(ray, objects, isec.object);
      if (!shadow.didIntersect) {
        count++;
      }
    }

    vec3 r_normal = normalize(r);

    float max1 =  max((float)dot(isec.normal , r_normal),0.0f);

    illuminationColour += (count / (float) SHADOW_SAMPLES) * max1 * lights[i].colour / ( 4.0f * powf(r.length(),2) * PI );

  }
  return illuminationColour;
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
    new_ray.direction = camera.transform_c2w_rotate(point - randomise);
    new_ray.origin = ray.origin + randomise;

    //TODO: Don't pass objects as it's available as a member variable
    Intersection inter = ClosestIntersection(new_ray, objects);

    if (inter.didIntersect)
    {
        vec3 tmp_colour = inter.colour;
        tmp_colour *= 0.75f*(DirectLight(inter, objects)+indirectLight);
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


#include "Raytracer.h"
#include "Lighting.h"
#include <cstdlib>
#include <ctime>

#define PI 3.141592653f

Raytracer::Raytracer(SDL_Surface* screen, Camera &camera, Lighting &lighting, vector<Triangle>& triangles, int dofSamples) : Renderer(screen, camera, lighting, triangles), lightPos( 0, -0.5, -0.7 ), lightColour(70, 70, 70), focalLength(width), indirectLight( 0.5f, 0.5f, 0.5f), type(type), dofSamples(dofSamples)
{
  srand (static_cast <unsigned> (time(0)));
}

bool Raytracer::CheckIntersection( const float u, const float v )
{
    if (u >= 0 &&
        v >= 0 &&
        u + v <= 1)
    {
      return true;
    }
    return false;
}


bool Raytracer::ClosestIntersection(
  Ray ray,  
  const vector<Triangle>& triangles,
  Intersection& closestIntersection , int index )
{
  bool result = false;
  for (unsigned i=0; i<triangles.size(); i++)
  {
    vec3 v0 = triangles[i].v0;
    vec3 v1 = triangles[i].v1;
    vec3 v2 = triangles[i].v2;

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 b = ray.origin - v0;

    mat3 A( -ray.direction, e1, e2 );
    vec3 x = glm::inverse( A ) * b;      
    float t = x.x;
    float u = x.y;
    float v = x.z;
		//Changed t < d to t <= d, to fix shadow bug
    if (t <= closestIntersection.distance &&
        t >= 0 &&
        CheckIntersection( u, v) &&
				i != index )
    {
      closestIntersection.distance = t;
      closestIntersection.position = ray.origin + t * ray.direction;
      closestIntersection.triangleIndex = i;
      result = true;
    }
  }
  return result;
}

vec3 Raytracer::DirectLight( const Intersection& i, const vector<Triangle>& triangles ){

	vec3 r = lightPos - i.position ;
	vec3 r_normal = normalize(r);
	float light_distance  = r.length();

	//Check closest intersection between camera position
	Intersection lightIntersection;
	lightIntersection.distance = 1;
  Ray ray = {i.position, r};
	if(ClosestIntersection(ray,triangles,lightIntersection, i.triangleIndex)){
		return vec3(0,0,0);
	}

	float max1 =  max((float)dot(triangles[i.triangleIndex].normal , r_normal),0.0f);

	vec3 illuminationColour = max1 * lightColour / ( 4.0f * powf(light_distance,2) * PI )  ;

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
      //vec3 color( 1.0, 0.0, 0.0 );
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
  float sampleWeight = 1/dofSamples;
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

    Intersection inter;
    inter.distance = numeric_limits<float>::max();

    if (ClosestIntersection(new_ray, triangles, inter, -1))
    {
        vec3 tmp_colour = triangles[inter.triangleIndex].color;
        tmp_colour *= 0.75f*(DirectLight(inter, triangles)+indirectLight);
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

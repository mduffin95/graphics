#include "Raytracer.h"
#include "Lighting.h"
#include <cstdlib>
#include <ctime>

#define PI 3.141592653f

Raytracer::Raytracer(SDL_Surface* screen, Camera &camera, Lighting &lighting, vector<Triangle>& triangles, RenderType type) : Renderer(screen, camera, lighting, triangles), lightPos( 0, -0.5, -0.7 ), 
    lightColour(70, 70, 70), focalLength(width), indirectLight( 0.5f, 0.5f, 0.5f), type(type)
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
  vec3 start,
  vec3 dir,
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
    vec3 b = start - v0;

    mat3 A( -dir, e1, e2 );
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
      closestIntersection.position = start + t * dir;
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
	if(ClosestIntersection(i.position,r,triangles,lightIntersection, i.triangleIndex)){
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
      if(type == Normal)
        colour = NormalRay(x, y);
      if(type == DepthOfField)
        colour = DOFRay(x, y);
      //vec3 color( 1.0, 0.0, 0.0 );
      PutPixelSDL( screen, x, y, colour );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

vec3 Raytracer::DOFRay(const int x, const int y)
{
  float aperture_radius = 0.05;
  float focalDepth = 2;
  //float threshold = 0.0002;
  int num = 0;
  vec3 delta = vec3(255, 255, 255);
  vec3 colour = vec3(0,0,0);
  vec3 average = colour;
  //while(glm::length(delta) > threshold)
  for( int i=0; i<2; i++)
  {
    for( int j=0; j<2; j++)
    {
      float r1 = -0.25 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/0.5f));
      float r2 = -0.25 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/0.5f));
      float r3 = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));
      float r4 = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));
      vec3 point((x + 0.25f + i/2.0f + r1 - width / 2.0f) / focalLength, 
                -(y + 0.25f + j/2.0f + r2 - height / 2.0f) / focalLength, 1);
      point *= focalDepth;
      vec3 rand_eye = vec3(r3*aperture_radius, r4*aperture_radius, 0);//Get random point
      vec3 d = point - rand_eye;
      d = camera.transform_c2w_rotate(d);
      rand_eye = camera.transform_c2w_rotate(rand_eye) + camera.pos;

      //d = d*camera.r_y*camera.R_x;
      
      Intersection inter;
      inter.distance = numeric_limits<float>::max();
      vec3 col_tmp;
      if (ClosestIntersection(rand_eye, d, triangles, inter, -1))
      {
          col_tmp = triangles[inter.triangleIndex].color;
          col_tmp *= 0.75f*(DirectLight(inter, triangles)+indirectLight);
      }
      else
      { 
          col_tmp = vec3(0, 0, 0);
      }
      colour += col_tmp;
      num++;
      //delta = colour / (float)num - average;
      //average = colour / (float)num;
    }
  }
  //return average;
  return colour / (float)num;
}

vec3 Raytracer::NormalRay(const int x, const int y)
{
  vec3 d(x - width / 2.0f, -(y - height / 2.0f), focalLength);

  //d = d*camera.r_y*camera.R_x;
  d = camera.transform_c2w_rotate(d);

  Intersection inter;
  inter.distance = numeric_limits<float>::max();
  vec3 colour;
  if (ClosestIntersection(camera.pos, d, triangles, inter, -1))
  {
      colour = triangles[inter.triangleIndex].color;
      colour *= 0.75f*(DirectLight(inter, triangles)+indirectLight);
  }
  else
  { 
      colour = vec3(0, 0, 0);
  }
  return colour;
}

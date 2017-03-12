#include "Material.h"
#include "Intersection.h"
#include "Ray.h"
#include <vector>

#define PI 3.141592653f

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

Intersection ShadowIntersection(
  Ray ray, //Needs to be a ray from the point to the light (not normalised) 
  const std::vector<std::shared_ptr<Object>>& objects,
  const Object *exclude)
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

vec3 DirectLight( const Intersection& isec, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects )
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

    float max1 =  std::max((float)dot(isec.normal , r_normal),0.0f);

    illuminationColour += (count / (float) SHADOW_SAMPLES) * max1 * lights[i].colour / ( 4.0f * powf(r.length(),2) * PI );

  }
  return illuminationColour;
}

vec3 DefaultMat::Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const
{
  //(DirectLight(inter, objects)+indirectLight)
  return colour * DirectLight(isec, lights, objects) * indirectLight;
}

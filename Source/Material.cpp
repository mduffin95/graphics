#include "Material.h"
#include "Intersection.h"
#include "Ray.h"
#include <vector>
#include <iostream>

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

vec3 Material::DirectLight( const Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects, float Kd, float Ks) const
{
  vec3 i_amb = indirectLight * colour;
  vec3 i_diff(0,0,0);
  vec3 i_spec(0,0,0);

  vec3 v = normalize(isec.ray.direction); //Might need to be negative
  vec3 n = normalize(isec.normal);

  for (unsigned i=0; i<lights.size(); i++)
  {
    vec3 l = lights[i].pos - isec.pos;

    //Number of feeler rays that don't hit obstacles
    int count = 0;
    vec3 p(0,0,0);
    for (int s=0; s<SHADOW_SAMPLES; s++)
    {
    
      if (SHADOW_SAMPLES > 1)
      {
        p = perp_vec(l, lights[i].radius);       
      }

      Ray ray = {isec.pos, l + p};
      
      Intersection shadow = ShadowIntersection(ray, objects, isec.object);
      if (!shadow.didIntersect) {
        count++;
      }
    }

    float dot = glm::dot(isec.normal , normalize(l));

    if (dot > 0)
    {
      i_diff += (count / (float) SHADOW_SAMPLES) * dot * lights[i].colour / ( 4.0f * powf(l.length(),2) * PI );
    }

    vec3 l_norm = normalize(l);
    vec3 r = l_norm - 2.0f * glm::dot(l_norm, n) * n; 
    dot = glm::dot(r, v);
    if (dot > 0)
    {
      //TODO: Break distance attenuation out into method of Light
      //TODO: Light colour should be less than one in all dimensions. Add an intensity.
      i_spec += (count / (float) SHADOW_SAMPLES) * powf(dot, 20) * lights[i].colour / ( 4.0f * powf(l.length(),2) * PI );
    }
  }
  i_diff *= colour;
  return i_amb + i_diff * Kd + i_spec * Ks;
}

vec3 DefaultMat::Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const
{
  //(DirectLight(inter, objects)+indirectLight)
  return DirectLight(isec, indirectLight, lights, objects, 1.0f, 0.0f);
}

vec3 Phong::Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const
{
  return DirectLight(isec, indirectLight, lights, objects, 1.0f, 1.0f);
}
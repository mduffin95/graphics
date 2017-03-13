#include "Material.h"
#include "Intersection.h"
#include "Object.h"
#include "Ray.h"
#include <vector>
#include <iostream>





vec3 Material::DirectLight( const Intersection& isec, vec3 indirectLight, float Kd, float Ks) const
{
  vec3 i_amb = indirectLight * colour;
  vec3 i_diff(0,0,0);
  vec3 i_spec(0,0,0);

  vec3 v = normalize(isec.ray.direction); //Might need to be negative
  vec3 n = normalize(isec.normal);

  for (unsigned i=0; i<isec.renderer->lights.size(); i++)
  {
    vec3 light_colour = isec.renderer->lights[i].GetColour(isec);

    vec3 l = isec.renderer->lights[i].pos - isec.pos;
    float dot = glm::dot(isec.normal , normalize(l));

    if (dot > 0)
    {
      i_diff +=  light_colour * dot;
    }

    vec3 l_norm = normalize(l);
    vec3 r = l_norm - 2.0f * glm::dot(l_norm, n) * n; 
    dot = glm::dot(r, v);
    if (dot > 0)
    {
      //TODO: Light colour should be less than one in all dimensions. Add an intensity.
      i_spec += light_colour * powf(dot, 20);
    }
  }
  i_diff *= colour;
  return i_amb + i_diff * Kd + i_spec * Ks;
}

vec3 DefaultMat::Shade(Intersection& isec, vec3 indirectLight) const
{
  //(DirectLight(inter, objects)+indirectLight)
  return DirectLight(isec, indirectLight, 1.0f, 0.0f);
}

vec3 Phong::Shade(Intersection& isec, vec3 indirectLight) const
{
  return DirectLight(isec, indirectLight, 1.0f, 1.0f);
}

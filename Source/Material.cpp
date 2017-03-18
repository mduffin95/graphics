#include "Material.h"
#include "Intersection.h"
#include "Ray.h"
#include <vector>
#include <iostream>
#include <random>

#define PI 3.141592653f
#define MAX_DEPTH 1
#define SHADOW_SAMPLES 10 

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

void Material::DirectLight( const Intersection& isec, const std::vector<Light>& lights, KDNode *tree, vec3 &diff, vec3 &spec) const
{
  diff = vec3(0,0,0);
  spec = vec3(0,0,0);

  vec3 v = normalize(isec.ray.direction); //Might need to be negative
  vec3 n = normalize(isec.normal);

  for (unsigned i=0; i<lights.size(); i++)
  {
    vec3 l = lights[i].pos - isec.pos;

    //Number of feeler rays that don't hit obstacles
    int count = 0;
    vec3 p(0,0,0);
#pragma omp parallel for reduction(+:count)
    for (int s=0; s<SHADOW_SAMPLES; s++)
    {
    
      if (SHADOW_SAMPLES > 1)
      {
        p = perp_vec(l, lights[i].radius);       
      }

      Ray ray(isec.pos, l + p);
      
      Intersection shadow = tree->ShadowIntersection(ray, isec.object);
      if (!shadow.didIntersect) {
        count++;
      }
    }

    float dot = glm::dot(isec.normal , normalize(l));

    if (dot > 0)
    {
      diff += (count / (float) SHADOW_SAMPLES) * dot * lights[i].GetColour(l.length()) / PI;
    }

    vec3 l_norm = normalize(l);
    vec3 r = l_norm - 2.0f * glm::dot(l_norm, n) * n; 
    dot = glm::dot(r, v);
    if (dot > 0)
    {
      spec += (count / (float) SHADOW_SAMPLES) * powf(dot, shi) * lights[i].GetColour(l.length());
    }
  }
}

vec3 DefaultMat::Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth /*=0*/) const
{
  //(DirectLight(inter, objects)+indirectLight)
  vec3 diff, spec;
  vec3 amb = indirectLight * colour;
  DirectLight(isec, lights, tree, diff, spec);
  return amb + diff * Kd * colour / PI + spec * Ks;
}

vec3 Phong::Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth /*=0*/) const
{
  vec3 diff, spec;
  vec3 amb = indirectLight * colour;
  DirectLight(isec, lights, tree, diff, spec);
  return amb + diff * Kd * colour / PI + spec * Ks;
}

void create_coord_system(const vec3 &N, vec3 &Nt, vec3 &Nb)
{
  //Need to make sure this is a left-hand coordinate system (or do we?)
  if (std::fabs(N.x) > std::fabs(N.y))
    Nt = glm::normalize(vec3(N.z, 0, -N.x));
  else
    Nt = glm::normalize(vec3(0, -N.z, N.y));
  Nb = glm::cross(N, Nt);
}

vec3 uniform_sample_hemisphere(const float &r1, const float &r2)
{
  float sin_theta = sqrtf(1 - r1*r2);
  float phi = 2 * PI * r2;
  float x = sin_theta * cosf(phi);
  float z = sin_theta * sinf(phi);
  return vec3(x, r1, z);
}

//Scratchapixel inspired
vec3 Global::Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth /*=0*/) const
{
  if (depth > MAX_DEPTH)
    return vec3(0);
  vec3 diff, spec;
  DirectLight(isec, lights, tree, diff, spec);

  vec3 Nt, Nb;
  create_coord_system(isec.normal, Nt, Nb);
  
  std::default_random_engine generator; 
  std::uniform_real_distribution<float> distribution(0, 1);
  int samples = 16;
  float ind_x = 0.0f;
  float ind_y = 0.0f;
  float ind_z = 0.0f;
#pragma omp parallel for reduction(+:ind_x,ind_y,ind_z)
  for (int i=0; i<samples; i++)
  {
    float r1 = distribution(generator);
    float r2 = distribution(generator);
    vec3 sample = uniform_sample_hemisphere(r1, r2);
    vec3 sample_world(
        sample.x * Nb.x + sample.y * isec.normal.x + sample.z * Nt.x,
        sample.x * Nb.y + sample.y * isec.normal.y + sample.z * Nt.y,
        sample.x * Nb.z + sample.y * isec.normal.z + sample.z * Nt.z);
    Ray ray(isec.pos, sample_world);
    Intersection tmp_isec = tree->ClosestIntersection(ray);
    if (tmp_isec.didIntersect)
    {
      vec3 hit = r1 * tmp_isec.material->Shade(tmp_isec, indirectLight, lights, tree, depth+1); // 
      ind_x += hit.x;
      ind_y += hit.y;
      ind_z += hit.z;
    }
  }
  vec3 indirect(ind_x, ind_y, ind_z);
  indirect /= samples;
  return (diff / PI + 2.0f * indirect) * colour * Kd; //indirectLight * colour + 
}

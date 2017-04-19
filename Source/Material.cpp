#include "Material.h"
#include "Intersection.h"
#include "Ray.h"
#include "Triangle.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#define PI 3.141592653f
#define MAX_DEPTH 1
#define SHADOW_SAMPLES 10 
#define GLOBAL_ILLUM_SAMPLES 32 

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
  
  float ind_x = 0.0f;
  float ind_y = 0.0f;
  float ind_z = 0.0f;
#pragma omp parallel for reduction(+:ind_x,ind_y,ind_z)
  for (int i=0; i<GLOBAL_ILLUM_SAMPLES; i++)
  {
    float r1 = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX));
    float r2 = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX));
    vec3 sample = uniform_sample_hemisphere(r1, r2);
    vec3 sample_world(
        sample.x * Nb.x + sample.y * isec.normal.x + sample.z * Nt.x,
        sample.x * Nb.y + sample.y * isec.normal.y + sample.z * Nt.y,
        sample.x * Nb.z + sample.y * isec.normal.z + sample.z * Nt.z);
    Ray ray(isec.pos, sample_world);
    Intersection tmp_isec = tree->ClosestIntersection(ray, isec.object);
    if (tmp_isec.didIntersect)
    {
      vec3 hit = r1 * tmp_isec.material->Shade(tmp_isec, indirectLight, lights, tree, depth+1);
      ind_x += hit.x;
      ind_y += hit.y;
      ind_z += hit.z;
    }
  }
  vec3 indirect(ind_x, ind_y, ind_z);
  indirect /= GLOBAL_ILLUM_SAMPLES;
  return (diff + 2.0f * indirect) * colour * Kd; //indirectLight * colour + 
}


vec3 Mirror::Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth /*=0*/) const
{
  if (depth > MAX_DEPTH)
    return vec3(0); 
  vec3 n = normalize(isec.normal);
  vec3 incident = normalize(isec.ray.direction);
  vec3 r = incident - 2.0f * glm::dot(incident, n) * n; 
  Ray ray(isec.pos, r);
  Intersection tmp_isec = tree->ClosestIntersection(ray, isec.object);
  vec3 hit(0);
  if (tmp_isec.didIntersect)
  {
    hit = 0.9f * tmp_isec.material->Shade(tmp_isec, indirectLight, lights, tree, depth+1);
  }
  return Ks * colour * hit; //Using Ks as our reflectance value
}


vec3 Glass::Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth /*=0*/) const {
  if (depth > MAX_DEPTH)
    return vec3(0);
  float ri = 0.9f; //N1 / N2, so less than 1 when going into glass
  vec3 n = normalize(isec.normal);
  vec3 incident = normalize(isec.ray.direction);

  //Reflection
  vec3 r = incident - 2.0f * glm::dot(incident, n) * n; 
  Ray ray(isec.pos, r);
  Intersection tmp_isec = tree->ClosestIntersection(ray, isec.object);
  vec3 reflection(0);
  if (tmp_isec.didIntersect)
  {
    reflection = tmp_isec.material->Shade(tmp_isec, indirectLight, lights, tree, depth+1);
  }

  //Refraction
  float n_dot_i = glm::dot(n, incident); //cosine
  float k = 1.0f - ri * ri * (1.0f - n_dot_i * n_dot_i);
  if (k < 0.0f)
  {
    //TODO: Only return reflection
    return(vec3(0));
  }
  vec3 r2 = ri * incident - (ri * n_dot_i + sqrtf(k)) * n;

  Ray ray2(isec.pos, r2);
  Intersection tmp_isec2 = tree->ClosestIntersection(ray2, isec.object);
  vec3 refraction(0);
  if (tmp_isec2.didIntersect)
  {
    refraction = tmp_isec2.material->Shade(tmp_isec2, indirectLight, lights, tree, depth+1);
  }
  return 0.1f * reflection + 0.9f * refraction * colour; //TODO: Fresnel effects to change the proportion of reflection and refraction
 
}


vec3 TextureMat::Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth /*= 0*/) const
{
  //Get texture coordinates from isec
  Triangle *tri = (Triangle*)isec.object;
  //Find barycentric coordinates of intersection point
  unsigned char * ptr = diffuseTexture(glm::vec2(isec.u, isec.v)); 
  //Get texture using diffuseTexture
  char b = *ptr;
  char g = *(ptr+1);
  char r = *(ptr+2);
  return glm::vec3(r, g, b);
}

unsigned char * TextureMat::diffuseTexture(glm::vec2 textureCoordinate) const {
  glm::ivec2 coordinate(textureCoordinate.x * diffuse.GetWidth(), textureCoordinate.y * diffuse.GetHeight());
  return diffuse.Get(coordinate[0],coordinate[1]);
}

TextureMat::TextureMat(const char *textureFile, const char *objFile, std::vector<Triangle>& triangles) : Material(vec3(0), 1.0f, 1.0f, 40)
{
  diffuse.ReadTGAImage(textureFile);
  LoadObj(objFile, triangles);
}

void TextureMat::LoadObj(const char *objFile, std::vector<Triangle>& triangles) //pass in a reference to triangle array
{
  std::vector<glm::vec3> vs;
  std::vector<glm::vec2> vts;
  std::vector<glm::vec3> vns;

  bool debug = false;

  std::ifstream ifs(objFile);
  std::string line;
  while (std::getline(ifs, line))
  {
    std::istringstream ss(line);
    ss.clear();

    if (line.compare(0, 2, "v ") == 0) //This is a vertex
    {
      ss.ignore();
      glm::vec3 v;
      for(int i=0; i<3; i++) ss >> v[i];

      vs.push_back(v);
    }
    if (line.compare(0, 3, "vt ") == 0) //This is a texture coordinate
    {
      ss.ignore(2);
      glm::vec2 vt;
      for(int i=0; i<2; i++) ss >> vt[i];

      vts.push_back(vt);
    }
    if (line.compare(0, 3, "vn ") == 0) //This is a vertex normal
    {
      ss.ignore(2);
      glm::vec3 vn;
      for(int i=0; i<3; i++) ss >> vn[i];
      vns.push_back(vn);
    }
    if (line.compare(0, 2, "f ") == 0) //something else
    {
      ss.ignore();

      int i = 0;

      glm::ivec3 v[3];
      //todo handle negative indices
      std::string token;
      while(getline(ss, token, ' '))
      {
        if (token == "") continue; //Bit of a hack
        if(debug) std::cout << token << std::endl;
        std::string value;
        int j=0;
        std::istringstream stoken(token);

        while(getline(stoken, value, '/'))
        {
          std::istringstream svalue(value);
          svalue >> v[i][j];
          if(debug) std::cout << v[i][j] << ",";
          v[i][j]--;
          j++;

        }
        if(debug) std::cout << std::endl;
        i++;
      }


      if(debug) std::cout << std::endl;
      Triangle triangle (vs[v[0][0]],vs[v[0][1]],vs[v[0][2]], 
                         vts[v[1][0]],vts[v[1][1]],vts[v[1][2]], this);
      triangles.push_back(triangle);
    }
  }
}


//Load triangles using loadObj
//Need to attach this material to the triangle
//Need to also load in the diffuse texture 

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Intersection.h"
#include <glm/glm.hpp>
#include "Light.h"
#include "Object.h"
#include <vector>
#include "KDNode.h"

#define SHADOW_SAMPLES 1 

using glm::vec3;

class Intersection;
class Object;
class KDNode;

class Material
{
protected:
  vec3 colour;
  float Kd, Ks;
  int shi; //specular shininess
  Material(vec3 colour, float Kd, float Ks, int shi)
    : colour(colour), Kd(Kd), Ks(Ks), shi(shi) {}
    
  vec3 DirectLight( const Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, KDNode *tree, float Kd, float Ks) const;

public:
  //virtual destructor
  virtual ~Material() {};

  virtual vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, KDNode *tree) const = 0;
};

class DefaultMat : public Material
{
public:
  DefaultMat(vec3 colour) : Material(colour, 1.0f, 0.0f, 0) {}

  vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, KDNode *tree) const override;
};

class Phong : public Material
{
public:
  Phong(vec3 colour) : Material(colour, 1.0f, 1.0f, 20) {}

  vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, KDNode *tree) const override;
};

class Global : public Material
{
public:
  Global(vec3 colour) : Material(colour, 1.0f, 1.0f, 20) {}

  vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, KDNode *tree) const override;
};


#endif

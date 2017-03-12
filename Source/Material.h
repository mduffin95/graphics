#ifndef MATERIAL_H
#define MATERIAL_H

#include "Intersection.h"
#include <glm/glm.hpp>
#include "Light.h"
#include "Object.h"
#include <memory>
#include <vector>

#define SHADOW_SAMPLES 1 

using glm::vec3;

class Intersection;
class Object;

class Material
{
public:

  //virtual destructor
  virtual ~Material() {};

  virtual vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const = 0;

};

class DefaultMat : public Material
{
public:
  vec3 colour;
  DefaultMat(vec3 colour) : colour(colour) {}

  vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const override;
};

#endif

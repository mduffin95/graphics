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
protected:
  vec3 colour;
  Material(vec3 colour) : colour(colour) {}
    
  vec3 DirectLight( const Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects, float Kd, float Ks) const;

public:
  //virtual destructor
  virtual ~Material() {};

  virtual vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const = 0;
};

class DefaultMat : public Material
{
public:
  vec3 colour;
  DefaultMat(vec3 colour) : Material(colour) {}

  vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const override;
};

class Phong : public Material
{
public:
  vec3 colour;
  Phong(vec3 colour) : Material(colour) {}

  vec3 Shade(Intersection& isec, vec3 indirectLight, const std::vector<Light>& lights, const std::vector<std::shared_ptr<Object>>& objects) const override;
};

#endif

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Intersection.h"
#include <glm/glm.hpp>
#include "Light.h"
#include "RenderableObject.h"
#include <vector>
#include "KDNode.h"


using glm::vec3;

class Intersection;
class RenderableObject;
class KDNode;

class Material
{
protected:
  vec3 colour;
  float Kd, Ks;
  int shi; //specular shininess
  Material(vec3 colour, float Kd, float Ks, int shi)
    : colour(colour), Kd(Kd), Ks(Ks), shi(shi) {}
    
  void DirectLight( const Intersection& isec, const std::vector<Light>& lights, KDNode *tree, vec3 &diff, vec3 &spec) const;

public:
  //virtual destructor
  virtual ~Material() {};

  virtual vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth) const = 0;
  vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree) const
  {
    return Shade(isec, indirectLight, lights, tree, 0);
  }
};

class DefaultMat : public Material
{
public:
  DefaultMat(vec3 colour) : Material(colour, 1.0f, 0.0f, 0) {}

  vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth = 0) const override;
};

class Phong : public Material
{
public:
  Phong(vec3 colour) : Material(colour, 1.0f, 1.0f, 20) {}

  vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth = 0) const override;
};

class Global : public Material
{
public:
  Global(vec3 colour) : Material(colour, 1.0f, 1.0f, 20) {}

  vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth = 0) const override;
};

class Mirror : public Material
{
public:
  Mirror(vec3 colour) : Material(colour, 1.0f, 1.0f, 40) {}

  vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth = 0) const override;
};

class Glass : public Material
{
public:
  Glass(vec3 colour) : Material(colour, 1.0f, 1.0f, 40) {}

  vec3 Shade(Intersection& isec, vec3& indirectLight, const std::vector<Light>& lights, KDNode *tree, unsigned depth = 0) const override;
};

#endif

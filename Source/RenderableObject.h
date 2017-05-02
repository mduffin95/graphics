#ifndef OBJECT_H
#define OBJECT_H

#include "Ray.h"
#include "Intersection.h"
#include "Material.h"
#include "KDNode.h"
#include <memory>

class Intersection;
class Material;

class RenderableObject {

public:

  RenderableObject(Material* material) : material(material) {}
  //virtual destructor
  virtual ~RenderableObject() {};

  //Something for checking intersections
  virtual Intersection Intersect(Ray ray) const = 0;
  virtual int CheckPlaneIntersection(int axis, float value) const = 0;
  virtual float GetLeftExtreme(int axis) const = 0;
  virtual float GetRightExtreme(int axis) const = 0;

  AABB GetAABB()
  {
    float left0 = GetLeftExtreme(0);
    float left1 = GetLeftExtreme(1);
    float left2 = GetLeftExtreme(2);
    float right0 = GetRightExtreme(0);
    float right1 = GetRightExtreme(1);
    float right2 = GetRightExtreme(2);
    return AABB(vec3(left0,left1,left2), vec3(right0,right1,right2));
  }
  //These are a bit of a hack to get the rasteriser part to compile
  //TODO: Bounding box
protected:
  Material* material;
};

#endif

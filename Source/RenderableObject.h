#ifndef OBJECT_H
#define OBJECT_H

#include "Ray.h"
#include "Intersection.h"
#include "Material.h"
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

  //These are a bit of a hack to get the rasteriser part to compile
  //TODO: Bounding box
protected:
  Material* material;
};

#endif

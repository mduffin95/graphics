#ifndef OBJECT_H
#define OBJECT_H

#include "Ray.h"
#include "Intersection.h"

class IObject {

public:
  //Something for checking intersections
  virtual Intersection Intersect(Ray ray) const = 0;

  //These are a bit of a hack to get the rasteriser part to compile
  virtual vec3 get_v0() const = 0;
  virtual vec3 get_v1() const = 0;
  virtual vec3 get_v2() const = 0;

  virtual vec3 get_colour() const = 0;
  //TODO: Bounding box
};

#endif

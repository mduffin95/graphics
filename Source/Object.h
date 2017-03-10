#ifndef OBJECT_H
#define OBJECT_H

#include "Ray.h"
#include "Intersection.h"

class Object {

public:
  //Something for checking intersections
  virtual Intersection Intersect(Ray ray) const = 0;

  //TODO: Bounding box
};

#endif

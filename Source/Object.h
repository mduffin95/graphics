#ifndef OBJECT_H
#define OBJECT_H

#include "Ray.h"
#include "Intersection.h"

class Object {

public:
  //Something for checking intersections
  virtual Intersection intersect(Ray ray) = 0;

  //TODO: Bounding box
};

#endif

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "IObject.h" //There is a circular dependency so this is not included

using glm::vec3;
class IObject;

class Intersection {
public:
  Intersection() 
    : didIntersect(false), distance(1), object(NULL) {}
  bool didIntersect;
  vec3 pos;
  vec3 normal; //Normal of the surface at the intersection point
  vec3 colour;
  float distance;
  const IObject *object;

};

#endif

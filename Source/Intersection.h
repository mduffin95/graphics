#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "RenderableObject.h" //There is a circular dependency so this is not included
#include "Material.h"

using glm::vec3;
class RenderableObject;

class Intersection {
public:
  Intersection() 
    : didIntersect(false), distance(1), object(NULL) {}
  bool didIntersect;
  vec3 pos;
  vec3 normal; //Normal of the surface at the intersection point
  Ray ray;
  float distance;
  const RenderableObject *object;
  Material* material;
  float u, v, t;

};

#endif

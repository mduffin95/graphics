#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "Object.h" //There is a circular dependency so this is not included
#include "Material.h"

using glm::vec3;
class Object;

class Intersection {
public:
  Intersection() 
    : didIntersect(false), distance(1), object(NULL) {}
  bool didIntersect;
  vec3 pos;
  vec3 normal; //Normal of the surface at the intersection point
  float distance;
  const Object *object;
  std::shared_ptr<Material> material;

};

#endif

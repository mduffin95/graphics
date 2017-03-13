#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "Ray.h"
#include "Renderer.h"

using glm::vec3;
class Object;
class Material;
class Renderer;

class Intersection {
public:
  Intersection() 
    : didIntersect(false), distance(1), object(NULL) {}
  bool didIntersect;
  vec3 pos;
  vec3 normal; //Normal of the surface at the intersection point
  Ray ray;
  float distance; //TODO: Get rid of this
  const Object *object;
  const Renderer *renderer;
  std::shared_ptr<Material> material;

};

#endif

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "Triangle.h" //There is a circular dependency so this is not included
using glm::vec3;
class Triangle;

class Intersection {
public:
  Intersection() 
    : triangle(NULL), didIntersect(false), distance(1) {}
  bool didIntersect;
  vec3 pos;
  float distance;
  const Triangle *triangle;

};

#endif

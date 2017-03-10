#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "Triangle.h" //There is a circular dependency so this is not included
using glm::vec3;
class Triangle;

class Intersection {
public:
  Intersection() 
    : didIntersect(false), pos(vec3(0,0,0)), distance(1), triangle(NULL) {}
  bool didIntersect;
  vec3 pos;
  float distance;
  const Triangle *triangle;

};

#endif

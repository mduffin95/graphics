#ifndef TYPES_H
#define TYPES_H

#include <glm/glm.hpp>

struct Intersection
{
  vec3 position;
  float distance;
  int triangleIndex;
};

struct Ray
{
  vec3 origin;
  vec3 direction;
};


#endif

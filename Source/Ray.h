#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
using glm::vec3;

struct Ray
{
  vec3 origin;
  vec3 direction;
  vec3 dirfrac;

  Ray() : origin(vec3(0,0,0)), direction(vec3(0,0,0)) {}
  Ray(vec3 origin, vec3 direction) : origin(origin), direction(direction)
  {
    vec3 rdir = normalize(direction);
    dirfrac.x = 1.0f / rdir.x;
    dirfrac.y = 1.0f / rdir.y;
    dirfrac.z = 1.0f / rdir.z;
  }
};


#endif

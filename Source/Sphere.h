#ifndef SPHERE_H
#define SPHERE_H
#include <glm/glm.hpp>
#include "Object.h"

class Sphere : public Object
{
public:
  glm::vec3 position;
  float radius;
	float radius2;

  Intersection Intersect(Ray) const override;  
  int CheckPlaneIntersection(int axis, float value) const override;
  float GetLeftExtreme(int axis) const override;
  float GetRightExtreme(int axis) const override;


  Sphere(glm::vec3 position, float radius, Material* material);
};

#endif

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "RenderableObject.h"
class Triangle : public RenderableObject
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material* material);
	void ComputeNormal();

  Intersection Intersect(Ray) const override;  
  int CheckPlaneIntersection(int axis, float value) const override;
  float GetLeftExtreme(int axis) const override;
  float GetRightExtreme(int axis) const override;
};

#endif

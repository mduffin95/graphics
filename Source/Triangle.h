#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "Object.h"
class Triangle : public Object
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material);
	void ComputeNormal();

  Intersection Intersect(Ray) const override;  
  vec3 get_v0() const override;
  vec3 get_v1() const override;
  vec3 get_v2() const override;

  vec3 get_colour() const override;
};

#endif
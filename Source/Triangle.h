#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "RenderableObject.h"
class Triangle : public RenderableObject
{
public:
  //Vertices
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
  //Texture coordinates
  glm::vec2 vt0;
  glm::vec2 vt1;
  glm::vec2 vt2;

	glm::vec3 normal;
	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material* material);
	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, 
            glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, Material* material);
	void ComputeNormal();

  Intersection Intersect(Ray) const override;  
  int CheckPlaneIntersection(int axis, float value) const override;
  float GetLeftExtreme(int axis) const override;
  float GetRightExtreme(int axis) const override;
};

#endif

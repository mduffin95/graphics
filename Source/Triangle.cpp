#include "Triangle.h"
#include <glm/glm.hpp>
using glm::vec3;
using glm::mat3;

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
:v0(v0), v1(v1), v2(v2), color(color){
	ComputeNormal();
}

void Triangle::ComputeNormal()
{
	glm::vec3 e1 = v1-v0;
	glm::vec3 e2 = v2-v0;
	normal = glm::normalize( glm::cross( e2, e1 ) );
}

Intersection Triangle::Intersect(Ray ray) const
{
  Intersection result;

  vec3 e1 = v1 - v0;
  vec3 e2 = v2 - v0;
  vec3 b = ray.origin - v0;

  mat3 A( -ray.direction, e1, e2 );
  vec3 x = glm::inverse( A ) * b;      
  float t = x.x;
  float u = x.y;
  float v = x.z;
  //Changed t < d to t <= d, to fix shadow bug
  if (
      t >= 0 &&
      u >= 0 &&
      v >= 0 &&
      u + v <= 1)
  {
    result.distance = t;
    result.pos = ray.origin + t * ray.direction;
    result.triangle = this;
    result.didIntersect = true;
  }
  return result;

}


#include "Triangle.h"
#include <glm/glm.hpp>
#include <math.h>
using glm::vec3;
using glm::mat3;

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material* material) : RenderableObject(material), v0(v0), v1(v1), v2(v2) {
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, Material* material) : RenderableObject(material), v0(v0), v1(v1), v2(v2), vt0(vt0), vt1(vt1), vt2(vt2) {
	ComputeNormal();
}

void Triangle::ComputeNormal()
{
	glm::vec3 e1 = v2-v0;
	glm::vec3 e2 = v1-v0;
	normal = glm::normalize( glm::cross( e2, e1 ) );
}

Intersection Triangle::Intersect(Ray ray) const
{
  Intersection result;
  float t, u, v;
  vec3 e1 = v1 - v0;
  vec3 e2 = v2 - v0;
  vec3 pvec = cross(ray.direction, e2);
  float det = dot(e1, pvec);
  float invdet = 1 / det;
  if (fabsf(det) < 0.001f) return result;
  
  vec3 tvec = ray.origin - v0;
  u = dot(tvec, pvec);
  if (u<0 || u>det) return result;

  vec3 qvec = cross(tvec, e1);
  v = dot(ray.direction, qvec);
  if (v<0 || u+v>det) return result;

  t = dot(e2, qvec);
  if (t < 0) return result;
  
  t *= invdet;
  result.distance = t;
  result.pos = ray.origin + t * ray.direction;
  result.normal = normal;
  result.object = this;
  result.material = material;
  result.ray = ray;
  result.didIntersect = true;
  result.u = u;
  result.v = v;
  result.t = t;
  return result;
}

int Triangle::CheckPlaneIntersection(int axis, float value) const
{
  if (v0[axis] < value && v1[axis] < value && v2[axis] < value)
    return -1;
  if (v0[axis] > value && v1[axis] > value && v2[axis] > value)
    return 1;
  return 0;
}


float Triangle::GetLeftExtreme(int axis) const
{
  return std::min(std::min(v0[axis], v1[axis]), v2[axis]);
}

float Triangle::GetRightExtreme(int axis) const
{
  return std::max(std::max(v0[axis], v1[axis]), v2[axis]);
}

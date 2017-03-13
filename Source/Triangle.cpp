#include "Triangle.h"
#include <glm/glm.hpp>
using glm::vec3;
using glm::mat3;

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material* material) : Object(material), v0(v0), v1(v1), v2(v2) {
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
  float t, u, v;
  vec3 e1 = v2 - v0;
  vec3 e2 = v1 - v0;
  vec3 pvec = cross(ray.direction, e2);
  float det = dot(e1, pvec);
  float invdet = 1 / det;
  if (det < 0.01f) return result;
  
  //Not doing any culling yet
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
  return result;
}

vec3 Triangle::get_v0() const
{
  return v0;
}

vec3 Triangle::get_v1() const
{
  return v1;
}

vec3 Triangle::get_v2() const
{
  return v2;
}

vec3 Triangle::get_colour() const
{
  //TODO: This is bad, change it
  return vec3(0,0,0);
}

#include "Sphere.h"
#include <glm/glm.hpp>
#include "Ray.h"
using glm::vec3;
using glm::mat3;

Sphere::Sphere(glm::vec3 position, float radius, Material* material) : Object(material), position(position), radius(radius), radius2(radius*radius) {}


//Makes heavy use of scratchapixel code
//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool SolveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
  float disc = b*b - 4*a*c;
  if (disc < 0)
    return false;
  else if (disc == 0)
    x0 = x1 = -0.5 * b / a;
  else
  {
    float q = (b > 0) ?
      -0.5 * (b * sqrt(disc)) :
      -0.5 * (b - sqrt(disc));
    x0 = q/a;
    x1 = c/q;
  }
  if (x0 > x1) std::swap(x0, x1);
  return true;
}

Intersection Sphere::Intersect(Ray ray) const 
{ 
        float t0, t1; // solutions for t if the ray intersects 
#if 0 
        // geometric solution
        Vec3f L = center - orig; 
        float tca = L.dotProduct(dir); 
        // if (tca < 0) return false;
        float d2 = L.dotProduct(L) - tca * tca; 
        if (d2 > radius2) return false; 
        float thc = sqrt(radius2 - d2); 
        t0 = tca - thc; 
        t1 = tca + thc; 
#else 
        // analytic solution
        vec3 L = ray.origin - position; 
        float a = glm::dot(ray.direction, ray.direction); 
        float b = 2 * glm::dot(ray.direction, L); 
        float c = glm::dot(L, L) - radius2; 
        if (!SolveQuadratic(a, b, c, t0, t1)) return Intersection(); 
#endif 
        if (t0 > t1) std::swap(t0, t1); 
 
        if (t0 < 0) { 
            t0 = t1; // if t0 is negative, let's use t1 instead 
            if (t0 < 0) return Intersection(); // both t0 and t1 are negative 
        } 
 
 
				Intersection result;
				result.distance = t0;
				result.pos = ray.origin + t0 * ray.direction;
				result.normal = glm::normalize(result.pos - position);
        result.object = this;
        result.material = material;
        result.ray = ray;
        result.didIntersect = true;
        return result;
} 

int Sphere::CheckPlaneIntersection(int axis, float value) const
{
  if (position[axis] - radius > value)
    return 1;
  if (position[axis] + radius < value)
    return -1;
  return 0; 
}

float Sphere::GetLeftExtreme(int axis) const
{
  return position[axis] - radius;
} 

float Sphere::GetRightExtreme(int axis) const
{
  return position[axis] + radius;
}

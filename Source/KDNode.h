#ifndef KDNODE_H
#define KDNODE_H

#include <vector>
#include <glm/glm.hpp>
#include "Object.h"
using glm::vec3;

struct AABB
{
  vec3 lb;
	vec3 rt;
  AABB(vec3 lb, vec3 rt) : lb(lb), rt(rt) {}

  int GetLongestAxis();
  bool Intersect(Ray& ray, float& t);
};

class Intersection;
class Object;
class KDNode
{
private:
  AABB aabb;
  KDNode *left;
  KDNode *right;
  std::vector<Object*> objects;

public:
  KDNode(AABB aabb, std::vector<Object*> objects, int depth);

  Intersection ClosestIntersection(Ray&);
  Intersection ShadowIntersection(Ray&, const Object*);
};

#endif

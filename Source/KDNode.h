#ifndef KDNODE_H
#define KDNODE_H

#include <vector>
#include <glm/glm.hpp>
#include "RenderableObject.h"
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
class RenderableObject;
class KDNode
{
private:
  AABB aabb;
  KDNode *left;
  KDNode *right;
  std::vector<RenderableObject*> objects;
  int depth;

  bool StopCriterion();
  float CalculateCost(float split_pos, int axis, std::vector<RenderableObject*> objects);

public:
  KDNode(AABB aabb, std::vector<RenderableObject*> objects, int depth);

  Intersection ClosestIntersection(Ray&, const RenderableObject*);
  Intersection ShadowIntersection(Ray&, const RenderableObject*);
};

#endif

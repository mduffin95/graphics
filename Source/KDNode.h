#ifndef KDNODE_H
#define KDNODE_H

#include <vector>
#include <glm/glm.hpp>
#include "Ray.h"
using glm::vec3;

struct AABB
{
  vec3 lb;
	vec3 rt;
  AABB() : lb(vec3(0)), rt(vec3(0)) {}
  AABB(vec3 lb, vec3 rt) : lb(lb), rt(rt) {}

  int GetLongestAxis();
  bool Intersect(Ray& ray, float& t);
  friend AABB operator+(AABB);
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
  KDNode() {}
  KDNode(std::vector<RenderableObject*> objects);
  void Init(AABB aabb, std::vector<RenderableObject*> objects, int depth);

  Intersection ClosestIntersection(Ray&, const RenderableObject*);
  Intersection ShadowIntersection(Ray&, const RenderableObject*);

  static AABB GetEnclosingAABB( std::vector<RenderableObject*> objects );
};

#endif

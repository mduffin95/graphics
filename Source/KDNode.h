#ifndef KDNODE_H
#define KDNODE_H

#include <vector>
#include <glm/glm.hpp>
#include "Object.h"
using glm::vec3;

struct AABB
{
  vec3 min;
  vec3 size;
  AABB(vec3 min, vec3 size) : min(min), size(size) {}

  int GetLongestAxis();
};


class KDNode
{
private:
  AABB aabb;
  KDNode *left;
  KDNode *right;
  std::vector<Object*> objects;

public:
  KDNode(AABB aabb, std::vector<Object*> objects, int depth);

  Intersection ClosestIntersection(Ray);
};

#endif

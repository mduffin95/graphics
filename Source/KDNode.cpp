#include "KDNode.h"
#include <iostream>

int AABB::GetLongestAxis()
{
  if (size.x > size.y)
  {
    if (size.x > size.z)
      return 0; //x
    return 2; //z
  }
  else
  {
    if (size.y > size.z)
      return 1; //y
    return 2; //z
  }
}

KDNode::KDNode(AABB aabb, std::vector<Object*> objects, int depth) : aabb(aabb)
{
  std::cout << "Depth = " << depth << ", size = " << objects.size() << std::endl;
  if (depth == 0)
  {
    //Terminate
    this->objects = objects;
    left = NULL;
    right = NULL;
    return;
  } 
  int axis = aabb.GetLongestAxis();
  //current size
  vec3 sz = aabb.size;
  //Halve along longest axis
  sz[axis] /= 2.0f;
  //Create two new AABBs
  AABB left_aabb(aabb.min, sz);
  AABB right_aabb(aabb.min, sz);
  //Move one along
  float value = aabb.min[axis] + sz[axis];
  right_aabb.min[axis] = value;

  std::vector<Object*> left_objects;
  std::vector<Object*> right_objects;

  //Iterate over all objects, placing in left and right vectors
  for (unsigned i=0; i<objects.size(); i++)
  {
    int x = objects[i]->CheckPlaneIntersection(axis, value); 
    switch(x)
    {

    case(-1): //Less than
      left_objects.push_back(objects[i]);
      break;
    case(0): //Straddling
      left_objects.push_back(objects[i]);
      right_objects.push_back(objects[i]);
      break;
    case(1): //Greater than
      right_objects.push_back(objects[i]);
    }

  }

  left = new KDNode(left_aabb, left_objects, depth-1);
  right = new KDNode(right_aabb, right_objects, depth-1);
}

Intersection KDNode::ClosestIntersection(Ray ray)
{
  if(NULL == left && NULL == right)
  {
    Intersection closest;
    closest.distance = std::numeric_limits<float>::max();
    for (unsigned i=0; i<objects.size(); i++)
    {
      Intersection isec = objects[i]->Intersect(ray);
      if (isec.didIntersect && (isec.distance <= closest.distance))
      {
        closest = isec;
      }
    }
    return closest;

  }
  return Intersection();
}

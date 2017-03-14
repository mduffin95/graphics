#include "KDNode.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


int AABB::GetLongestAxis()
{
	vec3 size = rt - lb;
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

//Adapted from http://gamedev.stackexchange.com/questions/18436
bool AABB::Intersect(Ray& ray, float& t)
{
	// r.dir is unit direction vector of ray
  vec3 rdir = normalize(ray.direction);
  vec3 dirfrac;
	dirfrac.x = 1.0f / rdir.x;
	dirfrac.y = 1.0f / rdir.y;
	dirfrac.z = 1.0f / rdir.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// ray.origin is origin of ray
	float t1 = (lb.x - ray.origin.x)*dirfrac.x;
	float t2 = (rt.x - ray.origin.x)*dirfrac.x;
	float t3 = (lb.y - ray.origin.y)*dirfrac.y;
	float t4 = (rt.y - ray.origin.y)*dirfrac.y;
	float t5 = (lb.z - ray.origin.z)*dirfrac.z;
	float t6 = (rt.z - ray.origin.z)*dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
			t = tmax;
			return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
			t = tmax;
			return false;
	}

	t = tmin;
	return true;
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
  vec3 sz = aabb.rt - aabb.lb;
  //Halve along longest axis
  sz[axis] /= 2.0f;
  //Create two new AABBs
  AABB left_aabb(aabb.lb, aabb.lb + sz);
  AABB right_aabb(aabb.rt - sz, aabb.rt);
  std::cout << "Left lb = " << glm::to_string(left_aabb.lb) << std::endl;
  std::cout << "Left rt = " << glm::to_string(left_aabb.rt) << std::endl;
  std::cout << "Right lb = " << glm::to_string(right_aabb.lb) << std::endl;
  std::cout << "Right rt = " << glm::to_string(right_aabb.rt) << std::endl;
  //Move one along
  float value = left_aabb.rt[axis];

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

Intersection KDNode::ClosestIntersection(Ray& ray)
{
  //float t;
  //if (!aabb.Intersect(ray, t))
  //  return Intersection();

  Intersection closest;
  if(NULL == left && NULL == right) //At a leaf
  {
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
  else
  {
    Intersection isec_l = left->ClosestIntersection(ray);
    Intersection isec_r = right->ClosestIntersection(ray);

    if (isec_l.didIntersect && isec_r.didIntersect)
      return (isec_l.distance < isec_r.distance) ? isec_l : isec_r;
    if (isec_l.didIntersect)
      return isec_l;
    return isec_r;
  }
  /*
  //This is just for testing
  float tl, tr;
  bool lhit = left->aabb.Intersect(ray, tl); 
  bool rhit = right->aabb.Intersect(ray, tr); 
  if (lhit && rhit)
  {
    closest = (tl < tr) ? left->ClosestIntersection(ray) : right->ClosestIntersection(ray);
    if (closest.didIntersect)
      return closest;
    closest = (tl >= tr) ? left->ClosestIntersection(ray) : right->ClosestIntersection(ray);
    return closest;
  }
  if (lhit)
    return left->ClosestIntersection(ray);
  if (rhit)
    return right->ClosestIntersection(ray);
  return Intersection();
  */
}

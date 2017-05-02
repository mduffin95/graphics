#include "KDNode.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <assert.h>
#include "RenderableObject.h"


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
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// ray.origin is origin of ray
	float t1 = (lb.x - ray.origin.x)*ray.dirfrac.x;
	float t2 = (rt.x - ray.origin.x)*ray.dirfrac.x;
	float t3 = (lb.y - ray.origin.y)*ray.dirfrac.y;
	float t4 = (rt.y - ray.origin.y)*ray.dirfrac.y;
	float t5 = (lb.z - ray.origin.z)*ray.dirfrac.z;
	float t6 = (rt.z - ray.origin.z)*ray.dirfrac.z;

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

AABB operator+(const AABB &box1, const AABB &box2)
{
  vec3 lb;
  vec3 rt; 
  lb.x = box1.lb.x < box2.lb.x ? box1.lb.x : box2.lb.x;
  lb.y = box1.lb.y < box2.lb.y ? box1.lb.y : box2.lb.y;
  lb.z = box1.lb.z < box2.lb.z ? box1.lb.z : box2.lb.z;

  rt.x = box1.rt.x > box2.rt.x ? box1.rt.x : box2.rt.x;
  rt.y = box1.rt.y > box2.rt.y ? box1.rt.y : box2.rt.y;
  rt.z = box1.rt.z > box2.rt.z ? box1.rt.z : box2.rt.z;

  return AABB(lb, rt);
}

bool KDNode::StopCriterion()
{
  if (depth == 2)
    return true;
  return false;
}

float KDNode::CalculateCost(float split_pos, int axis, std::vector<RenderableObject*> objects)
{
  vec3 lb = aabb.lb;
  vec3 rt = aabb.rt;
  rt[axis] = split_pos;
  vec3 sz = rt - lb;
  float left_area = 2 * (sz.x * sz.y + sz.y * sz.z + sz.x * sz.z); 
  rt = aabb.rt;
  lb[axis] = split_pos;
  sz = rt - lb;
  float right_area = 2 * (sz.x * sz.y + sz.y * sz.z + sz.x * sz.z); 
  int left_count = 0;
  int right_count = 0;

  for (unsigned i=0; i<objects.size(); i++)
  {
    int x = objects[i]->CheckPlaneIntersection(axis, split_pos); 
    switch(x)
    {
    case(-1): //Less than
      left_count++;
      break;
    case(0): //Straddling
      left_count++;
      right_count++;
      break;
    case(1): //Greater than
      right_count++;
    }
  }

  return 0.3f + 1.0f * (left_area * left_count + right_area * right_count);
}

AABB KDNode::GetEnclosingAABB( std::vector<RenderableObject*> objects )
{
  AABB enclosing(vec3(0), vec3(0));
  for (unsigned i=0; i<objects.size(); i++)
  {
    enclosing = enclosing + objects[i]->GetAABB();  
  }
  return enclosing;
}

KDNode::KDNode(AABB aabb, std::vector<RenderableObject*> objects, int depth) : aabb(aabb)
{
  std::cout << "Depth = " << depth << ", size = " << objects.size() << " " << glm::to_string(aabb.lb) << " "<< glm::to_string(aabb.rt) << std::endl;
  this->depth = depth;
  if (StopCriterion())
  {
    //Terminate
    this->objects = objects;
    left = NULL;
    right = NULL;
    return;
  } 
  int axis = aabb.GetLongestAxis();
  
  float best_cost = 1000000;
  float best_pos = -1;
  //Check all split positions
  for (unsigned i=0; i<objects.size(); i++)
  {
    float left_extreme = objects[i]->GetLeftExtreme(axis);
    float right_extreme = objects[i]->GetRightExtreme(axis);
    float cost;
    cost = CalculateCost(right_extreme, axis, objects);
    if (cost < best_cost)
    {
      best_cost = cost;
      best_pos = right_extreme;
    }
    cost = CalculateCost(left_extreme, axis, objects);
    if (cost < best_cost)
    {
      best_cost = cost;
      best_pos = left_extreme;
    }
  }
  if (best_pos == -1)
  {
    //Terminate
    this->objects = objects;
    left = NULL;
    right = NULL;
    return;

  }

  std::vector<RenderableObject*> left_objects;
  std::vector<RenderableObject*> right_objects;

  //Iterate over all objects, placing in left and right vectors
  for (unsigned i=0; i<objects.size(); i++)
  {
    int x = objects[i]->CheckPlaneIntersection(axis, best_pos); 
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
  
  vec3 rt = aabb.rt;
  vec3 lb = aabb.lb;
  rt[axis] = best_pos;
  lb[axis] = best_pos;
  AABB left_aabb(aabb.lb, rt);
  AABB right_aabb(lb, aabb.rt);
  /*
  std::cout << "Left lb = " << glm::to_string(left_aabb.lb) << std::endl;
  std::cout << "Left rt = " << glm::to_string(left_aabb.rt) << std::endl;
  std::cout << "Right lb = " << glm::to_string(right_aabb.lb) << std::endl;
  std::cout << "Right rt = " << glm::to_string(right_aabb.rt) << std::endl;
  */

  left = new KDNode(left_aabb, left_objects, depth+1);
  right = new KDNode(right_aabb, right_objects, depth+1);
}

Intersection KDNode::ClosestIntersection(Ray& ray, const RenderableObject* exclude)
{
  //float t;
  //if (!aabb.Intersect(ray, t))
  //  return Intersection();

  Intersection closest;
  if(NULL == left && NULL == right) //At a leaf
  {
    closest.distance = std::numeric_limits<float>::max();
    //std::cout << objects.size() << std::endl;
    for (unsigned i=0; i<objects.size(); i++)
    {
      if (objects[i] == exclude)
        continue;
      Intersection isec = objects[i]->Intersect(ray);
      if (isec.didIntersect && (isec.distance <= closest.distance))
      {
        closest = isec;
      }
    }
    return closest;
  }
  /*
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
  */
  //This is just for testing
  float tl, tr;
  bool lhit = left->aabb.Intersect(ray, tl); 
  bool rhit = right->aabb.Intersect(ray, tr); 
  if (lhit && rhit)
  {
    closest = (tl < tr) ? left->ClosestIntersection(ray, exclude) : right->ClosestIntersection(ray, exclude);
    if (closest.didIntersect)
      return closest;
    closest = (tl >= tr) ? left->ClosestIntersection(ray, exclude) : right->ClosestIntersection(ray, exclude);
    return closest;
  }
  if (lhit)
    return left->ClosestIntersection(ray, exclude);
  if (rhit)
    return right->ClosestIntersection(ray, exclude);
  return Intersection();
}

Intersection KDNode::ShadowIntersection(Ray& ray, const RenderableObject* exclude)
{
 
  if(NULL == left && NULL == right) //At a leaf
  {
    for (unsigned i=0; i<objects.size(); i++)
    {
      if (objects[i] == exclude)
        continue;
      Intersection isec = objects[i]->Intersect(ray);
      if (isec.didIntersect && (isec.distance <= 1))
      {
        return isec;
      }
    }
    return Intersection();
  }
  /*
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
  */
  //This is just for testing
  float tl, tr;
  bool lhit = left->aabb.Intersect(ray, tl); 
  bool rhit = right->aabb.Intersect(ray, tr); 
  Intersection closest;
  if (lhit && rhit)
  {
    closest = (tl < tr) ? left->ShadowIntersection(ray, exclude) : right->ShadowIntersection(ray, exclude);
    if (closest.didIntersect)
      return closest;
    closest = (tl >= tr) ? left->ShadowIntersection(ray, exclude) : right->ShadowIntersection(ray, exclude);
    return closest;
  }
  if (lhit)
    return left->ShadowIntersection(ray, exclude);
  if (rhit)
    return right->ShadowIntersection(ray, exclude);
  return Intersection(); 
}

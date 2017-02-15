#include <glm/glm.hpp>
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec2;


struct Intersection
{
		vec3 position;
		float distance;
		int triangleIndex;
};

bool CheckIntersection( float u, float v )
{
	if (u >= 0 &&
			v >= 0 &&
			u + v <= 1)
	{
		return true;
	}
	return false;
}

bool ClosestIntersection(
				vec3 start,
				vec3 dir,
				const vector<Triangle>& triangles,
				Intersection& closestIntersection, int index )
{
	bool result = false;
	for (unsigned i=0; i<triangles.size(); i++)
	{
		vec3 v0 = triangles[i].v0;
		vec3 v1 = triangles[i].v1;
		vec3 v2 = triangles[i].v2;

		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;

		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b;
		float t = x.x;
		float u = x.y;
		float v = x.z;
		//Changed t < d to t <= d, to fix shadow bug
		if (t <= closestIntersection.distance &&
				t >= 0 &&
				CheckIntersection( u, v) &&
				i != index )
		{
			closestIntersection.distance = t;
			closestIntersection.position = start + t * dir;
			closestIntersection.triangleIndex = i;
			result = true;
		}
	}
	return result;
}
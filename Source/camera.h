#include <iostream>
#include <glm/glm.hpp>

#define PI 3.14159265359f

using namespace std;
using glm::vec3;
using glm::mat3;

class Camera
{
  vec3 pos;  
  mat3 R;

  public:
  Camera (vec3 pos) : pos(pos) {}

  void move (vec3);
  void rotate (float);
  
  vec3 transform(vec3);
  friend ostream& operator<<(ostream& os, const Camera& cam);
};

void Camera::move (vec3 movement)
{
  pos += R*movement;
}

void Camera::rotate (float yaw)
{
	R = mat3(cos(yaw), 0.0, -sin(yaw),  // 1. column
					 0, 1.0, 0.0,  // 2. column
					 sin(yaw), 0, cos(yaw)); // 3. column
}

vec3 Camera::transform(vec3 p)
{
  return (p - pos)*R; 
}

ostream& operator<<(ostream& os, const Camera& cam)  
{  
    os << "(" << cam.pos.x
       << "," << cam.pos.y
       << "," << cam.pos.z << ")";
    return os;  
}

#include <iostream>
#include <glm/glm.hpp>

#define PI 3.14159265359f

using namespace std;
using glm::vec3;
using glm::mat3;

class Camera
{
  vec3 pos;  
  mat3 R_x, R_y;

  public:
  Camera (vec3 pos) : pos(pos) {}

  void move (vec3);
  void rotate (float, float);
  
  vec3 transform(vec3);
  friend ostream& operator<<(ostream& os, const Camera& cam);
};

void Camera::move (vec3 movement)
{
  pos += R_y*R_x*movement;
}

void Camera::rotate (float pitch, float yaw)
{
  R_x = mat3(1, 0, 0,  // 1. column
					 0, cos(pitch), -sin(pitch),  // 2. column
					 0, sin(pitch), cos(pitch)); // 3. column

	R_y = mat3(cos(yaw), 0, -sin(yaw),  // 1. column
					 0, 1, 0,  // 2. column
					 sin(yaw), 0, cos(yaw)); // 3. column
}

vec3 Camera::transform(vec3 p)
{
  return (p - pos)*R_y*R_x; 
}

ostream& operator<<(ostream& os, const Camera& cam)  
{  
    os << "(" << cam.pos.x
       << "," << cam.pos.y
       << "," << cam.pos.z << ")";
    return os;  
}

#include "Camera.h"

Camera::Camera (const vec3 pos) : pos(pos) 
{
 	R_x = mat3();
	R_y = mat3();
}

void Camera::move (const vec3 movement)
{
	pos += R_y*R_x*movement;
}

void Camera::rotate (const float pitch, const float yaw)
{
	R_x *= mat3(1, 0, 0,  // 1. column
							0, cos(pitch), -sin(pitch),  // 2. column
							0, sin(pitch), cos(pitch)); // 3. column

	R_y *= mat3(cos(yaw), 0, -sin(yaw),  // 1. column
							0, 1, 0,  // 2. column
							sin(yaw), 0, cos(yaw)); // 3. column
}

vec3 Camera::transform(const vec3 p) const
{
	return (p - pos)*R_y*R_x;
}

vec3 Camera::transform1(const vec3 p) const
{
	return R_y*R_x*(p - pos);
}

ostream& operator<<(ostream& os, const Camera& cam)
{
	os << "(" << cam.pos.x
		 << "," << cam.pos.y
		 << "," << cam.pos.z << ")";
	return os;
}

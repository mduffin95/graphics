#include "Light.h"

#define PI 3.141592653f

void Light::move (vec3 movement)
{
  pos += movement;
}

vec3 Light::transform(vec3 p) const
{
  return (p - pos);
}

vec3 Light::GetColour(float distance) const
{
  return intensity * colour / ( 4.0f * powf(distance,2) * PI );
}

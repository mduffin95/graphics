#include "Light.h"

void Light::move (vec3 movement)
{
  pos += movement;
}

vec3 Light::transform(vec3 p) const
{
  return (p - pos);
}

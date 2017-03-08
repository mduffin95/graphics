//
// Created by John Griffith on 20/02/2017.
//

#include "Light.h"




Light::Light (vec3 pos, vec3 colour) : pos(pos), colour(colour)
{

}

void Light::move (vec3 movement)
{
  pos += movement;
}

vec3 Light::transform(vec3 p) const
{
  return (p - pos);
}

//
// Created by John Griffith on 20/02/2017.
//

#include "Lighting.h"




Lighting::Lighting (vec3 pos, vec3 colour) : pos(pos), colour(colour)
{

}

void Lighting::move (vec3 movement)
{
  pos += movement;
}

vec3 Lighting::transform(vec3 p) const
{
  return (p - pos);
}

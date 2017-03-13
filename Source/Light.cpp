#include "Light.h"

void Light::move (vec3 movement)
{
  pos += movement;
}

vec3 Light::transform(vec3 p) const
{
  return (p - pos);
}

vec3 Light::perp_vec(vec3 in) const
{
  float a = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));
  float b = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/2.0f));

  float c = -(a*in.x + b*in.y) / in.z;

  vec3 v(a, b, c);
  v = normalize(v);
  v *= radius;
  return v;
}

Intersection ShadowIntersection(const Intersection& isec)
{

  for (unsigned i=0; i<isec.renderer->objects.size(); i++)
  {
    if (isec.renderer->objects[i].get() == isec.object) //Prevents self-intersections
      continue;
    Intersection new_isec = isec.renderer->objects[i]->Intersect(isec);
    if (new_isec.didIntersect && (new_isec.distance <= 1))
      return new_isec;
  }
  return Intersection();
}

vec3 Light::GetColour(const Intersection& isec) const
{
  vec3 l = pos - isec.pos;

  //Number of feeler rays that don't hit obstacles
  int count = 0;
  vec3 p(0,0,0);
  for (int s=0; s<SHADOW_SAMPLES; s++)
  {
  
    if (SHADOW_SAMPLES > 1)
    {
      p = perp_vec(l);       
    }

    Ray ray = {isec.pos, l + p};
    Intersection tmp_isec = isec;
    tmp_isec.ray = ray;
    tmp_isec.didIntersect = false;
    
    Intersection shadow = ShadowIntersection(tmp_isec);
    if (!shadow.didIntersect) {
      count++;
    }
  }

  return (count / (float) SHADOW_SAMPLES) * intensity * colour / ( 4.0f * powf(l.length(),2) * PI );
}

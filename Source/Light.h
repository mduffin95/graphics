//
// Created by John Griffith on 20/02/2017.
//

#ifndef COMPUTER_GRAPHICS_LIGHTING_H
#define COMPUTER_GRAPHICS_LIGHTING_H

#include <glm/glm.hpp>
#include "Intersection.h"


#define PI 3.141592653f
#define SHADOW_SAMPLES 1 

using glm::vec3;

class Light {

public:
  Light (vec3 pos, vec3 colour, float intensity, float radius) : pos(pos), radius(radius), intensity(intensity), colour(colour) {}
  vec3 pos;
  float radius;
  float intensity;

  void move(vec3);
  vec3 transform(vec3) const;
  vec3 GetColour(const Intersection& isec) const;
private:
  vec3 colour;
  vec3 perp_vec(vec3) const;


};


#endif //COMPUTER_GRAPHICS_LIGHTING_H

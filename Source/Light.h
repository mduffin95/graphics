//
// Created by John Griffith on 20/02/2017.
//

#ifndef COMPUTER_GRAPHICS_LIGHTING_H
#define COMPUTER_GRAPHICS_LIGHTING_H

#include <glm/glm.hpp>
using glm::vec3;

class Light {

  public:
  Light (vec3 pos, vec3 colour, float intensity, float radius) : pos(pos), colour(colour), intensity(intensity), radius(radius) {}
  vec3 pos;
  vec3 colour;
  float intensity;
  float radius;

  void move (vec3);
  vec3 transform(vec3) const;
  vec3 GetColour(float) const;

};


#endif //COMPUTER_GRAPHICS_LIGHTING_H

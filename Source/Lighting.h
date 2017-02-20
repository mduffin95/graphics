//
// Created by John Griffith on 20/02/2017.
//

#ifndef COMPUTER_GRAPHICS_LIGHTING_H
#define COMPUTER_GRAPHICS_LIGHTING_H

#include <glm/glm.hpp>
using namespace std;
using glm::vec3;
class Lighting {

  public:
  Lighting (vec3 pos, vec3 colour);
  vec3 pos;
  vec3 colour;

  void move (vec3);
  vec3 transform(vec3) const;

};


#endif //COMPUTER_GRAPHICS_LIGHTING_H

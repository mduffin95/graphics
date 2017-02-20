#ifndef COMPUTER_GRAPHICS_CAMERA_H
#define COMPUTER_GRAPHICS_CAMERA_H


#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;

class Camera
{

  mat3 R_x, R_y;

  public:
  Camera (vec3 pos);
  vec3 pos;

  void move (vec3);
  void rotate (float, float);
  
  vec3 transform(vec3);
  friend ostream& operator<<(ostream& os, const Camera& cam);
};




#endif

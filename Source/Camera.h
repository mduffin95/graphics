#ifndef COMPUTER_GRAPHICS_CAMERA_H
#define COMPUTER_GRAPHICS_CAMERA_H


#include <iostream>
#include <glm/glm.hpp>

#define PI 3.14159265359f

using namespace std;
using glm::vec3;
using glm::mat3;

class Camera
{
public:
    vec3 pos;  
    mat3 R_x, R_y;

    Camera (vec3 pos);

    void move (vec3);
    void rotate (float, float);
      
    vec3 transform(vec3);
    vec3 transform1(vec3);
    friend ostream& operator<<(ostream& os, const Camera& cam);
};




#endif

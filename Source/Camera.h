#ifndef COMPUTER_GRAPHICS_CAMERA_H
#define COMPUTER_GRAPHICS_CAMERA_H


#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;

class Camera
{
public:
    vec3 pos;
    mat3 R_x, R_y;

    Camera (const vec3 pos);

    void move (const vec3);
    void rotate (float, float);

    vec3 transform(const vec3) const;
    vec3 transform1(const vec3) const;
    friend ostream& operator<<(ostream& os, const Camera& cam);
};




#endif

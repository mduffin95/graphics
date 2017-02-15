#ifndef COMPUTER_GRAPHICS_DRAW_H
#define COMPUTER_GRAPHICS_DRAW_H

#include "TestTriangle.h"
#include "camera.h"
class Renderer {


public:
		virtual void Draw(Camera &camera,vector<Triangle>& triangles) = 0;

};


#endif //COMPUTER_GRAPHICS_DRAW_H
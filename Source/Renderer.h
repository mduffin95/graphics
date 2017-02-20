#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Lighting.h"

class Renderer {


public:
		virtual void Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles) = 0;

};


#endif //COMPUTER_GRAPHICS_DRAW_H

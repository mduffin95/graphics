#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Lighting.h"

class Renderer {

protected:
    SDL_Surface * screen;
    int width;
    int height;

public:
    Renderer(SDL_Surface* screen);
		virtual void Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles) = 0;

};


#endif //COMPUTER_GRAPHICS_DRAW_H

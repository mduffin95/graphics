#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"

class Renderer {

protected:
    SDL_Surface * screen;
    int width;
    int height;

public:
    virtual void Draw(Camera &camera,vector<Triangle>& triangles) = 0;
    Renderer(SDL_Surface* screen);

};


#endif //COMPUTER_GRAPHICS_DRAW_H

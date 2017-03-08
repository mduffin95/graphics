#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Light.h"

class Renderer {

protected:
    SDL_Surface * screen;
    Camera &camera;
    vector<Light>& lights;
    vector<Triangle>& triangles;
    int width;
    int height;
    Renderer(SDL_Surface*, Camera&, vector<Light>&, vector<Triangle>&);

public:
		virtual void Draw() = 0;

};


#endif //COMPUTER_GRAPHICS_DRAW_H

#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Lighting.h"

class Renderer {

protected:
    SDL_Surface * screen;
    Camera &camera;
    Lighting &lighting;
    vector<Triangle> &triangles;
    int width;
    int height;
    Renderer(SDL_Surface*, Camera&, Lighting&, vector<Triangle>&);

public:
		virtual void Draw() = 0;

};


#endif //COMPUTER_GRAPHICS_DRAW_H

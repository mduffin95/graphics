#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Light.h"
#include <memory>
#include "Scene.h"

class Renderer {

protected:
    SDL_Surface * screen;
    Scene& scene;
    int width;
    int height;
    Renderer(SDL_Surface*, Scene&);

public:
		virtual void Draw() = 0;

};


#endif //COMPUTER_GRAPHICS_DRAW_H

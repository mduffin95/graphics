#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Light.h"
#include <memory>

class Light;

class Renderer {

protected:
  SDL_Surface * screen;
  Camera &camera;
  int width;
  int height;
  Renderer(SDL_Surface*, Camera&, std::vector<Light>&, std::vector<std::shared_ptr<Object>>&);

public:
  virtual void Draw() = 0;
  std::vector<Light>& lights;
  std::vector<std::shared_ptr<Object>>& objects;

};


#endif //COMPUTER_GRAPHICS_DRAW_H

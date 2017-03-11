#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen, Camera &camera, vector<Light>& lights, vector<shared_ptr<IObject>> &objects) 
  : screen(screen), camera(camera), lights(lights), objects(objects)
{
  width = screen->w;
  height = screen->h;
}

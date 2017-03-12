#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen, Camera &camera, std::vector<Light>& lights, std::vector<std::shared_ptr<Object>> &objects) 
  : screen(screen), camera(camera), lights(lights), objects(objects)
{
  width = screen->w;
  height = screen->h;
}

#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen, Scene& scene) 
  : screen(screen), scene(scene)
{
  width = screen->w;
  height = screen->h;
}

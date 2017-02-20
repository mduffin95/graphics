#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen) : screen(screen)
{
  width = screen->w;
  height = screen->h;
}

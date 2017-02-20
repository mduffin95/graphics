#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen)
{
    this->screen = screen;
    width = screen->w;
    height = screen->h;
}

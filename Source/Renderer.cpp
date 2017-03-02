#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen, Camera &camera, Lighting &lighting, vector<Triangle> &triangles) 
  : screen(screen), camera(camera), lighting(lighting), triangles(triangles)
{
  width = screen->w;
  height = screen->h;
}

#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen, Camera &camera, vector<Light>& lights, vector<Triangle> &triangles) 
  : screen(screen), camera(camera), lights(lights), triangles(triangles)
{
  width = screen->w;
  height = screen->h;
}

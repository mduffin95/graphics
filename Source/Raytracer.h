#ifndef RAYTRACER_H 
#define RAYTRACER_H

#include "Renderer.h"
#include "Ray.h"
#include "RenderableObject.h"
#include <SDL.h>

using glm::vec3;
class Raytracer : public Renderer {

public:

  Raytracer(SDL_Surface* screen, Scene& scene, int dofSamples);
  void Draw();

private:
  vec3 CastAtPixel(const int x, const int y);
  vec3 CastRay(const Ray ray);
  int focalLength;
  vec3 indirectLight;
  int dofSamples;

};

#endif

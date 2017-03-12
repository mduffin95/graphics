#ifndef RAYTRACER_H 
#define RAYTRACER_H

#include "Renderer.h"
#include "Ray.h"
#include "Object.h"
#include <SDL.h>

using glm::vec3;
class Raytracer : public Renderer {

public:

  Raytracer(SDL_Surface* screen, Camera &camera, std::vector<Light>& lighting, std::vector<std::shared_ptr<Object>>& objects, int dofSamples);
  void Draw();

private:
  Intersection ClosestIntersection(Ray, const std::vector<std::shared_ptr<Object>>&);
  vec3 CastAtPixel(const int x, const int y);
  vec3 CastRay(const Ray ray);
  int focalLength;
  vec3 indirectLight;
  float dofSamples;

};

#endif

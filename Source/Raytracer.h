#ifndef RAYTRACER_H 
#define RAYTRACER_H

#include "Renderer.h"
#include "TestModel.h"
#include <SDL.h>
#include "Types.h"

using glm::vec3;
class Raytracer : public Renderer {


public:
  enum RenderType {
    Normal, DepthOfField
  };

  Raytracer(SDL_Surface* screen, Camera &camera, vector<Light>& lighting, vector<Triangle>& triangles, int dofSamples);
  void Draw();

private:
  bool CheckIntersection(const float, const float);
  bool ClosestIntersection(Ray, const vector<Triangle>&, Intersection&, int);
  vec3 DirectLight( const Intersection& i, const vector<Triangle>& triangles );
  vec3 CastAtPixel(const int x, const int y);
  vec3 CastRay(const Ray ray);
  int focalLength;
  vec3 indirectLight;
  RenderType type;
  float dofSamples;

};

#endif

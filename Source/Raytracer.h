#ifndef RAYTRACER_H 
#define RAYTRACER_H

#include "Renderer.h"
#include "TestModel.h"
#include <glm/glm.hpp>
#include <SDL.h>

#define NUM_SAMPLES 3

struct Intersection
{
  vec3 position;
  float distance;
  int triangleIndex;
};

using glm::vec3;
class Raytracer : public Renderer {


public:
  enum RenderType {
    Normal, DepthOfField
  };

  Raytracer(SDL_Surface* screen, RenderType type);
  void Draw(const Camera &camera, const Lighting &lighting, const vector<Triangle> &triangles);

private:
  bool CheckIntersection(const float, const float);
  bool ClosestIntersection(vec3, vec3, const vector<Triangle>&, Intersection&, int);
  vec3 DirectLight( const Intersection& i, const vector<Triangle>& triangles );
  vec3 NormalRay(const int x, const int y, const Camera& camera, const Lighting &lighting, const vector<Triangle>& triangles);
  vec3 DOFRay(const int x, const int y, const Camera& camera, const Lighting &lighting, const vector<Triangle>& triangles);
  vec3 lightPos;
  vec3 lightColour;
  int focalLength;
  vec3 indirectLight;
  RenderType type;

  //void (*ProcessRay)(const int x, const int y, const Camera& camera, const Lighting &lighting, const vector<Triangle>& triangles);
};

#endif

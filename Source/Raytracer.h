#ifndef RAYTRACER_H 
#define RAYTRACER_H

#include "Renderer.h"
#include <SDL.h>
#include "Ray.h"
#include "Intersection.h"
#include "IObject.h"


#define SHADOW_SAMPLES 50

using glm::vec3;
class Raytracer : public Renderer {


public:

  Raytracer(SDL_Surface* screen, Camera &camera, vector<Light>& lighting, vector<shared_ptr<IObject>>& objects, int dofSamples);
  void Draw();

private:
  Intersection ClosestIntersection(Ray, const vector<shared_ptr<IObject>>&);
  Intersection ShadowIntersection(Ray, const vector<shared_ptr<IObject>>&, const IObject*);
  vec3 DirectLight( const Intersection& i, const vector<shared_ptr<IObject>>&);
  vec3 CastAtPixel(const int x, const int y);
  vec3 CastRay(const Ray ray);
  int focalLength;
  vec3 indirectLight;
  float dofSamples;

};

#endif

#ifndef RASTERISER_H
#define RASTERISER_H

#include "Renderer.h"
#include "Light.h"
#include "TestModel.h"




using glm::vec3;
using glm::ivec2;
class Rasteriser : public Renderer {

public:
  Rasteriser(SDL_Surface *screen, Camera& camera, vector<Light> &lights, vector<shared_ptr<IObject>> &objects);
  void Draw();

private:
  float *depthBufferCamera;
  float *depthBufferLight;
  vec3 *colourBuffer;
  vec3 getPoint(int x, int y, int w, int h);
  void DrawPolygon(const shared_ptr<IObject> obj);
};


#endif

#ifndef SCENE_H
#define SCENE_H

#include "KDNode.h"

struct Scene
{
  Scene(Camera& camera, std::vector<Light>& lights, KDNode *tree) : camera(camera), lights(lights), tree(tree) {}
  Camera& camera;
  std::vector<Light>& lights;
  KDNode *tree;
};

#endif



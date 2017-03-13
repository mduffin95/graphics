#ifndef SCENE_H
#define SCENE_H

struct Scene
{
  Scene(Camera& camera, std::vector<Light>& lights, std::vector<Object*>& objects) : camera(camera), lights(lights), objects(objects) {}
  Camera& camera;
  std::vector<Light>& lights;
  std::vector<Object*>& objects;
};

#endif



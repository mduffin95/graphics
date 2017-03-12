#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>
#include "Triangle.h"
#include <memory>


void LoadTestModel( std::vector<std::shared_ptr<Object>>& objects, std::vector<std::shared_ptr<Material>>& materials);
#endif

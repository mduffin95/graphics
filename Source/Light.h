//
// Created by John Griffith on 15/02/2017.
//

#ifndef LIGHT_H
#define LIGHT_H

#endif //COMPUTER_GRAPHICS_LIGHT_H

#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;


class Light {

		vec3 lightPos( 0.0f, -0.5f, -0.7f );
		vec3 lightColor = 14.f * vec3( 1, 1, 1 ) * 5.0f;
		vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
		float p = 0.75;

};



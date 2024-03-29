//
// Created by John Griffith on 15/02/2017.
//

#include "TestModel.h"
#include "Triangle.h"
#include "Sphere.h"
#include "RenderableObject.h"
#include "Material.h"
#include <memory>



// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel( std::vector<RenderableObject*>& objects)
{
	using glm::vec3;

  std::vector<Triangle> triangles;

	// Defines colours:
	Material *red = new Phong(vec3(    0.75f, 0.15f, 0.15f ));
	Material *yellow = new Phong(vec3(    0.75f, 0.75f, 0.15f ));
	Material *green = new Phong(vec3(  0.15f, 0.75f, 0.15f ));
	Material *cyan = new Phong(vec3(  0.15f, 0.75f, 0.75f ));
	Material *blue = new Phong(vec3(  0.15f, 0.15f, 0.75f ));
	Material *purple = new Phong(vec3(  0.75f, 0.15f, 0.75f ));
	Material *white = new Phong(vec3(  0.75f, 0.75f, 0.75f ));
	Material *mirror_turquoise = new Mirror(vec3(  0.27f, 0.88f, 0.95f ));
	Material *mirror = new Mirror(vec3(  0.75, 0.75f, 0.75f ));
	Material *glass = new Glass(vec3(  0.75f, 0.75f, 0.75f ));
	Material *turquoise = new Phong(vec3(    0.27f, 0.88f, 0.95f ));

	objects.clear();
	triangles.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec3 A(L,0,0);
	vec3 B(0,0,0);
	vec3 C(L,0,L);
	vec3 D(0,0,L);

	vec3 E(L,L,0);
	vec3 F(0,L,0);
	vec3 G(L,L,L);
	vec3 H(0,L,L);

	// Floor:
	triangles.push_back( Triangle(C,A,B, white) );
	triangles.push_back( Triangle(C,B,D, white) );

	// Right wall
	triangles.push_back( Triangle(A,C,E, red) );
	triangles.push_back( Triangle(C,G,E, red) );

	// Left wall
	triangles.push_back( Triangle(F,D,B, green) );
	triangles.push_back( Triangle(H,D,F, green) );

	// Ceiling
	triangles.push_back( Triangle(E,G,F, white ) );
	triangles.push_back( Triangle(F,G,H, white ) );

	// Back wall
	triangles.push_back( Triangle(G,C,D, white ) );
	triangles.push_back( Triangle(G,D,H, white ) );

	// ---------------------------------------------------------------------------
	// Short block

	A = vec3(265,0,114);
	B = vec3(425,0, 65);
	C = vec3(315,0,272);
	D = vec3(473,0,225);

	E = vec3(265,165,114);
	F = vec3(425,165, 65);
	G = vec3(315,165,272);
	H = vec3(473,165,225);

	// Front
	triangles.push_back( Triangle(E,B,A,white) );
	triangles.push_back( Triangle(E,F,B,white) );

	// Front
	triangles.push_back( Triangle(F,D,B,white) );
	triangles.push_back( Triangle(F,H,D,white) );

	// BACK
	triangles.push_back( Triangle(H,C,D,white) );
	triangles.push_back( Triangle(H,G,C,white) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,white) );
	triangles.push_back( Triangle(E,A,C,white) );

	// TOP
	triangles.push_back( Triangle(G,F,E,white) );
	triangles.push_back( Triangle(G,H,F,white) );

	// ---------------------------------------------------------------------------
	// Tall block

	A = vec3(132,0,247);
	B = vec3(290,0,296);
	C = vec3(83,0,406);
	D = vec3(241,0,456);

	E = vec3(132,330,247);
	F = vec3(290,330,296);
	G = vec3(83,330,406);
	H = vec3(241,330,456);

	// Front
	triangles.push_back( Triangle(E,B,A,white) );
	triangles.push_back( Triangle(E,F,B,white) );

	// Front
	triangles.push_back( Triangle(F,D,B,white) );
	triangles.push_back( Triangle(F,H,D,white) );

	// BACK
	triangles.push_back( Triangle(H,C,D,white) );
	triangles.push_back( Triangle(H,G,C,white) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,white) );
	triangles.push_back( Triangle(E,A,C,white) );

	// TOP
	triangles.push_back( Triangle(G,F,E,white) );
	triangles.push_back( Triangle(G,H,F,white) );


	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for( size_t i=0; i<triangles.size(); ++i )
	{
		triangles[i].v0 *= 2/L;
		triangles[i].v1 *= 2/L;
		triangles[i].v2 *= 2/L;

		triangles[i].v0 -= vec3(1,1,1);
		triangles[i].v1 -= vec3(1,1,1);
		triangles[i].v2 -= vec3(1,1,1);

		triangles[i].v0.x *= 1;
		triangles[i].v1.x *= 1;
		triangles[i].v2.x *= 1;

		triangles[i].v0.y *= 1;
		triangles[i].v1.y *= 1;
		triangles[i].v2.y *= 1;

		triangles[i].ComputeNormal();

    objects.push_back(new Triangle(triangles[i]));
	}

  //objects.push_back(new Sphere(vec3(-0.6,-0.8,-0.6), 0.2f, mirror_turquoise));
}

#include "Raytracer.h"
#include "TestModel.h"
#include "Light.h"
#include "Material.h"
#include "KDNode.h"
#include "ObjImporter.h"

bool ProcessInput(int& t, Camera & camera);
using namespace std;
using glm::vec3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

int main(int argc, char* argv[] )
{
  std::vector<RenderableObject*> objects;

  vec3 lightColour(1,1,1);
	Camera camera(vec3(0,0,-3));
	//Light light1(vec3(0,0,-3), lightColour, 500, 0.01f);
	Light light2(vec3(0,0.9,0), lightColour, 300, 0.01f);
  std::vector<Light> lights = {light2};
  std::vector<Material*> materials;

  std::vector<Triangle> triangles;

	Material *turquoise = new Phong(vec3(    0.27f, 0.88f, 0.95f ));
  ImportFromFile("Source/cruiser.obj", triangles, turquoise); 

	SDL_Surface *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );

	//LoadTestModel( objects, materials );
  for (unsigned i=0; i<triangles.size(); i++)
  {
    objects.push_back(&triangles[i]);
  }

  AABB aabb(vec3(-1,-1,-1), vec3(1,1,1));
  KDNode *tree = new KDNode(aabb, objects, 0);
  Scene scene(camera, lights, tree);

	//SDL_WM_GrabInput( SDL_GRAB_ON );
	//SDL_ShowCursor(0);

  Raytracer r (screen, scene, 1) ;
  //Rasteriser r (screen, camera, lights, objects) ;

	//A bit of a hack to flush initial events
//	SDL_Event e;
//	while( SDL_PollEvent(&e) );

//	while( ProcessInput(t,camera) )
//	{
	int t = SDL_GetTicks();	// Set start value for timer.
  for(int i=0; i<1; i++)
		r.Draw();
	int t2 = SDL_GetTicks();	// Set start value for timer.
	float dt = float(t2-t);
	cout << "Render time: " << dt << " ms." << endl;
//	}
	SDL_SaveBMP( screen, "screenshot.bmp" );
  delete tree;
	return 0;
}


bool ProcessInput(int& t, Camera & camera )
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	float sf = 0.002f;
	cout << "Render time: " << dt << " ms." << endl;
	cout << "Camera pos: " << camera << endl;
	Uint8* keystate = SDL_GetKeyState( 0 );

	vec3 forward(0, 0, 1);
	vec3 down(0, 1, 0);
	vec3 right(1,0,0);

	forward *= sf * dt;
	right *= sf * dt;

	//Camera Position
	if( keystate[SDLK_w] )
	{
		// Move camera forward
		camera.move(forward);
	}
	if( keystate[SDLK_s] )
	{
		// Move camera backward
		camera.move(-forward);
	}
	if( keystate[SDLK_a] )
	{
		// Move camera to the left
		camera.move(-right);
	}
	if( keystate[SDLK_d] )
	{
		// Move camera to the right
		camera.move(right);
	}

	SDL_Event e;

	//Single hit keys and mouse movement
	while( SDL_PollEvent(&e) )
	{
		if( e.type == SDL_QUIT )
			return false;
		if( e.type == SDL_KEYDOWN )
		{
			if( e.key.keysym.sym == SDLK_ESCAPE)
				return false;
		}
		if( e.type == SDL_MOUSEMOTION )
		{
			camera.rotate(-sf * e.motion.yrel, sf * e.motion.xrel);
		}
	}

	return true;
}

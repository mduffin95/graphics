#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "camera.h"
#include <algorithm>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec2;


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
#define C(x,y)  ((x + SCREEN_WIDTH/2) + (y+SCREEN_HEIGHT/2) * SCREEN_WIDTH)
SDL_Surface* screen;
int t;
float clipping_distance = 0.01f;
vector<Triangle> triangles;
vector<Triangle> triangles_extra;
vec3 cameraPos(0,0,-3);
Camera camera(cameraPos);
//in radians
float yaw = 0.0f;
float * depth_buffer = (float*)malloc(sizeof(float)*SCREEN_HEIGHT*SCREEN_WIDTH);
float screen_plane = 0.25 ;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();


int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	LoadTestModel( triangles );

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}
	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
  float sf = 0.001f;
	//cout << "Render time: " << dt << " ms." << endl;
	cout << "Camera pos: " << camera << endl;
	Uint8* keystate = SDL_GetKeyState( 0 );

	vec3 forward(0, 0, 1);
	vec3 down(0, 1, 0);
	vec3 right(1,0,0);

	forward *= sf * dt;
	right *= sf * dt;

	//Camera Position
	if( keystate[SDLK_UP] )
	{
		// Move camera forward

    camera.move(forward);
	}
	if( keystate[SDLK_DOWN] )
	{
		// Move camera backward
    camera.move(-forward);
	}
	if( keystate[SDLK_z] )
	{
		// Move camera to the left
    camera.move(-right);
	}
	if( keystate[SDLK_x] )
	{
		// Move camera to the right
    camera.move(right);
	}

	//Rotation
	if( keystate[SDLK_LEFT]){
		yaw -= sf * dt;
    camera.rotate(yaw);
	}

	if( keystate[SDLK_RIGHT]){
		yaw += sf * dt;
    camera.rotate(yaw);
	}

	//Light position
	/*
	if( keystate[SDLK_w] )
		lightPos += forward;

	if( keystate[SDLK_s] )
		lightPos -= forward;

	if( keystate[SDLK_a] )
		lightPos -= right;

	if( keystate[SDLK_d] )
		lightPos += right;

	if( keystate[SDLK_q] )
		lightPos -= down;

	if( keystate[SDLK_e] )
		lightPos += down;*/
}

void DrawPolygon( const Triangle& t )
{
  //Transform to camera coordinates
  vec3 v0_dash = camera.transform(t.v0);
  vec3 v1_dash = camera.transform(t.v1);
  vec3 v2_dash = camera.transform(t.v2);
  
  //Matrix of vertices
  mat3 M(v0_dash, v1_dash, v2_dash);

  mat3 M_i = glm::inverse(M);

  vec3 w = vec3(1,1,1) * M_i;
  //Get edge functions (rows of M_inv)

  for (int y=-SCREEN_HEIGHT/2; y<SCREEN_HEIGHT/2; y++)
  {
    for(int x=-SCREEN_WIDTH/2; x<SCREEN_WIDTH/2; x++)
    {
      //vec3 p = getPoint(x, y);
      vec3 p(x/ (float) SCREEN_WIDTH, -y/ (float) SCREEN_HEIGHT, 1);
      vec3 E = M_i * p;
      //Check all edge functions
      if (E.x > 0 &&
          E.y > 0 &&
          E.z > 0)
      {
        float W = 1/glm::dot(w, p);
				if(depth_buffer[C(x,y)] > W ){
					depth_buffer[C(x,y)] = W;
					PutPixelSDL(screen, x + SCREEN_WIDTH/2, y + SCREEN_HEIGHT/2, t.color);
				}
      }
    }
  }
}

void Draw()
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	for(int i = 0 ; i < SCREEN_HEIGHT * SCREEN_WIDTH ; i ++){
		depth_buffer[i] = INFINITY;
	}

	for( int i=0; i<triangles.size(); ++i )
	{
		DrawPolygon(triangles[i]);
	}


	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

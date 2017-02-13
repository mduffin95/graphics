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
#define C(x,y)  (x + y * SCREEN_WIDTH)
SDL_Surface* screen;
int t;
float clipping_distance = 0.01f;
vector<Triangle> triangles;
vector<Triangle> triangles_extra;
vec3 cameraPos(0,0,-3);
Camera camera(cameraPos);
//in radians
float yaw = 0.0f;
vec3 current_colour;
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

bool VertexShader( const vec3& v, vec3& p_raster ) {

	vec3 p_camera = camera.transform(v);

	vec2 p_screen;

	p_screen.x = screen_plane * p_camera.x / p_camera.z;
	p_screen.y = screen_plane * p_camera.y / p_camera.z;

	float l = -screen_plane;
	float r = screen_plane;
	float t = screen_plane;
	float b = -screen_plane;


	/*
float l = -SCREEN_WIDTH / 2;
	float r = SCREEN_WIDTH / 2;
	float t = SCREEN_HEIGHT / 2;
	float b = -SCREEN_HEIGHT / 2;

	 */
	vec2 p_ndc;
	p_ndc.x = 2 * p_screen.x / (r - l) - (r + l) / (r - l);
	p_ndc.y = 2 * p_screen.y / (t - b) - (t + b) / (t - b);

	p_raster.x = (p_ndc.x + 1) / 2 * SCREEN_WIDTH;
	p_raster.y = (1-p_ndc.y) / 2 * SCREEN_HEIGHT;

	p_raster.z = p_camera.z;

	return true;
}

float lambdaCalc(vec3 &a, vec3 &b, vec3 &p)
{
	return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}


void DrawPolygon( const Triangle& t )
{
  vec3 v0_dash = camera.transform(t.v0);
  vec3 v1_dash = camera.transform(t.v1);
  vec3 v2_dash = camera.transform(t.v2);
  
  mat3 M(v0_dash, v1_dash, v2_dash);
  mat3 M_i = glm::inverse(M);

  vec3 e0 = vec3(1, 0, 0) * M_i;
  vec3 e1 = vec3(0, 1, 0) * M_i;
  vec3 e2 = vec3(0, 0, 1) * M_i;

  vec3 w = vec3(1,1,1) * M_i;
  //Get edge functions (rows of M_inv)

  for (int y=0; y<SCREEN_HEIGHT; y++)
  {
    for(int x=0; x<SCREEN_WIDTH; x++)
    {
      vec3 p(x/ (float) SCREEN_WIDTH, y/ (float) SCREEN_HEIGHT, 1);
      //vec3 E = glm::transpose(M_i) * p;
      //Check all edge functions
      if (glm::dot(e0, p) > 0 &&
          glm::dot(e1, p) > 0 &&
          glm::dot(e2, p) > 0)
      {
        float W = 1/glm::dot(w, p);
        PutPixelSDL(screen, x, y, t.color);
      }
    }
  }
  //
/*
	u_long V = 3;

	vector<vec3> proj_vertices( V );


	for( int i=0; i<V; ++i ) {
		VertexShader(vertices[i], proj_vertices[i]);
	}

	vec3 V0 = proj_vertices[0];
	vec3 V1 = proj_vertices[1];
	vec3 V2 = proj_vertices[2];

	vec2 bb_min (+numeric_limits<int>::max(),+numeric_limits<int>::max());
	vec2 bb_max (-numeric_limits<int>::max(),-numeric_limits<int>::max()) ;

	for( int i=0; i<V; ++i ) {

		if(proj_vertices[i].x<bb_min.x) bb_min.x = proj_vertices[i].x;
		if(proj_vertices[i].x>bb_max.x) bb_max.x = proj_vertices[i].x;

		if(proj_vertices[i].y<bb_min.y) bb_min.y = proj_vertices[i].y;
		if(proj_vertices[i].y>bb_max.y) bb_max.y = proj_vertices[i].y;

	}
  
  if(bb_max.x < 0 || bb_max.y < 0 || bb_min.x > SCREEN_WIDTH || bb_min.y > SCREEN_HEIGHT)
    return;

  int x0 = std::max(0, (int)floor(bb_min.x));
  int x1 = std::min(SCREEN_WIDTH-1, (int)floor(bb_max.x));
  int y0 = std::max(0, (int)floor(bb_min.y));
  int y1 = std::min(SCREEN_HEIGHT-1, (int)floor(bb_max.y));
  

	for (int y = y0; y <= y1; ++y) {
	  for (int x = x0; x <= x1; ++x) {
			vec3 p(x + 0.5, y + 0.5, 1);

			float lambda0 = lambdaCalc(V1, V2, p);
			float lambda1 = lambdaCalc(V2, V0, p);
			float lambda2 = lambdaCalc(V0, V1, p);

			float totalArea = lambdaCalc(V0, V1, V2);

			lambda0 /= totalArea;
			lambda1 /= totalArea;
			lambda2 /= totalArea;

			if(lambda0 >= 0 && lambda1 >= 0 && lambda2>=0){

				float z = 1 / ( 1/V0.z * lambda0 + 1/V1.z * lambda1 + 1/V2.z * lambda2 );

				if(z >= 0 && z < depth_buffer[C(x,y)] ){
					depth_buffer[C(x,y)] = z;
					PutPixelSDL(screen, x, y, current_colour);
				}
			}
		}
	}
*/

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

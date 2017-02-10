#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::ivec2;
using glm::vec2;

#define PI 3.14159265359f

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
#define C(x,y)  (x + y * SCREEN_HEIGHT)
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos( 0, 0, -3.001 );
mat3 R;
//float focalLength = SCREEN_HEIGHT / 1;
float nearClippingPlane = 1.0f;
float angle = 0.0f;
vec3 current_colour;
float * depth_buffer = (float*)malloc(sizeof(float)*SCREEN_HEIGHT*SCREEN_WIDTH);
/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
void VertexShader( const vec3& v, vec3& p ); 
float lambdaCalc(const vec2 &a, const vec2 &b, const vec2 &p);
void DrawPolygon( const vector<vec3>& vertices );

/*
void Interpolate( ivec2 a, ivec2 b, vector<ivec2>& result )
{
	int N = result.size();
	vec2 step = vec2(b-a) / float(max(N-1,1));
	vec2 current( a );
	for( int i=0; i<N; ++i )
	{
		result[i] = current;
		current += step;
	}
}
*/

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
	cout << "Render time: " << dt << " ms." << endl;
	Uint8* keystate = SDL_GetKeyState( 0 );


	float yaw;
	yaw =  (-angle /180) * PI ;
	R = mat3(cos(yaw), 0.0, -sin(yaw),  // 1. column
					 0, 1.0, 0.0,  // 2. column
					 sin(yaw), 0, cos(yaw)); // 3. column


	vec3 forward(R[2][0], R[2][1], R[2][2]);
	vec3 down(R[1][0], R[1][1], R[1][2]);
	vec3 right(R[0][0], R[0][1], R[0][2]);


	forward *= 0.1;
	right *= 0.1;


	//Camera Position
	if( keystate[SDLK_UP] )
	{
		// Move camera forward
		cameraPos +=  forward ;

	}
	if( keystate[SDLK_DOWN] )
	{
		// Move camera backward
		cameraPos -= forward;
	}
	if( keystate[SDLK_z] )
	{
		// Move camera to the left
		cameraPos -= right;
	}
	if( keystate[SDLK_x] )
	{
		// Move camera to the right
		cameraPos += right;
	}

	//Rotation
	if( keystate[SDLK_LEFT]){
		angle += 5.0;
	}

	if( keystate[SDLK_RIGHT]){
		angle -= 5.0;
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

void VertexShader( const vec3& v, vec3& p_raster ) {

	vec3 p_camera = (v - cameraPos)*R;
  vec2 p_screen;

	p_screen.x = nearClippingPlane * p_camera.x / p_camera.z;
	p_screen.y = nearClippingPlane * p_camera.y / p_camera.z;

  float l = -SCREEN_WIDTH / 2;
  float r = SCREEN_WIDTH / 2;
  float t = SCREEN_HEIGHT / 2;
  float b = -SCREEN_HEIGHT / 2;

  vec2 p_ndc;
  p_ndc.x = 2 * p_screen.x / (r - l) - (r + l) / (r - l);
  p_ndc.y = 2 * p_screen.y / (t - b) - (t + b) / (t - b);

  
	p_raster.x = (p_screen.x + 1) / 2 * SCREEN_WIDTH;
	p_raster.y = (p_screen.y + 1) / 2 * SCREEN_HEIGHT;
	p_raster.z = p_camera.z;
}

float lambdaCalc(const vec2 &a, const vec2 &b, const vec2 &p)
{
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}


void DrawPolygon( const vector<vec3>& vertices )
{
	u_long V = 3;

	vector<vec3> proj_vertices( V );

	for( int i=0; i<V; ++i ) {
		VertexShader(vertices[i], proj_vertices[i]);
	}

	vec3 v0_3d = proj_vertices[0];
	vec3 v1_3d = proj_vertices[1];
	vec3 v2_3d = proj_vertices[2];

  //2D vectors
  vec2 v0(v0_3d.x, v0_3d.y); 
  vec2 v1(v1_3d.x, v1_3d.y); 
  vec2 v2(v2_3d.x, v2_3d.y); 

  float totalArea = lambdaCalc(v0, v1, v2);

	for(int y = 0; y <= SCREEN_HEIGHT; y++) {
		for (int x = 0; x <=SCREEN_WIDTH; x++) {

			vec2 p(x + 0.5f, y + 0.5f);

			float lambda0 = lambdaCalc(v1, v2, p);
			float lambda1 = lambdaCalc(v2, v0, p);
			float lambda2 = lambdaCalc(v0, v1, p);

      lambda0 /= totalArea;
      lambda1 /= totalArea;
      lambda2 /= totalArea;

			if(lambda0 >= 0 && lambda1 >= 0 && lambda2>=0 && (lambda0 + lambda1 + lambda2 <= 1.01) ){

				float z = 1 / ( 1/v0_3d.z * lambda0 + 1/v1_3d.z * lambda1 + 1/v2_3d.z * lambda2 );

				if(z > 0 && z < depth_buffer[C(x,y)] ){
					depth_buffer[C(x,y)] = z;
					PutPixelSDL(screen, x, y, current_colour);
				}
			}
		}
	}
}


void Draw()
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for(int i = 0 ; i < SCREEN_HEIGHT * SCREEN_WIDTH ; i ++){
		depth_buffer[i] = INFINITY;
	}


	for( int i=0; i<triangles.size(); ++i )
	{
		current_colour = triangles[i].color;
		vector<vec3> vertices(3);
		vertices[0] = triangles[i].v0;
		vertices[1] = triangles[i].v1;
		vertices[2] = triangles[i].v2;
		DrawPolygon(vertices);
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

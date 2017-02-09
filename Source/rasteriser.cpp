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
float focalLength = SCREEN_HEIGHT / 1;
float angle = 0.0f;
vec3 current_colour;
float * depth_buffer = (float*)malloc(sizeof(float)*SCREEN_HEIGHT*SCREEN_WIDTH);
/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
void VertexShader( const vec3& v, vec3& p ); 
float lambdaCalc(const vec3 &a, const vec3 &b, const vec3 &p);
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

void VertexShader( const vec3& v, vec3& p ) {

	vec3 p_dash = (v - cameraPos)*R;

	int x = (int)(focalLength * p_dash.x / p_dash.z + SCREEN_WIDTH / 2);
	int y = (int)(focalLength * p_dash.y / p_dash.z + SCREEN_HEIGHT / 2);

	p.x = x;
	p.y = y;
	p.z = p_dash.z;
}

float lambdaCalc(const vec3 &a, const vec3 &b, const vec3 &p)
{
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}


void DrawPolygon( const vector<vec3>& vertices )
{
	u_long V = 3;

	vector<vec3> vertexPixels( V );

	for( int i=0; i<V; ++i ) {
		VertexShader(vertices[i], vertexPixels[i]);
	}

	vec3 V0 = vertexPixels[0];
	vec3 V1 = vertexPixels[1];
	vec3 V2 = vertexPixels[2];

	for(int y = 0 ; y <= SCREEN_HEIGHT ; y++) {
		for (int x = 0; x <=SCREEN_WIDTH; x++) {

			vec3 p(x, y, 0);

			float lambda0 = lambdaCalc(V1, V2, p);
			float lambda1 = lambdaCalc(V2, V0, p);
			float lambda2 = lambdaCalc(V0, V1, p);

			float totalArea = lambdaCalc(V0, V1, V2);

			lambda0 /= totalArea;
			lambda1 /= totalArea;
			lambda2 /= totalArea;

			if(lambda0 >= 0 && lambda1 >= 0 && lambda2>=0 && (lambda0 + lambda1 + lambda2 <= 1) ){

				float z = 1 / ( 1/V0.z * lambda0 + 1/V1.z * lambda1 + 1/V2.z * lambda2 );

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

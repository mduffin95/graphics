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
void ComputePolygonRows(
				const vector<ivec2>& vertexPixels,
				vector<ivec2>& leftPixels,
				vector<ivec2>& rightPixels );
void DrawPolygonRows(
				const vector<ivec2>& leftPixels,
				const vector<ivec2>& rightPixels, vec3 color );



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

	float x = focalLength * p_dash.x / p_dash.z + SCREEN_WIDTH / 2;
	float y = focalLength * p_dash.y / p_dash.z + SCREEN_HEIGHT / 2;

	p.x = x;
	p.y = y;
	p.z = p_dash.z;
}

float lamdaCalc(vec3 &a, vec3 &b, vec3 &p)
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

	/* Don't need the min/max code atm



	vec2 bb_min (+numeric_limits<int>::max(),+numeric_limits<int>::max());
	vec2 bb_max (-numeric_limits<int>::max(),-numeric_limits<int>::max()) ;

	for( int i=0; i<V; ++i ) {
		VertexShader(vertices[i], vertexPixels[i]);

		if(vertexPixels[i].x<bb_min.x) bb_min.x = vertexPixels[i].x;
		if(vertexPixels[i].x>bb_max.x) bb_max.x = vertexPixels[i].x;

		if(vertexPixels[i].y<bb_min.y) bb_min.y = vertexPixels[i].y;
		if(vertexPixels[i].y>bb_max.y) bb_max.y = vertexPixels[i].y;

	}



	//for(int y = (int)floor(bb_min.y) ; y <= (int)ceil(bb_max.y) ; y++) {
		//for (int x = (int)floor(bb_min.x); x <= (int)ceil(bb_max.x); x++) {

	*/

	for(int y = 0 ; y <= SCREEN_HEIGHT ; y++) {
		for (int x = 0; x <=SCREEN_WIDTH; x++) {

			vec3 p(x,y,1);

			float lamda0 = lamdaCalc(V1, V2, p);
			float lamda1 = lamdaCalc(V2, V0, p);
			float lamda2 = lamdaCalc(V0, V1, p);

			float totalArea = lamdaCalc(V0, V1, V2);

			lamda0 /= totalArea;
			lamda1 /= totalArea;
			lamda2 /= totalArea;

			if(lamda0 >= 0 && lamda1 >= 0 && lamda2>=0 ){

				float z = 1 / ( 1/V0.z * lamda0 + 1/V1.z * lamda1 + 1/V2.z * lamda2 );

				if(z >= 0 && z < depth_buffer[C(x,y)] ){
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

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
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos( 0, 0, -3.001 );
mat3 R;
float focalLength = SCREEN_HEIGHT / 1;
float angle = 0.0f;
vec3 current_colour;
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

void ComputePolygonRows(
				const vector<ivec2>& vertexPixels,
				vector<ivec2>& leftPixels,
				vector<ivec2>& rightPixels )
{
// 1. Find max and min y-value of the polygon
//    and compute the number of rows it occupies.


	int y_min = +numeric_limits<int>::max();
	int y_max = 0;

	for(int i = 0 ; i < vertexPixels.size() ; i ++){
		if(vertexPixels[i].y < y_min){
			y_min = vertexPixels[i].y;
		}

		if(vertexPixels[i].y > y_max){
			y_max = vertexPixels[i].y;
		}
	}

	int height = y_max - y_min + 1;
// 2. Resize leftPixels and rightPixels
//    so that they have an element for each row.
	leftPixels = vector<ivec2>(height);
	rightPixels = vector<ivec2>(height);

// 3. Initialize the x-coordinates in leftPixels
//    to some really large value and the x-coordinates
//    in rightPixels to some really small value.
	for(int i = 0 ; i<height ; i++){
		leftPixels[i].x  = +numeric_limits<int>::max();
		rightPixels[i].x = -numeric_limits<int>::max();
	}

// 4. Loop through all edges of the polygon and use
//    linear interpolation to find the x-coordinate for
//    each row it occupies. Update the corresponding
//    values in rightPixels and leftPixels.



	//Loop through edges
	for( int i=0; i<3; ++i )
	{
		int j = (i+1)%3; // The next vertex

		vec2 a = vertexPixels[i];
		vec2 b = vertexPixels[j];
		ivec2 delta = glm::abs( a- b );

		int pixels = glm::max( delta.x, delta.y ) + 1;

		vector<ivec2> result (pixels);
		Interpolate(a, b, result);

		//Loop through pixels on edge

		//TODO: Make more efficient
		for( int k = 0; k < result.size(); k++)
		{
			int y = result[k].y - y_min;
			int x = result[k].x;

			leftPixels[y].y = result[k].y;
			rightPixels[y].y = result[k].y;
			if (leftPixels[y].x > x)
			{
				leftPixels[y].x = x;
			}
			if (rightPixels[y].x < x)
			{
				rightPixels[y].x = x;
			}
		}
	}
}


void VertexShader( const vec3& v, ivec2& p ) {

	vec3 p_dash = (v - cameraPos)*R;

	int x = (int)(focalLength * p_dash.x / p_dash.z + SCREEN_WIDTH / 2);
	int y = (int)(focalLength * p_dash.y / p_dash.z + SCREEN_HEIGHT / 2);

	p.x = x;
	p.y = y;

}

void DrawPolygonRows(
				const vector<ivec2>& leftPixels,
				const vector<ivec2>& rightPixels){
	vec3 color(1, 1, 1);

	long height = leftPixels.size();

	for(int i = 0 ; i < height ; i++){

		int width = leftPixels[i].x - rightPixels[i].x;

		for(int j = 0; j < width ; j++){

			PutPixelSDL(screen, leftPixels[i].x+j,leftPixels[i].y, current_colour);

		}
	}
}

void DrawPolygon( const vector<vec3>& vertices )
{
	int V = vertices.size();

	vector<ivec2> vertexPixels( V );

	for( int i=0; i<V; ++i ) {
		VertexShader(vertices[i], vertexPixels[i]);
	}
	vector<ivec2> leftPixels;
	vector<ivec2> rightPixels;

	ComputePolygonRows( vertexPixels, leftPixels, rightPixels );

	//DrawPolygonRows( leftPixels, rightPixels );
}



void DrawLineSDL( SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color ){

	ivec2 delta = glm::abs( a- b );
	int pixels = glm::max( delta.x, delta.y ) + 1;

	vector<ivec2> result (pixels);
	Interpolate(a,b ,result );

	for(int i = 0 ; i < pixels ; i++) {
		//PutPixelSDL(surface, result[i].x, result[i].y, color);
	}

}

void DrawPolygonEdges( const vector<vec3>& vertices )
{
	int V = vertices.size();
	// Transform each vertex from 3D world position to 2D image position:
	vector<ivec2> projectedVertices( V );
	for( int i=0; i<V; ++i )
	{
		VertexShader( vertices[i], projectedVertices[i] );
	}
	// Loop over all vertices and draw the edge from it to the next vertex:
	for( int i=0; i<V; ++i )
	{
		int j = (i+1)%V; // The next vertex
		vec3 color( 1, 1, 1 );
		//DrawLineSDL( screen, projectedVertices[i], projectedVertices[j], color );
	}
}


void Draw()
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for( int i=0; i<triangles.size(); ++i )
	{
		current_colour = triangles[i].color;
		vector<vec3> vertices(3);
		vertices[0] = triangles[i].v0;
		vertices[1] = triangles[i].v1;
		vertices[2] = triangles[i].v2;

		DrawPolygon(vertices);
	}

	if ( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec2;

#define PI 3.14159265359f

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
#define C(x,y)  (x + y * SCREEN_WIDTH)
SDL_Surface* screen;
int t;
float clipping_distance = 1;
vector<Triangle> triangles;
vec3 cameraPos( 0.5,0,-0.801001);
mat3 R;
float angle = 0.0f;
vec3 current_colour;
float * depth_buffer = (float*)malloc(sizeof(float)*SCREEN_HEIGHT*SCREEN_WIDTH);
float nearClippingPlane = SCREEN_HEIGHT ;

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

bool VertexShader( const vec3& v, vec3& p_raster ) {

	vec3 p_camera = (v - cameraPos)*R;

	vec2 p_screen;

	if( abs(p_camera.z) <= clipping_distance  ) return false;

	p_screen.x = nearClippingPlane * p_camera.x / p_camera.z;
	p_screen.y = nearClippingPlane * p_camera.y / p_camera.z;

	float l = -SCREEN_WIDTH / 2;
	float r = SCREEN_WIDTH / 2;
	float t = SCREEN_HEIGHT / 2;
	float b = -SCREEN_HEIGHT / 2;

	vec2 p_ndc;
	p_ndc.x = 2 * p_screen.x / (r - l) - (r + l) / (r - l);
	p_ndc.y = 2 * p_screen.y / (t - b) - (t + b) / (t - b);

	p_raster.x = (p_ndc.x + 1) / 2 * SCREEN_WIDTH;
	p_raster.y = (1-p_ndc.y) / 2 * SCREEN_HEIGHT;

	p_raster.z = p_camera.z;

	return true;
}

float lamdaCalc(vec3 &a, vec3 &b, vec3 &p)
{
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

bool insideScreen(vec3 &point){
	if(point.x > SCREEN_WIDTH)return false;
	if(point.x < 0)return false;
	if(point.y > SCREEN_HEIGHT)return false;
	if(point.y < 0)return false;

	return true;
}

void DrawPolygon( const vector<vec3>& vertices )
{
	u_long V = 3;

	vector<vec3> vertexPixels( V );


	for( int i=0; i<V; ++i ) {
		if(!VertexShader(vertices[i], vertexPixels[i])){
			return ;
		};
	}

	vec3 V0 = vertexPixels[0];
	vec3 V1 = vertexPixels[1];
	vec3 V2 = vertexPixels[2];


	//cout << V0.x << "," << V0.y << "," <<  V0.z << ":" << current_colour.x <<  "," << current_colour.y << "," << current_colour.z << "\n";


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

			vec3 p(x + 0.5, y+0.5, 1);

			float lamda0 = lamdaCalc(V1, V2, p);
			float lamda1 = lamdaCalc(V2, V0, p);
			float lamda2 = lamdaCalc(V0, V1, p);

			float totalArea = lamdaCalc(V0, V1, V2);

			lamda0 /= totalArea;
			lamda1 /= totalArea;
			lamda2 /= totalArea;
			vec3 green(  0.15f, 0.75f, 0.15f );
			if(lamda0 >= 0 && lamda1 >= 0 && lamda2>=0){

				float z = 1 / ( 1/V0.z * lamda0 + 1/V1.z * lamda1 + 1/V2.z * lamda2 );

				if(z >= 0 && z < depth_buffer[C(x,y)] ){
					depth_buffer[C(x,y)] = z;
					PutPixelSDL(screen, x, y, current_colour);
					if(current_colour == green && x == 500 && y == 250) {
						//cout << x <<","<<y << "   " <<V0.x<<","<<V0.y<<","<<V0.z<<"   "<<V1.x<<","<<V1.y<<","<<V1.z << "    "<<V2.x<<","<<V2.y<<","<<V2.z <<"  " << lamda0 << "  "<< lamda1 << "  "<< lamda2 << "  "  << z<<"\n" ;
					}
				}
			}
		}
	}


	//cout << cameraPos.x << ","<< cameraPos.y << ","<< cameraPos.z << "\n";
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
		current_colour = triangles[i].color;
		vector<vec3> vertices(3);
		vertices[0] = triangles[i].v0;
		vertices[1] = triangles[i].v1;
		vertices[2] = triangles[i].v2;
		DrawPolygon(vertices);
	}

	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

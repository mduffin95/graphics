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
float clipping_distance = 0.5;
vector<Triangle> triangles;
vector<Triangle> triangles_extra;
vec3 cameraPos( 0,0,-3);
mat3 R;
float angle = 0.0f;
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
	//cout << "Render time: " << dt << " ms." << endl;
	cout << "Camera pos: (" << cameraPos.x << "," 
                          << cameraPos.y << ","
                          << cameraPos.z << ","
                          << ")" << endl;
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

	p_screen.x = screen_plane * p_camera.x / p_camera.z;
	p_screen.y = screen_plane * p_camera.y / p_camera.z;

	float l = -screen_plane;
	float r = screen_plane;
	float t = screen_plane;
	float b = -screen_plane;

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


void DrawPolygon( const vector<vec3>& vertices )
{
	u_long V = 3;

	vector<vec3> proj_vertices( V );


	for( int i=0; i<V; ++i ) {
		if(!VertexShader(vertices[i], proj_vertices[i])){
			return ;
		}
	}

	vec3 V0 = proj_vertices[0];
	vec3 V1 = proj_vertices[1];
	vec3 V2 = proj_vertices[2];


	/* Don't need the min/max code atm



	vec2 bb_min (+numeric_limits<int>::max(),+numeric_limits<int>::max());
	vec2 bb_max (-numeric_limits<int>::max(),-numeric_limits<int>::max()) ;

	for( int i=0; i<V; ++i ) {
		VertexShader(vertices[i], proj_vertices[i]);

		if(proj_vertices[i].x<bb_min.x) bb_min.x = proj_vertices[i].x;
		if(proj_vertices[i].x>bb_max.x) bb_max.x = proj_vertices[i].x;

		if(proj_vertices[i].y<bb_min.y) bb_min.y = proj_vertices[i].y;
		if(proj_vertices[i].y>bb_max.y) bb_max.y = proj_vertices[i].y;

	}

	//for(int y = (int)floor(bb_min.y) ; y <= (int)ceil(bb_max.y) ; y++) {
		//for (int x = (int)floor(bb_min.x); x <= (int)ceil(bb_max.x); x++) {

	*/

	for(int y = 0 ; y <= SCREEN_HEIGHT ; y++) {
		for (int x = 0; x <=SCREEN_WIDTH; x++) {

			vec3 p(x + 0.5, y+0.5, 1);

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

}

int temp_triangle_count;

void ClipTriangle( Triangle triangle){

	//TODO: TIDY THIS CODE!

	vector<vec3> verticies(3);
	verticies[0] = triangle.v0;
	verticies[1] = triangle.v1;
	verticies[2] = triangle.v2;

	bool bV0=false;   bool bV1=false;   bool bV2=false;
	int inCount = 0;

	vec3 pos = (verticies[0] - cameraPos)*R;

	if(pos.z > clipping_distance){
		bV0 = true;
		inCount++;
	}

	pos = (verticies[1] - cameraPos)*R;

	if(pos.z > clipping_distance){
		bV1 = true;
		inCount++;
	}

	pos = (verticies[2] - cameraPos)*R;
	if(pos.z > clipping_distance){
		bV2 = true;
		inCount++;
	}

	vector<vec3> in_out(3);

	if(inCount == 1) {

		//cout << "Clipped 1\n" ;

		if (bV0) {
			in_out[0] = verticies[0];
			in_out[1] = verticies[1];
			in_out[2] = verticies[2];
		}
		if (bV1) {
			in_out[0] = verticies[1];
			in_out[1] = verticies[0];
			in_out[2] = verticies[2];
		}
		if (bV2) {
			in_out[0] = verticies[2];
			in_out[1] = verticies[1];
			in_out[2] = verticies[0];
		}

		//Parametric line stuff
		// p = v0 + v01*t
		vec3 v01 = in_out[1] - in_out[0];

		float t1 = (clipping_distance - in_out[0]).z / v01.z;

		float newz1 = clipping_distance;
		float newx1 = in_out[0].x + v01.x * t1;
		float newy1 = in_out[0].y + v01.y * t1;

		// Second vert point
		vec3 v02 = in_out[2] - in_out[0];

		float t2 = (clipping_distance - in_out[0].z) / v02.z;

		float newz2 = clipping_distance;
		float newx2 = in_out[0].x + v02.x * t2;
		float newy2 = in_out[0].y + v02.y * t2;

		in_out[2].x = newx2;
		in_out[2].y = newy2;
		in_out[2].z = newz2;

		in_out[1].x = newx1;
		in_out[1].y = newy1;
		in_out[1].z = newz1;

		Triangle newt(in_out[0], in_out[1], in_out[2], current_colour);

		triangles_extra.push_back(newt);

		temp_triangle_count++;

	}else if(inCount == 2){

		if (!bV0) {
			in_out[0] = verticies[1];
			in_out[1] = verticies[2];
			in_out[2] = verticies[0];
		}
		if (!bV1) {
			in_out[0] = verticies[0];
			in_out[1] = verticies[2];
			in_out[2] = verticies[1];
		}
		if (!bV2) {
			in_out[0] = verticies[0];
			in_out[1] = verticies[1];
			in_out[2] = verticies[2];
		}

		//Parametric line stuff
		// p = v0 + v01*t
		vec3 v01 = in_out[2] - in_out[0];

		float t1 = ((clipping_distance - (in_out[0]).z)/v01.z );

		float newz1 = clipping_distance;
		float newx1 = (in_out[0]).x + v01.x * t1;
		float newy1 = (in_out[0]).y + v01.y * t1;

		// Second point
		vec3 v02 = in_out[2] - in_out[1];

		float t2 = ((clipping_distance - (in_out[1]).z)/v02.z );

		float newz2 = clipping_distance;
		float newx2 = (in_out[1]).x + v02.x * t2;
		float newy2 = (in_out[1]).y + v02.y * t2;

		in_out[2].x = newx1;
		in_out[2].y = newy1;
		in_out[2].z = newz1;

		Triangle newt(in_out[0], in_out[1], in_out[2], current_colour);

		triangles_extra.push_back(newt);

		temp_triangle_count++;

		Triangle tempTriangle( vec3(newx1, newy1, newz1) , vec3(newx2, newy2, newz2) , in_out[1] , current_colour);
		triangles_extra.push_back(tempTriangle);

		temp_triangle_count ++;

	}else if(inCount == 3){

		triangles_extra.push_back(triangle);
		temp_triangle_count++;

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

	temp_triangle_count=0;
	triangles_extra.clear();
	for( int i=0; i<triangles.size(); ++i )
	{
		current_colour = triangles[i].color;
		ClipTriangle(triangles[i]);
	}

	for( int i=0; i<temp_triangle_count; ++i )
	{
		current_colour = triangles_extra[i].color;
		vector<vec3> vertices(3);
		vertices[0] = triangles_extra[i].v0;
		vertices[1] = triangles_extra[i].v1;
		vertices[2] = triangles_extra[i].v2;
		DrawPolygon(vertices);
	}


	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

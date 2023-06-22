#pragma once

#include <math.h>
#include <iostream>
#include <fstream>
#include "gl\freeglut.h"		// OpenGL header files
#include <list>
#include <vector>
#include <string>
#include <map>
#include <sstream>


//#define TIME_CHECK_
#define DEPTH_CALIB_
#pragma warning(disable:4996)
#define BLOCK 4
#define READ_SIZE 110404//5060//110404
#define scale 20
#define M_PI 3.14159265358979323846
//#define TEST 14989
//#define STATIC_

using namespace std;

struct Vertex{
	float X;
	float Y;
	float Z;
	int index_1;
	int index_2;
	int index_3;
};

struct MMesh {
	int V1;
	int V2;
	int V3;
	int V4;
	int T1;
	int T2;
	int T3;
	int T4;
};

struct Material {
	string name;
	float Ns;
	float Ka[3];
	float Ks[3];
	float Ke[3];
	float Ni;
	float d;
	int illum;
	std::string map_Kd;
};
vector<Material> materials;


// variables for GUI
const float TRACKBALLSIZE = 0.8f;
const int RENORMCOUNT = 97;
//const int width = KinectBasic::nColorWidth;
//const int height = KinectBasic::nColorHeight;

GLint drag_state = 0;
GLint button_state = 0;

GLint rot_x = 0;
GLint rot_y = 0;
GLint trans_x = 0;
GLint trans_y = 0;
GLint trans_z = 0;

GLubyte mytexels[2048][2048][3];

int add_depth_flag = 0;
int model_flag = 0;
int depth_display_flag = 0;
int geodesic_skel[23][5] = { 0 };
int trcon = 0;
int trflag = 0;
int rotflag = 0;
float zmin = 100000, zmax = -100000;

int side_status[50] = { 0 };

float quat[4] = {0};
float t[3] = {0};

Vertex skt[23];
BOOLEAN bTracked = false;
bool checkt = false;
Vertex *vertex;
Vertex *vertex_color;
Vertex* vertex_texture;
MMesh* mymesh;

Vertex* vertex2;
Vertex* vertex_color2;
MMesh* mymesh2;

Vertex* vertex3;
Vertex* vertex_color3;
MMesh* mymesh3;

Vertex* vertex4;
Vertex* vertex_color4;
MMesh* mymesh4;


Vertex* vertex5;
Vertex* vertex_color5;
MMesh* mymesh5;

Vertex* vertex6;
Vertex* vertex_color6;
MMesh* mymesh6;

Vertex* vertex7;
Vertex* vertex_color7;
MMesh* mymesh7;

Vertex* vertex8;
Vertex* vertex_color8;
MMesh* mymesh8;

Vertex* vertex10;
Vertex* vertex_color10;
MMesh* mymesh10;

Vertex* vertex9;
Vertex* vertex_color9;
MMesh* mymesh9;

Vertex* vertex11;
Vertex* vertex_color11;
MMesh* mymesh11;


Vertex* rock1;
Vertex* rock_color1;
MMesh* rockmesh1;
Vertex* rock2;
Vertex* rock_color2;
MMesh* rockmesh2;
Vertex* rock3;
Vertex* rock_color3;
MMesh* rockmesh3;
Vertex* rock4;
Vertex* rock_color4;
MMesh* rockmesh4;

Vertex* wood1;
Vertex* wood_color1;
MMesh* woodmesh1;
Vertex* wood2;
Vertex* wood_color2;
MMesh* woodmesh2;
Vertex* wood3;
Vertex* wood_color3;
MMesh* woodmesh3;
Vertex* wood4;
Vertex* wood_color4;
MMesh* woodmesh4;
Vertex* wood5;
Vertex* wood_color5;
MMesh* woodmesh5;

Vertex* wood6;
Vertex* wood_color6;
MMesh* woodmesh6;
Vertex* wood7;
Vertex* wood_color7;
MMesh* woodmesh7;
Vertex* wood8;
Vertex* wood_color8;
MMesh* woodmesh8;
Vertex* wood9;
Vertex* wood_color9;
MMesh* woodmesh9;
Vertex* wood10;
Vertex* wood_color10;
MMesh* woodmesh10;

Vertex* wood11;
Vertex* wood_color11;
MMesh* woodmesh11;
Vertex* wood12;
Vertex* wood_color12;
MMesh* woodmesh12;
Vertex* wood13;
Vertex* wood_color13;
MMesh* woodmesh13;


Vertex* tree1;
Vertex* tree_color1;
MMesh* treemesh1;
Vertex* tree2;
Vertex* tree_color2;
MMesh* treemesh2;

Vertex* tree3;
Vertex* tree_color3;
MMesh* treemesh3;
Vertex* tree4;
Vertex* tree_color4;
MMesh* treemesh4;
Vertex* tree5;
Vertex* tree_color5;
MMesh* treemesh5;

Vertex* tree6;
Vertex* tree_color6;
MMesh* treemesh6;
Vertex* tree7;
Vertex* tree_color7;
MMesh* treemesh7;
Vertex* tree8;
Vertex* tree_color8;
MMesh* treemesh8;
Vertex* tree9;
Vertex* tree_color9;
MMesh* treemesh9;
Vertex* tree10;
Vertex* tree_color10;
MMesh* treemesh10;

Vertex* tree11;
Vertex* tree_color11;
MMesh* treemesh11;
Vertex* tree12;
Vertex* tree_color12;
MMesh* treemesh12;


Vertex* stone1;
Vertex* stone_color1;
MMesh* stonemesh1;

Vertex* stone2;
Vertex* stone_color2;
MMesh* stonemesh2;

Vertex* stone3;
Vertex* stone_color3;
MMesh* stonemesh3;

Vertex* stone4;
Vertex* stone_color4;
MMesh* stonemesh4;

Vertex* stone5;
Vertex* stone_color5;
MMesh* stonemesh5;

Vertex* stone6;
Vertex* stone_color6;
MMesh* stonemesh6;

Vertex* stone7;
Vertex* stone_color7;
MMesh* stonemesh7;

Vertex* stone8;
Vertex* stone_color8;
MMesh* stonemesh8;

Vertex* stone9;
Vertex* stone_color9;
MMesh* stonemesh9;

Vertex* stone10;
Vertex* stone_color10;
MMesh* stonemesh10;

Vertex* cloud1;
MMesh* cloudmesh1;
Vertex* cloud2;
MMesh* cloudmesh2;
Vertex* cloud3;
MMesh* cloudmesh3;
Vertex* cloud4;
MMesh* cloudmesh4;
Vertex* cloud5;
MMesh* cloudmesh5;

Vertex* wall;

Vertex* bird;
Vertex* bird_color;
MMesh* birdmesh;
int birdl;

bool recheck;

// variables for display OpenGL based point viewer
int dispWindowIndex = 0;
GLuint dispBindIndex = 0;
const float dispPointSize = 2.0f;

// variables for display text
string dispString = "";
const string dispStringInit = "Depth Threshold: D\nInfrared Threshold: I\nNonlocal Means Filter: N\nPick BodyIndex: P\nAccumulate Mode: A\nSelect Mode: C,B(select)\nSave: S\nReset View: R\nQuit: ESC";
string frameRate;

HANDLE hMutex;
//KinectBasic kinect;

// functions for GUIs
void InitializeWindow(int argc, char* argv[]);

// high-level functions for GUI
void draw_center();
void idle();
void display();
void close();
void special(int, int, int) {}
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void reshape(int, int);
void motion(int, int);


// basic functions for computation/GUI
// trackball codes were imported from those of Gavin Bell
// which appeared in SIGGRAPH '88
void vzero(float*);
void vset(float*, float, float, float);
void vsub(const float*, const float*, float*);
void vcopy(const float*, float*);
void vcross(const float *v1, const float *v2, float *cross);
float vlength(const float *v);
void vscale(float *v, float div);
void vnormal(float *v);
float vdot(const float *v1, const float *v2);
void vadd(const float *src1, const float *src2, float *dst);

void trackball(float q[4], float, float, float, float);
//void add_quats(float*, float*, float*);
void axis_to_quat(float a[3], float phi, float q[4]);
void normalize_quat(float q[4]);
float tb_project_to_sphere(float, float, float);
void build_rotmatrix(float m[4][4], float q[4]);
void Reader();
void DrawObj();
void DrawMeshObj();
void Setskt();
void drawing(int);

//CameraSpacePoint m_SpacePoint[JointType::JointType_Count];
void Track();

int len, len2, len3, len4, len5, len6, len7, len8, len9, len10;
int len11, len12, len13, len14, len15;
int lwood1, lwood2, lwood3, lwood4, lwood5, lwood6, lwood7;
int lwood8, lwood9, lwood10, lwood11, lwood12, lwood13;
int ltree1, ltree2, ltree3, ltree4, ltree5, ltree6, ltree7;
int ltree8, ltree9, ltree10, ltree11, ltree12;

int lstone1, lstone2, lstone3, lstone4, lstone5;
int lstone6, lstone7, lstone8, lstone9, lstone10;

int lcloud1, lcloud2, lcloud3, lcloud4, lcloud5;
float objectPositionX = 0.0f; //오브젝트 초기 위치
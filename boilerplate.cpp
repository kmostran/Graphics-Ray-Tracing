// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include "ImageBuffer.h"
#include <math.h>
#include <vector>

#define PI 3.14159265

// Specify that we want the OpenGL core profile before including GLFW headers
#ifndef LAB_LINUX
	#include <glad/glad.h>
#else
	#define GLFW_INCLUDE_GLCOREARB
	#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

using namespace glm; //use glm without having to keep adding in glm
using namespace std;

struct Light{
	float x;
	float y;
	float z;
	vec3 cl;
	vec3 ca;
};

Light light1;
Light light2;
Light light3;


struct Sphere{
	float x;
	float y;
	float z;
	float r;
	vec3 cr;
	vec3 cp;
};
Sphere sphere1;

Sphere sphereA;
Sphere sphereB;
Sphere sphereC;

Sphere sphereD;
Sphere sphereE;
Sphere sphereF;
Sphere sphereG;
Sphere sphereH;
Sphere sphereI;
Sphere sphereJ;
Sphere sphereK;
Sphere sphereL;
Sphere sphereM;
Sphere sphereN;
Sphere sphereO;
Sphere sphereP;
Sphere sphereQ;
Sphere sphereR;
Sphere sphereS;

Sphere sphereT;
Sphere sphereU;
Sphere sphereV;
Sphere sphereW;
Sphere sphereX;
Sphere sphereY;
Sphere sphereZ;
Sphere spherea;
Sphere sphereb;
Sphere spherec;
Sphere sphered;
Sphere spheree;
Sphere spheref;
Sphere sphereg;
Sphere sphereh;



struct Triangle{
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;
	float x3;
	float y3;
	float z3;
	vec3 cr;
	vec3 cp;
};
Triangle triangle1;
Triangle triangle2;
Triangle triangle3;
Triangle triangle4;
Triangle triangle5;
Triangle triangle6;
Triangle triangle7;
Triangle triangle8;
Triangle triangle9;
Triangle triangle10;
Triangle triangle11;
Triangle triangle12;

Triangle triangleA;
Triangle triangleB;
Triangle triangleC;
Triangle triangleD;
Triangle triangleE;
Triangle triangleF;
Triangle triangleG;
Triangle triangleH;
Triangle triangleI;
Triangle triangleJ;
Triangle triangleK;
Triangle triangleL;
Triangle triangleM;
Triangle triangleN;
Triangle triangleO;
Triangle triangleP;
Triangle triangleQ;
Triangle triangleR;
Triangle triangleS;
Triangle triangleT;
Triangle triangleU;
Triangle triangleV;
Triangle triangleW;
Triangle triangleX;
Triangle triangleY;
Triangle triangleZ;
Triangle triangleAA;
Triangle triangleBB;
Triangle triangleCC;
Triangle triangleDD;
Triangle triangleEE;
Triangle triangleFF;
Triangle triangleGG;

Triangle triangleF1;
Triangle triangleF2;

struct Plane{
	float xn;
	float yn;
	float zn;
	float xq;
	float yq;
	float zq;
	vec3 cr;
	vec3 cp;
};
Plane plane1;

Plane planeA;
Plane planeB;

Plane planeB1;

vec3 color;
vec3 c;		//colour multiplier for shading

vec3 origin;

bool shadow;
int shadowCounter;
bool objectHit;
bool doingShadow;
float minT;
float oldMinT;

vec4 circleIntersect(const vec3& d, Sphere &sphere, Light &light);
vec4 planeIntersect(const vec3& v, Plane &plane, Light &light);
vec4 triangelIntersect(const vec3& v, Triangle &triangle, Light &light);
int rayIntersect(const vec3& normalViewRay, bool shadow);
int rayIntersect2(const vec3& normalViewRay, bool shadow);
int rayIntersect3(const vec3& normalViewRay, bool shadow);


// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

struct MyShader
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint  vertex;
	GLuint  fragment;
	GLuint  program;

	// initialize shader and program names to zero (OpenGL reserved value)
	MyShader() : vertex(0), fragment(0), program(0)
	{}
};

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
	// load shader source from files
	string vertexSource = LoadSource("vertex.glsl");
	string fragmentSource = LoadSource("fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry)
{
	// three vertex positions and assocated colours of a triangle
	const GLfloat vertices[][2] = {
		{ -.6f, -.4f },
                { 0, .6f },
		{ .6f, -.4f }
	};

	const GLfloat colours[][3] = {
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	geometry->elementCount = 3;

	// these vertex attribute indices correspond to those specified for the
	// input variables in the vertex shader
	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	// assocaite the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(MyGeometry *geometry, MyShader *shader)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

//Print a vec3
//glm is similar to glsl (aside from minor things, such as glm not being able to swizzle
void printVec3(const char* prefix, const vec3& v)
{
	cout << prefix << " (" << v.x << ", " << v.y << ", " << v.z << ")" << endl;
}


vec4 circleIntersect(const vec3& d, Sphere &sphere, Light &light)
{
	// cout <<  " (" << v.x << ", " << v.y << ", " << v.z << ")" << endl;
	// cout << " (" << sphere.x << ", " << sphere.y << ", " << sphere.c1 << ")" << endl;
	vec4 closestIntersection(0,0,0,0);
	vec3 cir(sphere.x,sphere.y,sphere.z);
//	float aq = dot(d,d);
//	float bq = -2*dot(cir,d);
//	float cq = -2*dot(origin,cir) + dot(origin,origin) + dot(cir,cir)-(sphere.r*sphere.r);
        float aq = dot(d,d);
        float bq = 2*dot(origin,d)-2*dot(cir,d);
        float cq = dot(origin,origin)- 2*dot(origin,cir) + dot(cir,cir) - (sphere.r*sphere.r);
	//cout << d.x << d.y << d.z << endl;
	//cout << a << "{}" << b << "{}" << c << endl;

	if((bq*bq)-(4*aq*cq)<0){
		return(closestIntersection);
	}

	float x1 = (-bq+sqrt((bq*bq)-(4*aq*cq)))/(2*aq);
	float x2 = (-bq-sqrt((bq*bq)-(4*aq*cq)))/(2*aq);

	//cout << x1 << "()" << x2 << endl;

	vec3 x1r(d.x*x1,d.y*x1,d.z*x1);
	vec3 x2r(d.x*x2,d.y*x2,d.z*x2);
	if(x1r.z<x1r.z){
		closestIntersection.x=x1r.x;
		closestIntersection.y=x1r.y;
		closestIntersection.z=x1r.z;
		closestIntersection.w=x1;
	}
	else{
		closestIntersection.x=x2r.x;
		closestIntersection.y=x2r.y;
		closestIntersection.z=x2r.z;
		closestIntersection.w=x2;
	}
	vec3 intersectionPoint(closestIntersection.x,closestIntersection.y,closestIntersection.z);
	vec3 e = -d*closestIntersection.w;
	vec3 l(light.x-intersectionPoint.x,light.y-intersectionPoint.y,light.z-intersectionPoint.z);
	vec3 normL = normalize(l);
	vec3 normH((e.x+l.x)/length(e+l),(e.y+l.y)/length(e+l),(e.z+l.z)/length(e+l));
	vec3 normN = normalize(intersectionPoint-cir);
	if(doingShadow==false){
	c.x=sphere.cr.x*(light.ca.x+light.cl.x*std::max(0.f,dot(normN,normL)))+light.cl.x*sphere.cp.x*pow(std::max(0.f,dot(normH,normN)),128);
	c.y=sphere.cr.y*(light.ca.y+light.cl.y*std::max(0.f,dot(normN,normL)))+light.cl.y*sphere.cp.y*pow(std::max(0.f,dot(normH,normN)),128);
	c.z=sphere.cr.z*(light.ca.z+light.cl.z*std::max(0.f,dot(normN,normL)))+light.cl.z*sphere.cp.z*pow(std::max(0.f,dot(normH,normN)),128);}
	//c.x=sphere.cr.x;c.y=sphere.cr.y;c.z=sphere.cr.z;
	return(closestIntersection);
}

vec4 planeIntersect(const vec3& d, Plane &plane, Light &light)
{
	vec4 closestIntersection(0,0,0,0);
	vec3 n(plane.xn,plane.yn,plane.zn);
	vec3 q(plane.xq,plane.yq,plane.zq);
	float t = (dot(q,n)-dot(origin,n))/dot(d,n);
	closestIntersection.x = d.x*t;
	closestIntersection.y = d.y*t;
	closestIntersection.z = d.z*t;
	closestIntersection.w = t;
	vec3 intersectionPoint(closestIntersection.x,closestIntersection.y,closestIntersection.z);
	vec3 e = -d*t;
	vec3 l(light.x-intersectionPoint.x,light.y-intersectionPoint.y,light.z-intersectionPoint.z);
	vec3 normL = normalize(l);
	vec3 normH((e.x+l.x)/length(e+l),(e.y+l.y)/length(e+l),(e.z+l.z)/length(e+l));
	vec3 normN = n;
	if(doingShadow==false){
	c.x=plane.cr.x*(light.ca.x+light.cl.x*std::max(0.f,dot(normN,normL)))+light.cl.x*plane.cp.x*pow(std::max(0.f,dot(normH,normN)),256);
	c.y=plane.cr.y*(light.ca.y+light.cl.y*std::max(0.f,dot(normN,normL)))+light.cl.y*plane.cp.y*pow(std::max(0.f,dot(normH,normN)),256);
	c.z=plane.cr.z*(light.ca.z+light.cl.z*std::max(0.f,dot(normN,normL)))+light.cl.z*plane.cp.z*pow(std::max(0.f,dot(normH,normN)),256);}
	//c.x=plane.c1;c.y=plane.c2;c.z=plane.c3;
	return(closestIntersection);
}

vec4 triangleIntersect(const vec3& d, Triangle &triangle, Light &light)
{
	float u;
	float v;
	float t;
	vec4 closestIntersection(0,0,0,0);
	//vec3 p1(triangle.x1,triangle.y1,triangle.z1);
	//vec3 p2(triangle.x2,triangle.y2,triangle.z2);
	//vec3 p3(triangle.x3,triangle.y3,triangle.z3);
	//vec3 e1 = p2-p1;
	//vec3 e2 = p3-p1;
	vec3 e1(triangle.x2-triangle.x1,triangle.y2-triangle.y1,triangle.z2-triangle.z1);
	vec3 e2(triangle.x3-triangle.x1,triangle.y3-triangle.y1,triangle.z3-triangle.z1);
	vec3 e3(triangle.x3-triangle.x2,triangle.y3-triangle.y2,triangle.z3-triangle.z2);
	vec3 e4(triangle.x1-triangle.x3,triangle.y1-triangle.y3,triangle.z1-triangle.z3);
	//1-2,3-2
	// triangleA.x1=2.75;
	// triangleA.x2=2.75;
	// triangleA.x3=-2.75;
	// triangleA.y1=2.75;
	// triangleA.y2=2.75;
	// triangleA.y3=2.75;
	// triangleA.z1=-10.5;
	// triangleA.z2=-5;
	// triangleA.z3=-5;
	// triangleA.cr=vec3(0,0.5,0.5);
	// triangleA.cp=vec3(0.5,0.5,0.5);
	// vec3 e3(triangleA.x2-triangleA.x1,triangleA.y2-triangleA.y1,triangleA.z2-triangleA.z1);
	// vec3 e4(triangleA.x3-triangleA.x1,triangleA.y3-triangleA.y1,triangleA.z3-triangleA.z1);

	//vec3 e1(-0.93+0.4,0.55+2.75,-8.51+9.55);
	//vec3 e2(0.11+0.4,-2.75+2.75,-7.98+9.55);

	vec3 s(-triangle.x1,-triangle.y1,-triangle.z1);
	s = origin + s;
	mat3 matA(-d,s,e2);
	mat3 matB(-d,e1,e2);
	mat3 matC(-d,e1,s);
	mat3 matD(s,e1,e2);
	u = determinant(matA)/determinant(matB);
	v = determinant(matC)/determinant(matB);
	t = determinant(matD)/determinant(matB);

	if((u<0)||(u>1)||(v<0)||(v>1)||(u+v>1)||(u+v<0)){
		return(closestIntersection);
	}
	else{
		closestIntersection.x=1-u-v;
		closestIntersection.y=u;
		closestIntersection.z=v;
		closestIntersection.w=t;
	}

	vec3 intersectionPoint(closestIntersection.x,closestIntersection.y,closestIntersection.z);
	vec3 e = -d*t;
	vec3 l(light.x-intersectionPoint.x,light.y-intersectionPoint.y,light.z-intersectionPoint.z);
	vec3 normL = normalize(l);
	vec3 normH((e.x+l.x)/length(e+l),(e.y+l.y)/length(e+l),(e.z+l.z)/length(e+l));
	// vec3 normN = normalize(cross(e2,e1));
	vec3 normN = normalize(cross(e3,e4));
	// if(dot(normN,normH)<0){
	// 	normN=-normN;
	// }
	// if(dot(normN,normL)<0){
	// 	normN=-normN;
	// }
	// c.x=triangle.cr.x*(light.ca.x+light.cl.x*std::max(0.f,dot(normN,normL)))+light.cl.x*triangle.cp.x*pow(std::max(0.f,dot(normH,normN)),256);
	// c.y=triangle.cr.y*(light.ca.y+light.cl.y*std::max(0.f,dot(normN,normL)))+light.cl.y*triangle.cp.y*pow(std::max(0.f,dot(normH,normN)),256);
	// c.z=triangle.cr.z*(light.ca.z+light.cl.z*std::max(0.f,dot(normN,normL)))+light.cl.z*triangle.cp.z*pow(std::max(0.f,dot(normH,normN)),256);
	//c.x=triangle.cr.x*light.cl.x*std::max(0.f,dot(normN,normL));
	//c.y=triangle.cr.y*light.cl.y*std::max(0.f,dot(normN,normL));
	//c.z=triangle.cr.z*light.cl.z*std::max(0.f,dot(normN,normL));
	//c = normN;

	c.x=triangle.cr.x*(light.ca.x+light.cl.x*std::max(0.f,std::max(-dot(normN,normL),dot(normN,normL))))+light.cl.x*triangle.cp.x*pow(std::max(0.f,std::max(-dot(normH,normN),dot(normH,normN))),256);
	c.y=triangle.cr.y*(light.ca.y+light.cl.y*std::max(0.f,std::max(-dot(normN,normL),dot(normN,normL))))+light.cl.y*triangle.cp.y*pow(std::max(0.f,std::max(-dot(normH,normN),dot(normH,normN))),256);
	c.z=triangle.cr.z*(light.ca.z+light.cl.z*std::max(0.f,std::max(-dot(normN,normL),dot(normN,normL))))+light.cl.z*triangle.cp.z*pow(std::max(0.f,std::max(-dot(normH,normN),dot(normH,normN))),256);

	//}
	//else{
	//normN=-normN;
	//c.x=triangle.cr.x*(light.ca.x+light.cl.x*std::max(0.f,dot(normN,normL)))+light.cl.x*triangle.cp.x*pow(std::max(0.f,dot(normH,normN)),256);
	//c.y=triangle.cr.y*(light.ca.y+light.cl.y*std::max(0.f,dot(normN,normL)))+light.cl.y*triangle.cp.y*pow(std::max(0.f,dot(normH,normN)),256);
	//c.z=triangle.cr.z*(light.ca.z+light.cl.z*std::max(0.f,dot(normN,normL)))+light.cl.z*triangle.cp.z*pow(std::max(0.f,dot(normH,normN)),256);
	//}
	//cout << doingShadow << endl;
	//if(doingShadow==false){
	//c.x=triangle.cr.x*(light.ca.x+light.cl.x*std::max(dot(-normN,normL),dot(normN,normL)))+light.cl.x*triangle.cp.x*pow(std::max(dot(-normN,normL),dot(normH,normN)),12);
	//c.y=triangle.cr.y*(light.ca.y+light.cl.y*std::max(dot(-normN,normL),dot(normN,normL)))+light.cl.y*triangle.cp.y*pow(std::max(dot(-normN,normL),dot(normH,normN)),12);
	//c.z=triangle.cr.z*(light.ca.z+light.cl.z*std::max(dot(-normN,normL),dot(normN,normL)))+light.cl.z*triangle.cp.z*pow(std::max(dot(-normN,normL),dot(normH,normN)),12);
	//}

	//c.x=triangle.cr.x;c.y=triangle.cr.y;c.z=triangle.cr.z;

	//vec3 shadowRay = vec3(light.x,light.y,light.z)-intersectionPoint;
	//if(shadowCounter==1){
		//rayIntersect(shadowRay,true);
		//doingShadow = true;
	//}
	//if(objectHit==true)
	//{
		//c.x=0.2;
		//c.y=0.2;
		//c.z=0.2;
	//}

	return(closestIntersection);
}


int rayIntersect(const vec3& normalViewRay, bool shadow)
{

	light1.x=0;
	light1.y=2.7;
	light1.z=-7.75;
	//light1.y=0;
	//light1.z=0;
	light1.cl = vec3(1,1,1);
	light1.ca = vec3(.8,.8,.8);

	sphere1.x=0.9;
	sphere1.y=-1.925;
	sphere1.z=-6.69;
	sphere1.r=0.825;
	sphere1.cr=vec3(0,0.5,0);
	sphere1.cp=vec3(0.8,0.8,0.8);

	//Sphere sphere2;
	//sphere2.x=0.9;
	//sphere2.y=-1.925;
	//sphere2.z=-9.69;
	//sphere2.r=3;
	//sphere2.cr=vec3(0,0.5,0.5);
	//sphere2.cp=vec3(0.8,0.8,0.8);

	triangle1.x1=-0.4;
	triangle1.x2=-0.93;
	triangle1.x3=0.11;
	triangle1.y1=-2.75;
	triangle1.y2=0.55;
	triangle1.y3=-2.75;
	triangle1.z1=-9.55;
	triangle1.z2=-8.51;
	triangle1.z3=-7.98;
	triangle1.cr=vec3(0,0.5,0.5);
	triangle1.cp=vec3(0.5,0.5,0.5);

	triangle2.x1=0.11;
	triangle2.x2=-0.93;
	triangle2.x3=-1.46;
	triangle2.y1=-2.75;
	triangle2.y2=0.55;
	triangle2.y3=-2.75;
	triangle2.z1=-7.98;
	triangle2.z2=-8.51;
	triangle2.z3=-7.47;
	triangle2.cr=vec3(0,0.5,0.5);
	triangle2.cp=vec3(0.5,0.5,0.5);

	triangle3.x1=-1.46;
	triangle3.x2=-0.93;
	triangle3.x3=-1.97;
	triangle3.y1=-2.75;
	triangle3.y2=0.55;
	triangle3.y3=-2.75;
	triangle3.z1=-7.47;
	triangle3.z2=-8.51;
	triangle3.z3=-9.04;
	triangle3.cr=vec3(0,0.5,0.5);
	triangle4.cp=vec3(0.5,0.5,0.5);

	triangle4.x1=-1.97;
	triangle4.x2=-0.93;
	triangle4.x3=-0.4;
	triangle4.y1=-2.75;
	triangle4.y2=0.55;
	triangle4.y3=-2.75;
	triangle4.z1=-9.04;
	triangle4.z2=-8.51;
	triangle4.z3=-9.54;
	triangle4.cr=vec3(0,0.5,0.5);
	triangle4.cp=vec3(0.5,0.5,0.5);

	triangle5.x1=2.75;
	triangle5.x2=2.75;
	triangle5.x3=-2.75;
	triangle5.y1=2.75;
	triangle5.y2=2.75;
	triangle5.y3=2.75;
	triangle5.z1=-10.5;
	triangle5.z2=-5;
	triangle5.z3=-5;
	triangle5.cr=vec3(0.1,0.7,0.7);
	triangle5.cp=vec3(1,1,1);

	triangle6.x1=-2.75;
	triangle6.x2=2.75;
	triangle6.x3=-2.75;
	triangle6.y1=2.75;
	triangle6.y2=2.75;
	triangle6.y3=2.75;
	triangle6.z1=-10.5;
	triangle6.z2=-10.5;
	triangle6.z3=-5;
	triangle6.cr=vec3(0.1,0.7,0.7);
	triangle6.cp=vec3(1,1,1);

	triangle7.x1=2.75;
	triangle7.x2=2.75;
	triangle7.x3=2.75;
	triangle7.y1=2.75;
	triangle7.y2=2.75;
	triangle7.y3=-2.75;
	triangle7.z1=-5;
	triangle7.z2=-10.5;
	triangle7.z3=-10.5;
	triangle7.cr=vec3(0,0.8,0);
	triangle7.cp=vec3(0.1,0.1,0.1);

	triangle8.x1=2.75;
	triangle8.x2=2.75;
	triangle8.x3=2.75;
	triangle8.y1=-2.75;
	triangle8.y2=2.75;
	triangle1.z3=-7.98;
	triangle1.cr=vec3(0,0.5,0.5);
	triangle1.cp=vec3(0.5,0.5,0.5);
	triangle8.y3=-2.75;
	triangle8.z1=-5;
	triangle8.z2=-5;
	triangle8.z3=-10.5;
	triangle8.cr=vec3(0,0.8,0);
	triangle8.cp=vec3(0.1,0.1,0.1);

	triangle9.x1=-2.75;
	triangle9.x2=-2.75;
	triangle9.x3=-2.75;
	triangle9.y1=-2.75;
	triangle9.y2=-2.75;
	triangle9.y3=2.75;
	triangle9.z1=-5;
	triangle9.z2=-10.5;
	triangle9.z3=-10.5;
	triangle9.cr=vec3(0.8,0,0);
	triangle9.cp=vec3(0.1,0.1,0.1);

	triangle10.x1=-2.75;
	triangle10.x2=-2.75;
	triangle10.x3=-2.75;
	triangle10.y1=2.75;
	triangle10.y2=-2.75;
	triangle10.y3=2.75;
	triangle10.z1=-5;
	triangle10.z2=-5;
	triangle10.z3=-10.5;
	triangle10.cr=vec3(0.8,0,0);
	triangle10.cp=vec3(0.1,0.1,0.1);

	triangle11.x1=2.75;
	triangle11.x2=2.75;
	triangle11.x3=-2.75;
	triangle11.y1=-2.75;
	triangle11.y2=-2.75;
	triangle11.y3=-2.75;
	triangle11.z1=-5;
	triangle11.z2=-10.5;
	triangle11.z3=-10.5;
	triangle11.cr=vec3(0.8,0,0.8);
	triangle11.cp=vec3(0.1,0.1,0.1);

	triangle12.x1=-2.75;
	triangle12.x2=2.75;
	triangle12.x3=-2.75;
	triangle12.y1=-2.75;
	triangle12.y2=-2.75;
	triangle12.y3=-2.75;
	triangle12.z1=-5;
	triangle12.z2=-5;
	triangle12.z3=-10.5;
	triangle12.cr=vec3(0.8,0,0.8);
	triangle12.cp=vec3(0.1,0.1,0.1);

	plane1.xn=0;
	plane1.xq=0;
	plane1.yn=0;
	plane1.yq=0;
	plane1.zn=1;
	plane1.zq=-10.5;
	plane1.cr=vec3(0,0.6,0.4);
	plane1.cp=vec3(0.2,0.2,0.2);

	vec4 circSect;
	vec4 triSect;
	vec4 plaSect;
	int minT;

	shadowCounter++;

			bool initial = true;

			circSect = circleIntersect(normalViewRay,sphere1,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			/*circSect = circleIntersect(normalViewRay,sphere2,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					if(shadowCounter==2)
						return(-1);
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}*/

			//1,4,3,2
			triSect = triangleIntersect(normalViewRay,triangle2,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle1,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle3,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle4,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle5,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle6,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle7,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle8,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle9,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle10,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle11,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangle12,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}

			plaSect = planeIntersect(normalViewRay,plane1,light1);
			if((plaSect.x!=0)||(plaSect.y!=0)||(plaSect.z!=0)){
				if((plaSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=plaSect.w;
					initial = false;}}
			}
			return(minT);
}

int rayIntersect2(const vec3& normalViewRay, bool shadow)
{

	light2.x=4;
	light2.y=6;
	light2.z=1;
	//light2.y=0;
	//light2.z=0;
	light2.cl = vec3(0.4,0.4,0.4);
	light2.ca = vec3(0.6,0.6,0.6);

	sphereA.x=1;
	sphereA.y=-0.5;
	sphereA.z=-3.5;
	sphereA.r=0.5;
	sphereA.cr=vec3(1,1,0);
	sphereA.cp=vec3(0.4,0.4,0.4);

	sphereB.x=0;
	sphereB.y=1;
	sphereB.z=-5;
	sphereB.r=0.5;
	sphereB.cr=vec3(0.4,0.4,0.4);
	sphereB.cp=vec3(0.5,0.5,0.5);

	sphereC.x=-0.8;
	sphereC.y=-0.75;
	sphereC.z=-4;
	sphereC.r=0.25;
	sphereC.cr=vec3(1,0,1);
	sphereC.cp=vec3(0.8,0.8,0.8);

	//green cone
	triangleA.x1=0;
	triangleA.x2=0;
	triangleA.x3=0.4;
	triangleA.y1=-1;
	triangleA.y2=0.5;
	triangleA.y3=-1;
	triangleA.z1=-5.8;
	triangleA.z2=-5;
	triangleA.z3=-5.693;
	triangleA.cr=vec3(0,1,0);
	triangleA.cp=vec3(0.5,0.5,0.5);

	triangleB.x1=0.4;
	triangleB.x2=0;
	triangleB.x3=0.6298;
	triangleB.y1=-1;
	triangleB.y2=0.6;
	triangleB.y3=-1;
	triangleB.z1=-5.693;
	triangleB.z2=-5;
	triangleB.z3=-5.4;
	triangleB.cr=vec3(0,1,0);
	triangleB.cp=vec3(0.5,0.5,0.5);

	triangleC.x1=0.6928;
	triangleC.x2=0;
	triangleC.x3=0.8;
	triangleC.y1=-1;
	triangleC.y2=0.6;
	triangleC.y3=-1;
	triangleC.z1=-5.4;
	triangleC.z2=-5;
	triangleC.z3=-5;
	triangleC.cr=vec3(0,1,0);
	triangleC.cp=vec3(0.5,0.5,0.5);

	triangleD.x1=0.8;
	triangleD.x2=0;
	triangleD.x3=0.6928;
	triangleD.y1=-1;
	triangleD.y2=0.6;
	triangleD.y3=-1;
	triangleD.z1=-5;
	triangleD.z2=-5;
	triangleD.z3=-4.6;
	triangleD.cr=vec3(0,1,0);
	triangleD.cp=vec3(0.5,0.5,0.5);

	triangleE.x1=0.6928;
	triangleE.x2=0;
	triangleE.x3=0.4;
	triangleE.y1=-1;
	triangleE.y2=0.6;
	triangleE.y3=-1;
	triangleE.z1=-4.6;
	triangleE.z2=-5;
	triangleE.z3=-4.307;
	triangleE.cr=vec3(0,1,0);
	triangleE.cp=vec3(1,1,1);

	triangleG.x1=0.4;
	triangleG.x2=0;
	triangleG.x3=0;
	triangleG.y1=-1;
	triangleG.y2=0.6;
	triangleG.y3=-1;
	triangleG.z1=-4.307;
	triangleG.z2=-5;
	triangleG.z3=-4.2;
	triangleG.cr=vec3(0,1,0);
	triangleG.cp=vec3(0.1,0.1,0.1);

	triangleF.x1=0;
	triangleF.x2=0;
	triangleF.x3=-0.4;
	triangleF.y1=-1;
	triangleF.y2=0.6;
	triangleF.y3=-1;
	triangleF.z1=-4.2;
	triangleF.z2=-5;
	triangleF.z3=-4.307;
	triangleF.cr=vec3(0,1,0);
	triangleF.cp=vec3(0.1,0.1,0.1);

	triangleH.x1=-0.4;
	triangleH.x2=0;
	triangleH.x3=-0.6928;
	triangleH.y1=-1;
	triangleH.y2=0.6;
	triangleH.y3=-1;
	triangleH.z1=-4.307;
	triangleH.z2=-5;
	triangleH.z3=-4.6;
	triangleH.cr=vec3(0,1,0);
	triangleH.cp=vec3(0.1,0.1,0.1);

	triangleI.x1=-0.6928;
	triangleI.x2=0;
	triangleI.x3=-0.8;
	triangleI.y1=-1;
	triangleI.y2=0.6;
	triangleI.y3=-1;
	triangleI.z1=-4.6;
	triangleI.z2=-5;
	triangleI.z3=-5;
	triangleI.cr=vec3(0,1,0);
	triangleI.cp=vec3(0.1,0.1,0.1);

	triangleJ.x1=-0.8;
	triangleJ.x2=0;
	triangleJ.x3=-0.6928;
	triangleJ.y1=-1;
	triangleJ.y2=0.6;
	triangleJ.y3=-1;
	triangleJ.z1=-5;
	triangleJ.z2=-5;
	triangleJ.z3=-5.4;
	triangleJ.cr=vec3(0,1,0);
	triangleJ.cp=vec3(0.1,0.1,0.1);

	triangleK.x1=-0.6928;
	triangleK.x2=0;
	triangleK.x3=-0.4;
	triangleK.y1=-1;
	triangleK.y2=0.6;
	triangleK.y3=-1;
	triangleK.z1=-5.4;
	triangleK.z2=-5;
	triangleK.z3=-5.693;
	triangleK.cr=vec3(0,1,0);
	triangleK.cp=vec3(0.1,0.1,0.1);

	triangleL.x1=-0.4;
	triangleL.x2=0;
	triangleL.x3=0;
	triangleL.y1=-1;
	triangleL.y2=0.6;
	triangleL.y3=-1;
	triangleL.z1=-5.693;
	triangleL.z2=-5;
	triangleL.z3=-5.8;
	triangleL.cr=vec3(0,1,0);
	triangleL.cp=vec3(0.1,0.1,0.1);


	//red thing
	triangleN.x1=-1.276;
	triangleN.x2=-2;
	triangleN.x3=-1.276;
	triangleN.y1=-0.4472;
	triangleN.y2=-1;
	triangleN.y3=-0.4472;
	triangleN.z1=-6.474;
	triangleN.z2=-7;
	triangleN.z3=-7.526;
	triangleN.cr=vec3(0.7,0,0);
	triangleN.cp=vec3(0.5,0.5,0.5);

	triangleO.x1=-2;
	triangleO.x2=-2.276;
	triangleO.x3=-2.894;
	triangleO.y1=-1;
	triangleO.y2=-0.4472;
	triangleO.y3=-0.4472;
	triangleO.z1=-7;
	triangleO.z2=-6.149;
	triangleO.z3=-7;
	triangleO.cr=vec3(0.7,0,0);
	triangleO.cp=vec3(0.5,0.5,0.5);

	triangleP.x1=-2;
	triangleP.x2=-2.894;
	triangleP.x3=-2.276;
	triangleP.y1=-1;
	triangleP.y2=-0.4472;
	triangleP.y3=-0.4472;
	triangleP.z1=-7;
	triangleP.z2=-7;
	triangleP.z3=-7.851;
	triangleP.cr=vec3(0.7,0,0);
	triangleP.cp=vec3(0.5,0.5,0.5);

	triangleQ.x1=-2;
	triangleQ.x2=-2.276;
	triangleQ.x3=-1.276;
	triangleQ.y1=-1;
	triangleQ.y2=-0.4472;
	triangleQ.y3=-0.4472;
	triangleQ.z1=-7;
	triangleQ.z2=-7.851;
	triangleQ.z3=-7.526;
	triangleQ.cr=vec3(0.7,0,0);
	triangleQ.cp=vec3(1,1,1);

	triangleR.x1=-1.276;
	triangleR.x2=-1.276;
	triangleR.x3=-1.276;
	triangleR.y1=-0.4472;
	triangleR.y2=-0.4472;
	triangleR.y3=0.4472;
	triangleR.z1=-6.474;
	triangleR.z2=-7.526;
	triangleR.z3=-7;
	triangleR.cr=vec3(0.7,0,0);
	triangleR.cp=vec3(1,1,1);

	triangleS.x1=-2.276;
	triangleS.x2=-1.276;
	triangleS.x3=-1.724;
	triangleS.y1=-0.4472;
	triangleS.y2=-0.4472;
	triangleS.y3=0.4472;
	triangleS.z1=-6.149;
	triangleS.z2=-6.474;
	triangleS.z3=-6.149;
	triangleS.cr=vec3(0.7,0,0);
	triangleS.cp=vec3(0.3,0.3,0.3);

	triangleT.x1=-2.894;
	triangleT.x2=-2.276;
	triangleT.x3=-2.724;
	triangleT.y1=-0.4472;
	triangleT.y2=-0.4472;
	triangleT.y3=0.4472;
	triangleT.z1=-7;
	triangleT.z2=-6.149;
	triangleT.z3=-6.474;
	triangleT.cr=vec3(0.7,0,0);
	triangleT.cp=vec3(0.3,0.3,0.3);

	triangleU.x1=-2.276;
	triangleU.x2=-2.894;
	triangleU.x3=-2.724;
	triangleU.y1=-0.4472;
	triangleU.y2=-0.4472;
	triangleU.y3=0.4472;
	triangleU.z1=-7.851;
	triangleU.z2=-7;
	triangleU.z3=-7.526;
	triangleU.cr=vec3(0.8,0,0);
	triangleU.cp=vec3(0.3,0.3,0.3);

	triangleV.x1=-1.276;
	triangleV.x2=-2.276;
	triangleV.x3=-2.724;
	triangleV.y1=-0.4472;
	triangleV.y2=-0.4472;
	triangleV.y3=0.4472;
	triangleV.z1=-7.526;
	triangleV.z2=-7.851;
	triangleV.z3=-7.851;
	triangleV.cr=vec3(0.8,0,0);
	triangleV.cp=vec3(0.3,0.3,0.3);

	triangleW.x1=-1.276;
	triangleW.x2=-1.106;
	triangleW.x3=-1.724;
	triangleW.y1=-0.4472;
	triangleW.y2=0.4472;
	triangleW.y3=0.4472;
	triangleW.z1=-6.474;
	triangleW.z2=-7;
	triangleW.z3=-6.149;
	triangleW.cr=vec3(0.8,0,0);
	triangleW.cp=vec3(0.3,0.3,0.3);

	triangleX.x1=-1.276;
	triangleX.x2=-1.106;
	triangleX.x3=-1.724;
	triangleX.y1=-0.4472;
	triangleX.y2=0.4472;
	triangleX.y3=0.4472;
	triangleX.z1=-6.474;
	triangleX.z2=-7;
	triangleX.z3=-6.149;
	triangleX.cr=vec3(0.8,0,0);
	triangleX.cp=vec3(0.3,0.3,0.3);

	triangleY.x1=-2.276;
	triangleY.x2=-1.724;
	triangleY.x3=-2.724;
	triangleY.y1=-0.4472;
	triangleY.y2=0.4472;
	triangleY.y3=0.4472;
	triangleY.z1=-6.149;
	triangleY.z2=-6.149;
	triangleY.z3=-6.474;
	triangleY.cr=vec3(0.8,0,0);
	triangleY.cp=vec3(0.3,0.3,0.3);

	triangleZ.x1=-2.894;
	triangleZ.x2=-2.724;
	triangleZ.x3=-2.724;
	triangleZ.y1=-0.4472;
	triangleZ.y2=0.4472;
	triangleZ.y3=0.4472;
	triangleZ.z1=-7;
	triangleZ.z2=-6.474;
	triangleZ.z3=-7.526;
	triangleZ.cr=vec3(0.8,0,0);
	triangleZ.cp=vec3(0.3,0.3,0.3);

	triangleAA.x1=-2.276;
	triangleAA.x2=-2.724;
	triangleAA.x3=-1.724;
	triangleAA.y1=-0.4472;
	triangleAA.y2=0.4472;
	triangleAA.y3=0.4472;
	triangleAA.z1=-7.851;
	triangleAA.z2=-7.526;
	triangleAA.z3=-7.851;
	triangleAA.cr=vec3(0.8,0,0);
	triangleAA.cp=vec3(0.3,0.3,0.3);

	triangleBB.x1=-1.276;
	triangleBB.x2=-1.724;
	triangleBB.x3=-1.106;
	triangleBB.y1=-0.4472;
	triangleBB.y2=0.4472;
	triangleBB.y3=0.4472;
	triangleBB.z1=-7.526;
	triangleBB.z2=-7.851;
	triangleBB.z3=-7;
	triangleBB.cr=vec3(0.8,0,0);
	triangleBB.cp=vec3(0.3,0.3,0.3);

	triangleCC.x1=-1.724;
	triangleCC.x2=-1.106;
	triangleCC.x3=-2;
	triangleCC.y1=0.4472;
	triangleCC.y2=0.4472;
	triangleCC.y3=1;
	triangleCC.z1=-6.149;
	triangleCC.z2=-7;
	triangleCC.z3=-7;
	triangleCC.cr=vec3(0.8,0,0);
	triangleCC.cp=vec3(0.3,0.3,0.3);

	triangleDD.x1=-2.724;
	triangleDD.x2=-1.724;
	triangleDD.x3=-2;
	triangleDD.y1=0.4472;
	triangleDD.y2=0.4472;
	triangleDD.y3=1;
	triangleDD.z1=-6.474;
	triangleDD.z2=-6.149;
	triangleDD.z3=-7;
	triangleDD.cr=vec3(0.8,0,0);
	triangleDD.cp=vec3(0.3,0.3,0.3);

	triangleEE.x1=-2.724;
	triangleEE.x2=-2.724;
	triangleEE.x3=-2;
	triangleEE.y1=0.4472;
	triangleEE.y2=0.4472;
	triangleEE.y3=1;
	triangleEE.z1=-7.526;
	triangleEE.z2=-6.474;
	triangleEE.z3=-7;
	triangleEE.cr=vec3(0.8,0,0);
	triangleEE.cp=vec3(0.3,0.3,0.3);

	triangleFF.x1=-1.724;
	triangleFF.x2=-2.724;
	triangleFF.x3=-2;
	triangleFF.y1=0.4472;
	triangleFF.y2=0.4472;
	triangleFF.y3=1;
	triangleFF.z1=-7.851;
	triangleFF.z2=-7.526;
	triangleFF.z3=-7;
	triangleFF.cr=vec3(0.8,0,0);
	triangleFF.cp=vec3(0.3,0.3,0.3);

	triangleGG.x1=-1.106;
	triangleGG.x2=-1.724;
	triangleGG.x3=-2;
	triangleGG.y1=0.4472;
	triangleGG.y2=0.4472;
	triangleGG.y3=1;
	triangleGG.z1=-7;
	triangleGG.z2=-7.851;
	triangleGG.z3=-7;
	triangleGG.cr=vec3(0.8,0,0);
	triangleGG.cp=vec3(0.3,0.3,0.3);

	//floor
	planeA.xn=0;
	planeA.xq=0;
	planeA.yn=1;
	planeA.yq=-1;
	planeA.zn=0;
	planeA.zq=0;
	planeA.cr=vec3(0.8,0.8,0.8);
	planeA.cp=vec3(0.2,0.2,0.2);

	//wall
	planeB.xn=0;
	planeB.xq=0;
	planeB.yn=0;
	planeB.yq=0;
	planeB.zn=1;
	planeB.zq=-12;
	planeB.cr=vec3(0,0.7,0.7);
	planeB.cp=vec3(0.2,0.2,0.2);

	vec4 circSect;
	vec4 triSect;
	vec4 plaSect;
	int minT;

	shadowCounter++;

			bool initial = true;

			circSect = circleIntersect(normalViewRay,sphereA,light2);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereB,light2);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereC,light2);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			triSect = triangleIntersect(normalViewRay,triangleA,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}

			triSect = triangleIntersect(normalViewRay,triangleB,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}

			triSect = triangleIntersect(normalViewRay,triangleC,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}

			triSect = triangleIntersect(normalViewRay,triangleD,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleE,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleF,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleG,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleH,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleI,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleJ,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleK,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleL,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			/*triSect = triangleIntersect(normalViewRay,triangleM,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}*/
			triSect = triangleIntersect(normalViewRay,triangleN,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleP,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleO,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleR,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleQ,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleS,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleT,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleU,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleV,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleW,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleX,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleY,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleZ,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleAA,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleBB,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleCC,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleDD,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleEE,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleFF,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}
			triSect = triangleIntersect(normalViewRay,triangleGG,light2);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}

			plaSect = planeIntersect(normalViewRay,planeA,light2);
			if((plaSect.x!=0)||(plaSect.y!=0)||(plaSect.z!=0)){
				if((plaSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					if(plaSect.z<0){
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=plaSect.w;
					initial = false;}}}
			}

			plaSect = planeIntersect(normalViewRay,planeB,light2);
			if((plaSect.x!=0)||(plaSect.y!=0)||(plaSect.z!=0)){
				if((plaSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=plaSect.w;
					initial = false;}}
			}
			return(minT);
}

int rayIntersect3(const vec3& normalViewRay, bool shadow)
{

	light2.x=0;
	light2.y=2.7;
	light2.z=-7.75;
	//light1.y=0;
	//light1.z=0;
        light2.cl = vec3(0.7,0.7,0.7);
	light2.ca = vec3(0.5,0.5,0.5);

	sphereD.x=1.5;
	sphereD.y=-1.925;
	sphereD.z=-9.69;
	sphereD.r=0.5;
	sphereD.cr=vec3(0,0.8,0);
	sphereD.cp=vec3(0.8,0.8,0.8);

	sphereE.x=0.5;
	sphereE.y=-1.925;
	sphereE.z=-9.69;
	sphereE.r=0.5;
	sphereE.cr=vec3(0,0.8,0);
	sphereE.cp=vec3(0.8,0.8,0.8);

	sphereF.x=-0.5;
	sphereF.y=-1.925;
	sphereF.z=-9.69;
	sphereF.r=0.5;
	sphereF.cr=vec3(0,0.8,0);
	sphereF.cp=vec3(0.8,0.8,0.8);

	sphereG.x=-1.5;
	sphereG.y=-1.925;
	sphereG.z=-9.69;
	sphereG.r=0.5;
	sphereG.cr=vec3(0,0.8,0);
	sphereG.cp=vec3(0.8,0.8,0.8);

	sphereH.x=1.5;
	sphereH.y=-.925;
	sphereH.z=-9.69;
	sphereH.r=0.5;
	sphereH.cr=vec3(0,0.8,0);
	sphereH.cp=vec3(0.8,0.8,0.8);

	sphereI.x=0.5;
	sphereI.y=-.925;
	sphereI.z=-9.69;
	sphereI.r=0.5;
	sphereI.cr=vec3(0,0.8,0);
	sphereI.cp=vec3(0.8,0.8,0.8);

	sphereJ.x=-0.5;
	sphereJ.y=-.925;
	sphereJ.z=-9.69;
	sphereJ.r=0.5;
	sphereJ.cr=vec3(0,0.8,0);
	sphereJ.cp=vec3(0.8,0.8,0.8);

	sphereK.x=-1.5;
	sphereK.y=-.925;
	sphereK.z=-9.69;
	sphereK.r=0.5;
	sphereK.cr=vec3(0,0.8,0);
	sphereK.cp=vec3(0.8,0.8,0.8);

	sphereL.x=0.5;
	sphereL.y=0.075;
	sphereL.z=-9.69;
	sphereL.r=0.5;
	sphereL.cr=vec3(0,0.8,0);
	sphereL.cp=vec3(0.8,0.8,0.8);

	sphereM.x=-0.5;
	sphereM.y=0.075;
	sphereM.z=-9.69;
	sphereM.r=0.5;
	sphereM.cr=vec3(0,0.8,0);
	sphereM.cp=vec3(0.8,0.8,0.8);

	sphereN.x=-1.5;
	sphereN.y=0.075;
	sphereN.z=-9.69;
	sphereN.r=0.5;
	sphereN.cr=vec3(0,0.8,0);
	sphereN.cp=vec3(0.8,0.8,0.8);

	sphereO.x=1.5;
	sphereO.y=0.075;
	sphereO.z=-9.69;
	sphereO.r=0.5;
	sphereO.cr=vec3(0,0.8,0);
	sphereO.cp=vec3(0.8,0.8,0.8);

	sphereP.x=1.5;
	sphereP.y=1.075;
	sphereP.z=-9.69;
	sphereP.r=0.5;
	sphereP.cr=vec3(0,0.8,0);
	sphereP.cp=vec3(0.8,0.8,0.8);

	sphereQ.x=0.5;
	sphereQ.y=1.075;
	sphereQ.z=-9.69;
	sphereQ.r=0.5;
	sphereQ.cr=vec3(0,0.8,0);
	sphereQ.cp=vec3(0.8,0.8,0.8);

	sphereR.x=-0.5;
	sphereR.y=1.075;
	sphereR.z=-9.69;
	sphereR.r=0.5;
	sphereR.cr=vec3(0,0.8,0);
	sphereR.cp=vec3(0.8,0.8,0.8);

	sphereS.x=-1.5;
	sphereS.y=1.075;
	sphereS.z=-9.69;
	sphereS.r=0.5;
	sphereS.cr=vec3(0,0.8,0);
	sphereS.cp=vec3(0.8,0.8,0.8);

        //level 2
        sphereT.x=-1;
        sphereT.y=-1.475;
        sphereT.z=-8.69;
	sphereT.r=0.5;
        sphereT.cr=vec3(0.1,0.8,0.8);
	sphereT.cp=vec3(0.8,0.8,0.8);

        sphereU.x=0;
        sphereU.y=-1.475;
        sphereU.z=-8.69;
        sphereU.r=0.5;
        sphereU.cr=vec3(0.1,0.8,0.8);
        sphereU.cp=vec3(0.8,0.8,0.8);

        sphereV.x=1;
        sphereV.y=-1.475;
        sphereV.z=-8.69;
        sphereV.r=0.5;
        sphereV.cr=vec3(0.1,0.8,0.8);
        sphereV.cp=vec3(0.8,0.8,0.8);

        sphereW.x=-1;
        sphereW.y=-.475;
        sphereW.z=-8.69;
        sphereW.r=0.5;
        sphereW.cr=vec3(0.1,0.8,0.8);
        sphereW.cp=vec3(0.8,0.8,0.8);

        sphereX.x=0;
        sphereX.y=-.475;
        sphereX.z=-8.69;
        sphereX.r=0.5;
        sphereX.cr=vec3(0.1,0.8,0.8);
        sphereX.cp=vec3(0.8,0.8,0.8);

        sphereY.x=1;
        sphereY.y=-.475;
        sphereY.z=-8.69;
        sphereY.r=0.5;
        sphereY.cr=vec3(0.1,0.8,0.8);
        sphereY.cp=vec3(0.8,0.8,0.8);

        sphereZ.x=-1;
        sphereZ.y=.475;
        sphereZ.z=-8.69;
        sphereZ.r=0.5;
        sphereZ.cr=vec3(0.1,0.8,0.8);
        sphereZ.cp=vec3(0.8,0.8,0.8);

        spherea.x=0;
        spherea.y=.475;
        spherea.z=-8.69;
        spherea.r=0.5;
        spherea.cr=vec3(0.1,0.8,0.8);
        spherea.cp=vec3(0.8,0.8,0.8);

        sphereb.x=1;
        sphereb.y=.475;
        sphereb.z=-8.69;
        sphereb.r=0.5;
        sphereb.cr=vec3(0.1,0.8,0.8);
        sphereb.cp=vec3(0.8,0.8,0.8);


        //level 3
        spherec.x=-0.5;
        spherec.y=-0.975;
        spherec.z=-7.69;
        spherec.r=0.5;
        spherec.cr=vec3(0.9,0.1,0);
        spherec.cp=vec3(0.8,0.8,0.8);

        sphered.x=0.5;
        sphered.y=-0.975;
        sphered.z=-7.69;
        sphered.r=0.5;
        sphered.cr=vec3(0.9,0.1,0);
        sphered.cp=vec3(0.8,0.8,0.8);

        spheree.x=-0.5;
        spheree.y=0.025;
        spheree.z=-7.69;
        spheree.r=0.5;
        spheree.cr=vec3(0.9,0.1,0);
        spheree.cp=vec3(0.8,0.8,0.8);

        spheref.x=0.5;
        spheref.y=0.025;
        spheref.z=-7.69;
        spheref.r=0.5;
        spheref.cr=vec3(0.9,0.1,0);
        spheref.cp=vec3(0.8,0.8,0.8);

        //Final level
        sphereg.x=0;
        sphereg.y=-0.475;
        sphereg.z=-6.69;
        sphereg.r=0.5;
        sphereg.cr=vec3(0.8,0.8,0.8);
        sphereg.cp=vec3(0.8,0.8,0.8);

	planeB1.xn=0;
	planeB1.xq=0;
	planeB1.yn=0;
	planeB1.yq=0;
	planeB1.zn=1;
	planeB1.zq=-10.5;
        planeB1.cr=vec3(0.3,0.3,0.6);
	planeB1.cp=vec3(0.2,0.2,0.2);

	triangleF1.x1=2.75;
        triangleF1.x2=2.75;
	triangleF1.x3=-2.75;
	triangleF1.y1=-2.75;
	triangleF1.y2=-2.75;
	triangleF1.y3=-2.75;
        triangleF1.z1=0;
	triangleF1.z2=-10.5;
	triangleF1.z3=-10.5;
        triangleF1.cr=vec3(0.9,0.7,0.7);
	triangleF1.cp=vec3(0.1,0.1,0.1);

	triangleF2.x1=-2.75;
	triangleF2.x2=2.75;
        triangleF2.x3=-2.75;
	triangleF2.y1=-2.75;
	triangleF2.y2=-2.75;
	triangleF2.y3=-2.75;
	triangleF2.z1=0;
        triangleF2.z2=-5;
	triangleF2.z3=-10.5;
        triangleF2.cr=vec3(0.9,0.7,0.7);
	triangleF2.cp=vec3(0.1,0.1,0.1);

	vec4 circSect;
	vec4 triSect;
	vec4 plaSect;
	int minT;

	shadowCounter++;
			bool initial = true;

			circSect = circleIntersect(normalViewRay,sphereD,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereE,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereF,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereG,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereH,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereI,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereJ,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereK,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereL,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereM,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereN,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereO,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereP,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereQ,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereR,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}			circSect = circleIntersect(normalViewRay,sphereS,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereT,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereU,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereV,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereW,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereX,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereY,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereZ,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,spherea,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereb,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,spherec,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphered,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,spheree,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,spheref,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereg,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			circSect = circleIntersect(normalViewRay,sphereh,light1);
			//cout << circSect.x << " " << circSect.y << " " << circSect.z << endl;
			if((circSect.x!=0)||(circSect.y!=0)||(circSect.z!=0)){
				if((circSect.w<minT)||(initial)){
					//cout << oldMinT << " " <<  minT <<  endl;
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						//printVec3("c", c);
						//cout << "sphere shadow intersect" << endl;
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=circSect.w;
					initial = false;}}
			}

			triSect = triangleIntersect(normalViewRay,triangleF1,light1);
			if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
				if((triSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=triSect.w;
					initial = false;}}
			}

                        triSect = triangleIntersect(normalViewRay,triangleF2,light1);
                        if((triSect.x!=0)||(triSect.y!=0)||(triSect.z!=0)){
                                if((triSect.w<minT)||(initial)){
                                        if((shadowCounter==2)&(oldMinT/minT>0.001)){
                                                return(-1);}
                                        else{
                                        color.x = c.x;color.y = c.y;color.z = c.z;
                                        minT=triSect.w;
                                        initial = false;}}
                        }

			plaSect = planeIntersect(normalViewRay,planeB1,light1);
			if((plaSect.x!=0)||(plaSect.y!=0)||(plaSect.z!=0)){
				if((plaSect.w<minT)||(initial)){
					if((shadowCounter==2)&(oldMinT/minT>0.001)){
						return(-1);}
					else{
					color.x = c.x;color.y = c.y;color.z = c.z;
					minT=plaSect.w;
					initial = false;}}
			}
			return(minT);
}


// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        float windowSize = 2000;
	window = glfwCreateWindow(windowSize, windowSize, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	//Intialize GLAD
	#ifndef LAB_LINUX
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}
	#endif

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	MyShader shader;
	if (!InitializeShaders(&shader)) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// call function to create and fill buffers with geometry data
	MyGeometry geometry;
	if (!InitializeGeometry(&geometry))
		cout << "Program failed to intialize geometry!" << endl;
	//------------------------------------GLM STUFF
	//vec3 v(5,5,0);
	//printVec3("v",v);
	//vec3 n(0,5,0);
	//printVec3("n",n);
	//printVec3("n+v",n+v);
	//printVec3("n-v",n-v);
	//cout << "Length of v " << length(v) << endl;		//Two Norm

	//normalize vector
	//n = normalize(n);
	//printVec3("unit length n", n);

	//cross product
	//vec3 binormal = cross(v,n);
	//printVec3("v x n", binormal);
	//printVec3("n x v", cross(n,v));

	//dot product
	//std::cout << "binormal dot v " << dot(binormal,v) << endl;

	//reflect
	//printVec3("v refelcted about n", reflect(v,n));	//I  2.0 + dot(N,I)*N
	//------------------------------------

	//********************************************
	//************Assignment 4********************
	//********************************************
	//Scene 1
	//Image buffer stuff
	ImageBuffer image;
	image.Initialize();	//Init the image
	//int counter=0;
	float fov = 60;
	float z = (-1)*(windowSize/2)/tan((fov/360)*PI);
	float minZ;
	int xGL;
	int yGL;

	light1.x=0;
	light1.y=2.7;
	light1.z=-7.75;
	//light1.y=0;
	//light1.z=0;
	light1.cl = vec3(0.3,0.3,0.3);
	light1.ca = vec3(0.6,0.6,0.6);

	vec3 normalViewRay;

	oldMinT = 0;

	vec3 shadowRay;

         for(int y=0;y<image.Height();++y)
         {
                yGL = y-(windowSize/2);
                for(int x=0;x<image.Width();++x)
                {
                        origin = vec3(0,0,0);
                        minT = 0;
                        doingShadow=false;
                        shadow = false;
                        shadowCounter=0;
                        objectHit=false;
                        color = vec3(0,0,0);
                        c = vec3(0,0,0);
                        xGL = x-(windowSize/2);

                        vec3 viewRay(xGL,yGL,z);
                        normalViewRay = normalize(viewRay);

                        float t = rayIntersect(normalViewRay,false);

                        oldMinT = t;
                        doingShadow=true;
                        origin = vec3(light1.x,light1.y,light1.z);
                        //shadowRay = -vec3(light1.x,light1.y,light1.z) - (normalViewRay*t);
                        //shadowRay = normalize(shadowRay);
                        //if(rayIntersect(shadowRay,true)==-1){
                          //      color.x=0.15;color.y=0.15;color.z=0.15;
                        //}
                        image.SetPixel(x,y,color);
                }
         }
	image.SaveToFile("scene1.png");

	//Scene 2
	//Image buffer stuff
	ImageBuffer image2;
	image2.Initialize();	//Init the image
	//int counter=0;

	light1.x=0;
	light1.y=2.7;
	light1.z=-7.75;
	//light1.y=0;
	//light1.z=0;
	light1.cl = vec3(0.3,0.3,0.3);
	light1.ca = vec3(0.6,0.6,0.6);


	oldMinT = 0;

         for(int y=0;y<image.Height();++y)
         {
                yGL = y-(windowSize/2);
                for(int x=0;x<image.Width();++x)
                {
                        origin = vec3(0,0,0);
                        minT = 0;
                        doingShadow=false;
                        shadow = false;
                        shadowCounter=0;
                        objectHit=false;
                        color = vec3(0,0,0);
                        c = vec3(0,0,0);
                        xGL = x-(windowSize/2);

                        vec3 viewRay(xGL,yGL,z);
                        normalViewRay = normalize(viewRay);

                        float t = rayIntersect2(normalViewRay,false);

                        oldMinT = t;
                        doingShadow=true;
                        origin = vec3(light1.x,light1.y,light1.z);
//                        shadowRay = vec3(light1.x,light1.y,light1.z) - (normalViewRay*t);
//                        shadowRay = -shadowRay;
//                        if(rayIntersect3(shadowRay,true)==-1){
//                                color.x=0.15;color.y=0.15;color.z=0.15;
//                        }

                        image2.SetPixel(x,y,color);
                }
         }
	image2.SaveToFile("scene2.png");

	//Scene 3
	//Image buffer stuff
	ImageBuffer image3;
	image3.Initialize();	//Init the image
	//int counter=0;

	light3.x=0;
	light3.y=2.7;
	light3.z=-7.75;
	//light3.y=0;
	//light3.z=0;
	light3.cl = vec3(0.3,0.3,0.3);
	light3.ca = vec3(0.6,0.6,0.6);


	oldMinT = 0;

	for(int y=0;y<image.Height();++y)
	{
		yGL = y-(windowSize/2);
		for(int x=0;x<image.Width();++x)
		{
                        //origin = vec3(-0.05,-0.05,-0.05);
                        origin = vec3(0,0,0);
			minT = 0;
			doingShadow=false;
			shadow = false;
			shadowCounter=0;
			objectHit=false;
			color = vec3(0,0,0);
			c = vec3(0,0,0);
			xGL = x-(windowSize/2);

			vec3 viewRay(xGL,yGL,z);
			normalViewRay = normalize(viewRay);

			float t = rayIntersect3(normalViewRay,false);

			oldMinT = t;
			doingShadow=true;
			origin = vec3(light3.x,light3.y,light3.z);
//                        shadowRay = vec3(light1.x,light1.y,light1.z) - (normalViewRay*t);
//                        shadowRay = -shadowRay;
//                        if(rayIntersect3(shadowRay,true)==-1){
//                                color.x=0.15;color.y=0.15;color.z=0.15;
//                        }

			image3.SetPixel(x,y,color);
		}
	}
	image3.SaveToFile("scene3.png");



	//********************************************
	//************Assignment 4********************
	//********************************************


	//convert x and y to somthing pointing at a pixel once you have that ray go through each geometry and intersect
	//Find z based on fov

	// run an event-triggered main loop
	// while (!glfwWindowShouldClose(window))
	// {
	// 	image.Render();	//Render the image
	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();
	// }

	//save the image to a file

	// clean up allocated resources before exit
	image.Destroy();  //Destroy the image
	DestroyGeometry(&geometry);
	DestroyShaders(&shader);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}

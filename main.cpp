#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>

#include "imageloader.h"
#include "planet.h"

using namespace std;

int kay = 0;

static int k = 0; //The frequency of subdivision
static int nv = 12; //The number of vertices
static int nf = 20; //The number of faces
static int ne = 30; //The number of edges
static const float pi = 6.283185307179586232/2.0;

//GLuint sphereDL;
//GLuint _textureId; //The id of the texture
Vec3 center(0.0,0.0,0.0); //The center of the sphere
Vec3 axis(0.0, 1.0, 0.0); //axis
Vec3 longZero(0.0, 0.0, 1.0); //longitude zero
static const float radius = 0.8; 
static float maxEdgeLength = 1.0;
float spinAngle = 0.0;
static bool overlayWireframe = false;

float rollUp = 0.0;
float rollRight = 0.0;

void initGL();
void initScene();
void drawSceneGraphics();
void glutDisplay();
void glutIdle();
void glutReshape(int width, int height);
void exitHandler();
void createPlanet();

Planet myPlanet(center, axis, longZero, radius, k);

void drawAxis()
{
	//Vec3f c=(bbox.min+bbox.max)*0.5f;
	//float L=(bbox.max-bbox.min).max() * 0.2;
	
	//Vec3f X={L,0,0}, Y={0,L,0}, Z={0,0,L};
	
	//glLineWidth(2.0);
	
	glBegin(GL_LINES);
	
	glColor3f(1.0,0.0,0.0);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	
	glColor3f(0.0,1.0,0.0);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	
	glColor3f(0.0,0.0,1.0);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	
	glEnd();
	
}

void createPlanet(){
    
    myPlanet.init();
    myPlanet.mapFaces();
    myPlanet.render();
}

//Initializes the scene.  Handles initializing OpenGL stuff. 
void initScene() {
    
	initGL();
	createPlanet();
	
}

//Sets up general OpenGL rendering properties: lights, depth buffering, etc.
void initGL() {
	static const GLfloat light_model_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
	static const GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.9f, 0.9f};   
	static const GLfloat light0_direction[] = {1.0f, 1.0f, 1.4f, 0.0f};    

	// Enable depth buffering for hidden surface removal.
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	// Cull back faces.
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// Setup other misc features.
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_FLAT);

	// Setup lighting model.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);    
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
	glEnable(GL_LIGHT0);

}

/*
The main routine for displaying the scene.  Gets the latest snapshot of state
from the haptic thread and uses it to display a 3D cursor.
*/
void drawSceneGraphics() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           

	// Spin the sphere
	spinAngle += 0.6;

	const float sqrt5 = sqrt (5.0f);
	const float phi = (1.0f + sqrt5) * 0.5f;
	const float cRadius = sqrt (10.0f + (2.0f * sqrt5)) / (4.0f * phi);
	double a = (radius / cRadius) * 0.5;
	double b = (radius / cRadius) / (2.0f * phi);
	
	// Draw the geodesic sphere
	glPushMatrix();
	
	//glRotatef(30.0, 0, 0, 1); //Makes the planet stand straight
	glRotatef(180.0, 0, 0, 1);
	//glRotatef(180, 0, 1, 0);
	glRotatef(-45, 1, 0, 0);
	glRotatef(0.0,0,1,0);
	glRotatef(rollRight, 0, 1, 0);
	//glRotatef(spinAngle, b, a, 0.0); //Make sure that the wireframe spins at the same rate
	//glRotatef(spinAngle, 0.0, 1.0, 0.0);
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
	//createGeodesicSphere ();
	//glCallList(sphereDL);
	drawAxis();
	myPlanet.render();
	/*
	Planet myPlanet(Vec3(0,0,0), Vec3(0,1,0), 1.0, 3);
	cout << "init..." << endl;
	myPlanet.init();
	cout << "refine..." << endl;
	myPlanet.refine();
	cout << "done." << endl;
	myPlanet.render();
	*/
	
	glPopMatrix();
	// Draw the overlay wireframe
	if (overlayWireframe) {
		glPushMatrix();
		//glRotatef(30.0, 0, 0, 1);
		glRotatef(180.0, 0, 0, 1);
		//glRotatef(180, 0, 1, 0);
		glRotatef(-45, 1, 0, 0);
		glRotatef(0.0,0,1,0);
		glRotatef(rollRight, 0, 1, 0);
		//glRotatef(spinAngle, b, a, 0.0);
		//glRotatef(spinAngle, 0.0, 1.0, 0.0);
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1.0,0.0,0.0);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		myPlanet.render();
		glPopAttrib();
		glPopMatrix();
	}
}

//GLUT callback for redrawing the view.
void glutDisplay() {
    
	drawSceneGraphics();
	glutSwapBuffers();
}

//GLUT callback for idle state.  Use this as an opportunity to request a redraw.
void glutIdle() {
	glutPostRedisplay();
}

//GLUT callback for reshaping the window.
void glutReshape(int width, int height) {
	static const double kPI = 3.1415926535897932384626433832795;
	static const double kFovY = 40;

	double nearDist, farDist, aspect;

	glViewport(0, 0, width, height);

	// Compute the viewing parameters based on a fixed fov and viewing
	// a canonical box centered at the origin.

	nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
	farDist = nearDist + 2.0;
	aspect = (double) width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(kFovY, aspect, nearDist, farDist);

	// Place the camera down the Z axis looking at the origin.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, nearDist + 1.0,
		0, 0, 0,
		0, 1, 0);
}

/*
This handler is called when the application is exiting.
Deallocates any state and cleans up.
*/
void exitHandler() {
	//glDeleteLists(sphereDL, 1);
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	    case '[': 
	        if(kay>0) 
	            kay--; 
	            myPlanet.k = kay; 
	            myPlanet.init(); 
	            myPlanet.mapFaces(); 
	            cout << "k = " << kay << endl; 
	            break;
	    case ']': 
	        if(kay<5) 
	            kay++; 
	        myPlanet.k = kay; 
	        myPlanet.init();
	        myPlanet.mapFaces();
	        cout << "k = " << kay << endl; 
	        break;
	    case 'w':
	        rollUp += 5.0;
	        if(rollUp >= 360.0) rollUp -= 360.0;
	        glutPostRedisplay();
	        break;
	    case 's':
	        rollUp -= 5.0;
	        if(rollUp <= -360.0) rollUp += 360.0;
	        glutPostRedisplay();
	        break;
	    case 'a'://GLUT_KEY_LEFT:
	        rollRight += 5.0;
	        if(rollRight >= 360.0) rollRight -= 360.0;
            glutPostRedisplay();
	        break;
	    case 'd'://GLUT_KEY_RIGHT:
	        rollRight -= 5.0;
	        if(rollRight <= -360.0) rollRight += 360.0;
	        glutPostRedisplay();
	        break;
		case 27: //Escape key
			exit(0);
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	//glShadeModel(GL_SMOOTH);
	
	//Image* image = loadBMP("generic_terrain.bmp");
	/*
	Image* image = loadBMP("earth.bmp");
	_textureId = loadTexture(image);
	delete image;
	*/
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}

int main(int argc, char** argv) {
	Vec3 vectorA(1.0, 0.0, 0.0);
	Vec3 vectorB(-1.0, -1.0, 0.0); 
	printf("Angle1 = %f\n", vectorA.angle(vectorB)/pi);

	vectorA.setValue(1.0, 0.0, 0.0);
	vectorB.setValue(0.0, 1.0, 0.0);
	printf("Angle2 = %f\n", vectorA.angle(vectorB)/pi);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	
	glutCreateWindow("Planet");
	
	initRendering();
	
	maxEdgeLength = 0.5; //Determines the maximum edge length
	
	// Set glut callback functions.
	glutDisplayFunc(glutDisplay);
	glutReshapeFunc(glutReshape);
	glutIdleFunc(glutIdle);
	
	glutKeyboardFunc(handleKeypress);
	
	/*
	V = 10k^2 + 2
	F = 20k^2
	E = 30k^2
	where:
	V = the number of vertices
	F = the number of faces
	E = the number of edges
	k = frequency of subdivision
	*/

	nv = nf/2+2;
	ne = nf*3/2;

	printf("k = %d\n", k);
	printf("nv = %d\n", nv);
	printf("nf = %d\n", nf);
	printf("ne = %d\n", ne);

	// Provide a cleanup routine for handling application exit.
	atexit(exitHandler);
	
	initScene();
	
	glutMainLoop();
	return 0;
}

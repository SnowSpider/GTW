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

#include "btVector3.h"
#include "imageloader.h"
#include "planet.h"

using namespace std;

// Note: The planet spins around the Y axis.
// The equator lies on the xz plane.


//2PI = 6.283185307179586232
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

int kay = 3;
Planet myPlanet(Vec3(0,0,0), Vec3(0,1,0), 1.0, kay);

static int k = 0; //The frequency of subdivision
static int nv = 12; //The number of vertices
static int nf = 20; //The number of faces
static int ne = 30; //The number of edges
static const float pi = 6.283185307179586232/2.0;

GLuint sphereDL;
GLuint _textureId; //The id of the texture
btVector3 center(0.0,0.0,0.0); //The center of the sphere
btVector3 axis(0.0, 1.0, 0.0); //axis
btVector3 longZero(0.0, 0.0, 1.0); //longitude zero
static const float radius = 1.0; 
static float maxEdgeLength = 1.0;
float spinAngle = 0.0;
static bool overlayWireframe = false;

void getNeighbors (const btVector3& v);

void drawTriangle (const btVector3& a,
				   const btVector3& b,
				   const btVector3& c);
btVector3 midpointOnSphere (const btVector3& a,
							const btVector3& b);
void drawIcosahedron (const btVector3& a,
					  const btVector3& b,
					  const btVector3& c);
void createGeodesicSphere();
void initGL();
void initScene();
void drawSceneGraphics();
void glutDisplay();
void glutIdle();
void glutReshape(int width, int height);
void exitHandler();

//Draw a single triangle
void drawTriangle (const btVector3& a,
				   const btVector3& b,
				   const btVector3& c) {
	btVector3 triCenter = (a + b + c)/ 3.0f; //The center of the triangle

	btVector3 triNormal = triCenter - center; //The normal vector of the triangle

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glBegin(GL_TRIANGLES);
		glNormal3d(triNormal[0], triNormal[1], triNormal[2]); //Normal for lighting

		/*
		unsigned char color = (unsigned char)terrain->pixels[3 * (latitude * terrain->width + longitude)];
			float h = height * ((color / 255.0f) - 0.5f);
		*/

		btVector3 cw(c[0], 0.0, c[2]);
		btScalar cLatitude;
		cLatitude = c.angle(axis)/pi; //north-south
		btScalar cLongitude;
		if(c[0]>0) cLongitude = (2.0*pi - cw.angle(longZero))/pi/2.0; //east-west
		else cLongitude = cw.angle(longZero)/pi/2.0;
		if(!(cLongitude>=0&&cLongitude<=1)) cLongitude = 0.0; //longitude is -1.#IND00; yes, I know my solution is ugly.

		btVector3 bw(b[0], 0.0, b[2]);
		btScalar bLatitude;
		bLatitude = b.angle(axis)/pi; 
		btScalar bLongitude;
		if(b[0]>0) bLongitude = (2.0*pi - bw.angle(longZero))/pi/2.0; 
		else bLongitude = bw.angle(longZero)/pi/2.0; 
		if(!(bLongitude>=0&&bLongitude<=1)) bLongitude = 0.0;
		
		btVector3 aw(a[0], 0.0, a[2]);
		btScalar aLatitude;
		aLatitude = a.angle(axis)/pi; 
		btScalar aLongitude;
		if(a[0]>0) aLongitude = (2.0*pi - aw.angle(longZero))/pi/2.0; 
		else aLongitude = aw.angle(longZero)/pi/2.0;
		if(!(aLongitude>=0&&aLongitude<=1)) aLongitude = 0.0;

		/*
		signed area = sum of 2-d cross product
		U x V = Ux*Vy-Uy*Vx
		http://howevangotburned.wordpress.com/2011/02/28/the-oddyssey-of-texturing-a-geodesic-dome/
		*/
		if((cLongitude*bLatitude-cLatitude*bLongitude)+
			(bLongitude*aLatitude-bLatitude*aLongitude)+
			(aLongitude*cLatitude-aLatitude*cLongitude)<0){
			//glColor3f(1.0,0.0,0.0); //identify the reversed triangles
			if(c[0]<=0) cLongitude++;
			if(b[0]<=0) bLongitude++;
			if(a[0]<=0) aLongitude++;
		} 

		// terrain mapping ()

		/*
		glTexCoord2f(cLongitude, cLatitude);
		glVertex3d(c[0], c[1], c[2]); //Vertex c
		glTexCoord2f(bLongitude, bLatitude);
		glVertex3d(b[0], b[1], b[2]); //Vertex b
		glTexCoord2f(aLongitude, aLatitude);
		glVertex3d(a[0], a[1], a[2]); //Vertex a
		*/
		
		glTexCoord2f(cLongitude, cLatitude);
		glVertex3d(c[0], c[1], c[2]); //Vertex c
		glTexCoord2f(bLongitude, bLatitude);
		glVertex3d(b[0], b[1], b[2]); //Vertex b
		glTexCoord2f(aLongitude, aLatitude);
		glVertex3d(a[0], a[1], a[2]); //Vertex a
		
	glEnd();
}

//Project the mid-point of a triangle edge on the sphere
btVector3 midpointOnSphere (const btVector3& a,const  btVector3& b) {
	btVector3 midpoint = (a + b) * 0.5;
	btVector3 unitRadial = midpoint - center;
	unitRadial.normalize();
	btVector3 midPointOnSphere = center + (unitRadial * radius);

	return midPointOnSphere;
}

/*
Draw each face of the Icosahedron
If the length of the traingle edge is smaller than the max length specified
	Draw the face as it is
	OR
	Subdivide each triangle in to four more and Recurse
*/
void drawIcosahedron (const btVector3& a,
					  const btVector3& b,
					  const btVector3& c,
					  const int _k) {
	btVector3 edge1 = a - b;
	btVector3 edge2 = b - c;
	btVector3 edge3 = c - a;

	float mag1 = edge1.length() ;
	float mag2 = edge2.length();
	float mag3 = edge3.length();
	
	// if all edges are short enough
	/*
	if ((mag1 < maxEdgeLength) && (mag2 < maxEdgeLength) &&	(mag3 < maxEdgeLength))	{
		// draw triangle
		drawTriangle (a, b, c);
		nf++;
	}
	*/
	if (_k==0)	{
		// draw triangle
		drawTriangle (a, b, c);
		nf++;
	}
	
	else { // otherwise subdivide and recurse
		// find edge midpoints
		const btVector3 ab = midpointOnSphere (a, b);
		const btVector3 bc = midpointOnSphere (b, c);
		const btVector3 ca = midpointOnSphere (c, a);

		// Create 4 sub-divided triangles an recurse
		drawIcosahedron ( a, ab, ca, _k-1);
		drawIcosahedron (ab,  b, bc, _k-1);
		drawIcosahedron (ca, bc,  c, _k-1);
		drawIcosahedron (ab, bc, ca, _k-1);
	}
}

//Draw the Geodesic Sphere
void createGeodesicSphere () {
	//Initialize a platonic solid
	//Here we go with an Icosahedron
	//Icosahedron stats:
	//Vertices: 12
	//Edges: 30
	//Faces: 20
	//Edges per face: 3
	//Edges shared per vertex: 5

	//Edge length = golden ratio
	const float sqrt5 = sqrt (5.0f);
	const float phi = (1.0f + sqrt5) * 0.5f;
	// Circumscribed radius
	const float cRadius = sqrt (10.0f + (2.0f * sqrt5)) / (4.0f * phi);
	//Now we define constants which will define our Icosahedron's vertices
	double a = (radius / cRadius) * 0.5;
	double b = (radius / cRadius) / (2.0f * phi);

	
	// Vertices of the Icosahedron: 
	
	/*
	btVector3 v1 =  center +	btVector3( 0,  b, -a); 
	btVector3 v2 =  center +	btVector3( b,  a,  0); //North pole
	btVector3 v3 =  center +	btVector3(-b,  a,  0);
	btVector3 v4 =  center +	btVector3( 0,  b,  a);
	btVector3 v5 =  center +	btVector3( 0, -b,  a);
	btVector3 v6 =  center +	btVector3(-a,  0,  b);
	btVector3 v7 =  center +	btVector3( 0, -b, -a);
	btVector3 v8 =  center +	btVector3( a,  0, -b);
	btVector3 v9 =  center +	btVector3( a,  0,  b);
	btVector3 v10=  center +	btVector3(-a,  0, -b);
	btVector3 v11=  center +	btVector3( b, -a,  0);
	btVector3 v12=  center +	btVector3(-b, -a,  0); //South pole
	*/

	//I need the 2 poles to be at the point where 5 triangles meet instead of 6.

	btVector3 v1 =  center +	btVector3( 0,  b, -a); 
	btVector3 v2 =  center +	btVector3( b,  a,  0); //North pole ( 0,  1,  0)
	btVector3 v3 =  center +	btVector3(-b,  a,  0);
	btVector3 v4 =  center +	btVector3( 0,  b,  a);
	btVector3 v5 =  center +	btVector3( 0, -b,  a);
	btVector3 v6 =  center +	btVector3(-a,  0,  b);
	btVector3 v7 =  center +	btVector3( 0, -b, -a);
	btVector3 v8 =  center +	btVector3( a,  0, -b);
	btVector3 v9 =  center +	btVector3( a,  0,  b);
	btVector3 v10=  center +	btVector3(-a,  0, -b);
	btVector3 v11=  center +	btVector3( b, -a,  0);
	btVector3 v12=  center +	btVector3(-b, -a,  0); //South pole ( 0, -1,  0)

	sphereDL = glGenLists(1);

	glNewList(sphereDL,GL_COMPILE);

	// draw the icosahedron
	drawIcosahedron (v1, v2, v3		, kay);
	drawIcosahedron (v4, v3, v2		, kay);
	drawIcosahedron (v4, v5, v6		, kay);
	drawIcosahedron (v4, v9, v5		, kay);
	drawIcosahedron (v1, v7, v8		, kay);
	drawIcosahedron (v1, v10, v7	, kay);
	drawIcosahedron (v5, v11, v12	, kay);
	drawIcosahedron (v7, v12, v11	, kay); 
	drawIcosahedron (v3, v6, v10	, kay);
	drawIcosahedron (v12, v10, v6	, kay);
	drawIcosahedron (v2, v8, v9		, kay);
	drawIcosahedron (v11, v9, v8	, kay);
	drawIcosahedron (v4, v6, v3		, kay);
	drawIcosahedron (v4, v2, v9		, kay);
	drawIcosahedron (v1, v3, v10	, kay);
	drawIcosahedron (v1, v8, v2		, kay);
	drawIcosahedron (v7, v10, v12	, kay);
	drawIcosahedron (v7, v11, v8	, kay);
	drawIcosahedron (v5, v12, v6	, kay);
	drawIcosahedron (v5, v9, v11	, kay);

	glEndList();
}

//Initializes the scene.  Handles initializing OpenGL stuff. 
void initScene() {
	initGL();
	
	//createGeodesicSphere ();
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
	//glRotatef(spinAngle, b, a, 0.0); //Make sure that the wireframe spins at the same rate
	//glRotatef(spinAngle, 0.0, 1.0, 0.0);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
	//createGeodesicSphere ();
	//glCallList(sphereDL);
	
	
    myPlanet.k = kay;
    myPlanet.init();
    myPlanet.refine();
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
		//glRotatef(spinAngle, b, a, 0.0);
		//glRotatef(spinAngle, 0.0, 1.0, 0.0);
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1.0,0.0,0.0);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glCallList(sphereDL);
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
	glDeleteLists(sphereDL, 1);
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	    case '[': if(kay>0) kay--; cout << "k = " << kay << endl; 
	    cout << "faces.size() = " << myPlanet.faces.size() << endl;
	    cout << "vertices.size() = " << myPlanet.vertices.size() << endl;
	    for(int i=0;i<myPlanet.faces.size();i++){
	        cout << "face # " << i << ": " << endl;
	        cout << "   a = (" << (*(myPlanet.faces[i].mya))[0] << ", " 
	                           << (*(myPlanet.faces[i].mya))[1] << ", "
	                           << (*(myPlanet.faces[i].mya))[2] << ")" << endl;
	        cout << "   b = (" << (*(myPlanet.faces[i].myb))[0] << ", " 
	                           << (*(myPlanet.faces[i].myb))[1] << ", "
	                           << (*(myPlanet.faces[i].myb))[2] << ")" << endl;
	        cout << "   c = (" << (*(myPlanet.faces[i].myc))[0] << ", " 
	                           << (*(myPlanet.faces[i].myc))[1] << ", "
	                           << (*(myPlanet.faces[i].myc))[2] << ")" << endl;
	    }
	    break;
	    case ']': if(kay<5) kay++; cout << "k = " << kay << endl; break;
		case 27: //Escape key
			exit(0);
	}
}

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	//glShadeModel(GL_SMOOTH);
	
	//Image* image = loadBMP("generic_terrain.bmp");
	
	Image* image = loadBMP("earth.bmp");
	_textureId = loadTexture(image);
	delete image;
	
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}

int main(int argc, char** argv) {
    
	btVector3 vectorA(1.0, 0.0, 0.0);
	btVector3 vectorB(-1.0, -1.0, 0.0); 
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

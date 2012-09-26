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
#include "quaternion.h"
#include "camera.h"

using namespace std;

int window_width = 800, window_height = 600;
float window_aspect = (float)window_width / (float)window_height;

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
bool drawWireframe = true;
bool drawPlanet = true;
bool drawAxis = true;

float xpos = 0;
float ypos = 0;
float zpos = 0;

float yrot = 0.0;
float xrot = 0.0;

float xrotrad, yrotrad;

float zoom;

bool leftButtonDown = false;
bool middleButtonDown = false;
bool rightButtonDown = false;

float xi = 0.0f;
float yi = 0.0f;
float theta = 0.0f;
float phi = 0.0f;

float altitudeMultiplier = 0.0;
float moveSpeed = 0.05;

int targetCell = -1;

void renderAxis();
void createPlanet();
void initScene();
void initGL();
void drawSceneGraphics();
void glutDisplay();
void glutIdle();
void camera(void);
void glutReshape(int width, int height);
void exitHandler();
void handleKeypress(unsigned char key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void initRendering();
void handleResize();

Planet myPlanet(center, axis, longZero, radius, k);
//Camera myCamera(0.5, 0.5);

void renderAxis()
{
    //Vec3f c=(bbox.min+bbox.max)*0.5f;
    //float L=(bbox.max-bbox.min).max() * 0.2;
    
    //Vec3f X={L,0,0}, Y={0,L,0}, Z={0,0,L};
    
    //glLineWidth(2.0);
    
    glDisable(GL_LIGHTING);
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
    glEnable(GL_LIGHTING);
}

void createPlanet(){
    
    myPlanet.init();    
}

//Initializes the scene.  Handles initializing OpenGL stuff. 
void initScene() {
    
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
    
    camera();
    
    //glRotatef(30.0, 0, 0, 1); //Makes the planet stand straight
//    glRotatef(180.0, 0, 0, 1);
    //glRotatef(180, 0, 1, 0);
//    glRotatef(0.0,0,1,0);
    //glRotatef(xrot, 0, 1, 0);
    
    //glRotatef(spinAngle, b, a, 0.0); //Make sure that the wireframe spins at the same rate
    //glRotatef(spinAngle, 0.0, 1.0, 0.0);
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    
    //createGeodesicSphere ();
    //glCallList(sphereDL);
    
    if(drawAxis) renderAxis();
    
    if(drawPlanet) myPlanet.render();
    /*
    Planet myPlanet(Vec3(0,0,0), Vec3(0,1,0), 1.0, 3);
    cout << "init..." << endl;
    myPlanet.init();
    cout << "refine..." << endl;
    myPlanet.refine();
    cout << "done." << endl;
    myPlanet.render();
    */
    
    if(drawWireframe){
        //myPlanet.renderCells();
        //myPlanet.renderSelectionCells();
        //myPlanet.renderWireframe();
        if(targetCell != -1) myPlanet.renderCell(myPlanet.cells[targetCell-1]);
    }
    /*
    if(drawWireframe){ // Draw the overlay wireframe
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1.0,0.0,0.0);
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        myPlanet.render();
        glPopAttrib();
    }
    */
    glPopMatrix();
    glFlush();
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

/*
void SelectObjects(GLint x, GLint y) 
{ 
    GLuint selectBuff[64];                               
    GLint hits, viewport[4];                             
      
    glSelectBuffer(64, selectBuff);                      
    glGetIntegerv(GL_VIEWPORT, viewport);                
    glMatrixMode(GL_PROJECTION);                         
    glPushMatrix();                                      
    glRenderMode(GL_SELECT);                             
    glLoadIdentity();                                    
    gluPickMatrix(x, viewport[3]-y, 2, 2, viewport);     
    gluPerspective(45.0f,ratio,0.1f,100.0f);             
    glMatrixMode(GL_MODELVIEW);                          
    glLoadIdentity();                                    
    DrawGLScene();                                       
    hits = glRenderMode(GL_RENDER);                      
    if(hits>0) ProcessSelect(selectBuff);                
    glMatrixMode(GL_PROJECTION);                         
    glPopMatrix();                                       
    glMatrixMode(GL_MODELVIEW);                          
}
*/
void SelectObjects(GLint x, GLint y) 
{ 
    GLuint selectBuff[64];                               
    GLint hits, viewport[4];                             
      
    glSelectBuffer(64, selectBuff);                      
    glGetIntegerv(GL_VIEWPORT, viewport);                
    glMatrixMode(GL_PROJECTION);                         
    glPushMatrix();                                      
    glRenderMode(GL_SELECT);                             
    glLoadIdentity();                                    
    gluPickMatrix(x, viewport[3]-y, 2, 2, viewport);
    float aspect = double(window_width)/double(window_height); 
    gluPerspective(45.0f,aspect,0.1f,100.0f);             
    glMatrixMode(GL_MODELVIEW);                          
    glLoadIdentity();                                    
    drawSceneGraphics();                                       
    hits = glRenderMode(GL_RENDER);                      
    //if(hits>0) ProcessSelect(selectBuff);                
    glMatrixMode(GL_PROJECTION);                         
    glPopMatrix();                                       
    glMatrixMode(GL_MODELVIEW);                          
}

void camera(){
    /*
    float aspect = double(window_width)/double(window_height); 
    float near_height = 1.0;
    float zNear = 0.1;
    float zFar = 2.0;
    float near_distance = 0.1;
    int BUF_SIZE = 1000;
    int mouse_y = yi;
    int mouse_x = xi;
    glMatrixMode( GL_PROJECTION ); 
    glLoadIdentity(); 
    glFrustum(-near_height * aspect, near_height * aspect, -near_height, near_height, zNear, zFar);
    
    int window_y = (window_height - mouse_y) - window_height/2; 
    double norm_y = double(window_y)/double(window_height/2); 
    int window_x = mouse_x - window_width/2; 
    double norm_x = double(window_x)/double(window_width/2);
    
    float y = near_height * norm_y; 
    float x = near_height * aspect * norm_x;
    
    float ray_pnt[4] = {0.f, 0.f, 0.f, 1.f}; 
    float ray_vec[4] = {x, y, -near_distance, 0.f};
    
    GLuint buffer[BUF_SIZE]; 
    glSelectBuffer (BUF_SIZE, buffer);
    
    GLint hits; 
    glRenderMode(GL_SELECT); 
    // ...render as usual... 
    hits = glRenderMode(GL_RENDER);
    */
    
    glTranslatef(0,0,-zoom);
    glTranslatef(-xpos, -ypos, -zpos);
    glRotatef(yrot, 0, 1, 0);
    glRotatef(xrot, cos(yrot*PI/180.0), 0, sin(yrot*PI/180.0));
    
}

//GLUT callback for reshaping the window.
void glutReshape(int width, int height) {
    static const double kPI = 3.1415926535897932384626433832795;
    static const double kFovY = 45;
    
    double nearDist, farDist, aspect;
    
    glViewport(0, 0, width, height);
    
    // Compute the viewing parameters based on a fixed fov and viewing
    // a canonical box centered at the origin.
    
    nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
    farDist = nearDist + 2.0;
    aspect = (double) width / height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(kFovY, aspect, nearDist, farDist);
    gluPerspective(kFovY, aspect, 0.01, 100);

    // Place the camera down the Z axis looking at the origin.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    gluLookAt(0, 0, nearDist + 1.0,
              0, 0, 0,
              0, 1, 0);
    
    cout << "nearDist = " << nearDist << endl;
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
            cout << "k = " << kay << endl; 
            break;
        case ']': 
            if(kay<7) 
                kay++; 
            myPlanet.k = kay; 
            myPlanet.init();
            cout << "k = " << kay << endl; 
            break;
        case 'd':
            yrot += 5.0;
            if(yrot > 360.0) yrot -= 360.0;
            break;
        case 'a':
            yrot -= 5.0;
            if(yrot < -360.0) yrot += 360.0;
            break;
        case 's'://GLUT_KEY_LEFT:
            xrot += 5.0;
            if(xrot > 360.0) xrot -= 360.0;
            break;
        case 'w'://GLUT_KEY_RIGHT:
            xrot -= 5.0;
            if(xrot < -360.0) xrot += 360.0;
            break;
        case ',':
            zoom += 0.1;
            break;
        case '.':
            zoom -= 0.1;
            break;
        case 'f':
            if(drawWireframe) drawWireframe = false;
            else drawWireframe = true;
            break;
        case 'g':
            if(drawPlanet) drawPlanet = false;
            else drawPlanet = true;
            break;
        case 'h':
            if(drawAxis) drawAxis = false;
            else drawAxis = true;
            break;
        case 27: //Escape key
            exit(0);
    }
    glutPostRedisplay();
}

Vec3 getGLPos(int x, int y){
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
 
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
 
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
 
    return Vec3(posX, posY, posZ);
}

/*
// Update the cursor pos
void PlanetGui::updateCursorPos()
{
	// Make a ray from the mouse position
	Imath::V3f rayP, rayDir;

	// unproject at 2 depths
	GLdouble x, y, z;	

	// adjust the viewport for unProject to work	
	m_viewport[1] = 0;
	m_viewport[0] = 0;

	gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.001,
				  m_modelView, m_projection, m_viewport,
				  &x, &y, &z );
	rayP = Imath::V3f( x, y, z );
	
	gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.01,
				  m_modelView, m_projection, m_viewport,
				  &x, &y, &z );
	rayDir = Imath::V3f( x, y, z );
	
	// subtract them to get the ray
	rayDir = rayDir - rayP;
	rayDir = rayDir.normalize();

	Imath::V3f p;
	if (m_planet->rayHitPlanet( rayP, rayDir, p ))
	{
		m_mouseSurfacePos = p;
		m_mouseOnSurface = true;
	}
	else
	{		
		m_mouseOnSurface = false;
	}			

	// Now, find the hex which contains the cursor
	if (m_mouseOnSurface)
	{
		m_cursorHex = m_planet->getHexIndexFromPoint( m_mouseSurfacePos );
	}
}
*/

void mouseButton(int button, int state, int x, int y){
    y = window_height - y;
    
    if(button == GLUT_LEFT_BUTTON){ // orbitCam
        if(state == GLUT_UP){
            leftButtonDown = false; 
            xi = -1;
            yi = -1;
            
            /*
            cout << "Cursor position = (" << x << ", " << y << ")" << endl;
            Vec3 rayP; // Make a ray from the mouse position
			
	        rayP = getGLPos(x, y);
	        
	        Vec3 unitRadial = rayP - myPlanet.center;
	        unitRadial.normalize();
            Vec3 radiusToRayP = center + (unitRadial * radius);
	        
	        
	        
	        Vec3 rayDir = radiusToRayP;
	
	        // subtract them to get the ray
	        rayDir = rayDir - rayP;
	        rayDir = rayDir.normalize();
	        
	        cout << "Ray origin = (" << rayP[0] << ", " << rayP[1] << ", " << rayP[2] << ")" << endl; 
	        
	        Vec3 mouseSurfacePos;
	        bool mouseOnSurface = false;
	        cout << "click - ";
	        Vec3 p;
	        if (myPlanet.rayHitPlanet( rayP, rayDir, p )){
		        mouseSurfacePos = p;
		        mouseOnSurface = true;
		        cout << "true" << endl;
	        }
	        else{		
		        mouseOnSurface = false;
		        cout << "false" << endl;
		        targetCell = -1;
	        }			

	        // Now, find the cell which contains the cursor
	        if (mouseOnSurface){
		        PlanetCell& target = myPlanet.getCellFromPoint( mouseSurfacePos );
		        targetCell = target.id;
		        cout << target.id << endl;
	        }
	        */
        }
        else{
            leftButtonDown = true; 
            xi = x;
            yi = y;
        }
    }
    if(button == GLUT_MIDDLE_BUTTON){ // panObj
        if(state == GLUT_UP){
            middleButtonDown = false;
            xi = -1;
            yi = -1;
        } 
        else{
            middleButtonDown = true;
            xi = x;
            yi = y;
        }
    }
    if(button == GLUT_RIGHT_BUTTON){ // zoom
        if(state == GLUT_UP){
            rightButtonDown = false;
            xi = -1;
            yi = -1;
        } 
        else{
            rightButtonDown = true;
            xi = x;
            yi = y;
        }
    }
    
	glutPostRedisplay(); // let glut know to redraw the screen
}

void mouseMotion(int x, int y){
    y = window_height - y;
	
	if(leftButtonDown){
	    float deltaX = x - xi;
	    float deltaY = y - yi;
	    theta += (deltaX * 0.1);
	    phi += (deltaY * 0.1);
	    xi = x;
	    yi = y;
	    yrot = theta;
	    xrot = -phi;
	}
	
	if(middleButtonDown){
	    float deltaX = x - xi;
	    float deltaY = y - yi;
	    xpos += (deltaX * 0.1);
	    ypos += (deltaY * 0.1);
	    xi = x;
	    yi = y;
	    cout << "Woolooloo" << endl;
	}
	
	if(rightButtonDown){
	    float deltaY = y - yi;
	    zoom += (deltaY * 0.1);
	    yi = y;
	}
	

	glutPostRedisplay(); // let glut know to redraw the screen
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
    /*
    vector<int> arr;
    int g = 4;
    arr.push_back(g);
    g = 5;
    cout << "arr[0] = " << arr[0] << endl;
    vector<int> brr;
    brr = arr;
    g = brr[0];
    cout << "brr[0] = " << brr[0] << endl;
    */
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
    glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
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
    //atexit(exitHandler);
    initGL();
    initScene();
    
    glutMainLoop();
    return 0;
}

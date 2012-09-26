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

#include "planet.h"
#include "quaternion.h"
//#include "camera.h"

using namespace std;

// Window size
int window_width = 800, window_height = 600;
float window_aspect = (float)window_width / (float)window_height;

// Planet parameters
Vec3 center(0.0,0.0,0.0); 
Vec3 axis(0.0, 1.0, 0.0); 
Vec3 longitude_zero(0.0, 0.0, 1.0); 
float radius = 1.0;
int k = 0;

Planet myPlanet(center, axis, longitude_zero, radius, k);

Vec3 cameraPosition;
Quaternion currentQuat(0.0f, 0.0f, 0.0f, 1.0f);
Quaternion lastQuat(0.0f, 0.0f, 0.0f, 1.0f);
const float PIOVER180 = 3.14159265/180.f;

// Debugging flags
bool drawPlanet = true;
bool drawWireframe = true;
bool drawAxis = true;

float zoom = 0.0;
float rotX = 0.0; // scroll up/down
float rotY = 0.0; // scroll left/right

void initGL();
void buildPlanet();
void drawScene();
void resize(int w, int h);
void translateX(float offset);
void translateY(float offset);
void translateZ(float offset);
void rotateX(float offset);
void rotateY(float offset);
void handleKeyPress(unsigned char key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void idle();

void initGL(){
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
}

void buildPlanet(){
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
    
    myPlanet.init();
}

void drawScene(){
    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(52.0f, window_aspect, 0.2f, 255.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -4.0f);
    */
    
    Mat4 translation(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 0);
    
    
    glTranslatef(0,0,-zoom);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //glTranslatef(0,0,-zoom);
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    //glTranslatef(0,0,-4.0f);
    glPushMatrix();
    
    if(drawAxis) myPlanet.renderAxis();
    if(drawPlanet) myPlanet.renderEarth();
    
    glPopMatrix();
    
    glFlush();
    
    glutSwapBuffers();
}

void resize(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.01, 100.0);
    
    // Place the camera down the Z axis looking at the origin.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0, 0, 2.4 + 1.0,
              0, 0, 0,
              0, 1, 0);
}

void translateX(float offset){
    
}

void translateY(float offset){
    
}

void translateZ(float offset){
    zoom += offset;
    
}

void rotateX(float offset){
    
}

void rotateY(float offset){
    
}

void rotateZ(float offset){
    
}

void euler(float pitch, float yaw, float roll){
    Quaternion qroll(sin(roll/2.0), 0, 0, cos(roll/2.0));
    Quaternion qpitch(0, sin(pitch/2.0), 0, cos(pitch/2.0));
    Quaternion qyaw(0, 0, sin(yaw/2.0), cos(yaw/2.0));
    rotation = qroll * qpitch * qyaw;
}

void handleKeyPress(unsigned char key, int x, int y){
    switch (key) {
        case '[': 
            if(k>0) 
                k--; 
            myPlanet.complexity = k; 
            myPlanet.init(); 
            cout << "k = " << k << endl; 
            break;
        case ']': 
            if(k<7) 
                k++; 
            myPlanet.complexity = k; 
            myPlanet.init();
            cout << "k = " << k << endl; 
            break;
        case 'w':
            rotX += 0.05;
            euler(rotX, rotY, 0.0);
            break;
        case 's':
            rotX -= 0.05;
            euler(rotX, rotY, 0.0);
            break;
        case 'a'://GLUT_KEY_LEFT:
            rotY += 0.05;
            euler(rotX, rotY, 0.0);
            break;
        case 'd'://GLUT_KEY_RIGHT:
            rotY -= 0.05;
            euler(rotX, rotY, 0.0);
            break;
        case ',':
            translateZ(0.1);
            break;
        case '.':
            translateZ(-0.1);
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

void idle(){
    glutPostRedisplay();
}

/*
    GLUT callback functions
    Camera
    Quaternion
    Keyboard control
    Mouse control
    Selection
    Consistent winding direction
    Buildings & units
    Command & control GUI
    In-game menu
    Wind & current map
    Precipitation map
    Vegitation map
    Temperature map
    Population density map
    Procedurally generated clouds
    Ballistics
    Collision detection
    Explosion: animated sprites & particles
    Mushroom cloud
    Day & night
    Sun & Moon
    Atmosphere
    Pathfinding (A*)
    Skybox with stars
    Flight mechanics
    Framerate counter
    Sound effects
    Music
    Cinematics
    AI
    Multiplayer game (shared machine)
    Online game
    
*/

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(window_width, window_height);
    
    glutCreateWindow("Global Thermonuclear War");
    
    initGL();
    buildPlanet();
    
    // Set glut callback functions.
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutKeyboardFunc(handleKeyPress);
    //glutMouseFunc(mouseButton);
	//glutMotionFunc(mouseMotion);
    
    glutMainLoop();
    return 0;
}

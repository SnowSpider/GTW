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
#include "quat.h"
#include <math.h>
#include "arcball.h"

using namespace std;

// Window size
int window_width = 640, window_height = 480;
float window_aspect = (float)window_width / (float)window_height;

// Planet parameters
Vec3 center(0.0,0.0,0.0); 
Vec3 axis(0.0, 1.0, 0.0); 
Vec3 longitude_zero(0.0, 0.0, 1.0); 
float radius = 1.0;
int k = 3;

Planet myPlanet(center, axis, longitude_zero, radius, k);

Vec3 cameraPosition;
Mat4 identity(1,0,0,0,
              0,1,0,0,
              0,0,1,0,
              0,0,0,1);
Mat4 rotation = identity;
Mat4 viewMatrix;
Mat4 modelMatrix;

Quat lastQuat;
Quat rotQuat;
Quat curQuat;

const float PIOVER180 = 3.14159265/180.f;

// User Defined Variables

Matrix4fT   transform   = {  1.0f,  0.0f,  0.0f,  0.0f, // NEW: Final transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT   lastRot     = {  1.0f,  0.0f,  0.0f, // NEW: Last Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

Matrix3fT   curRot     = {  1.0f,  0.0f,  0.0f, // NEW: This Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };
ArcBallT ArcBall(640.0f, 480.0f);
Point2fT    MousePt;

// Debugging flags
bool drawPlanet = true;
bool drawWireframe = true;
bool drawAxis = true;

float xi, yi;
bool leftButtonDown = false;
bool middleButtonDown = false;
bool rightButtonDown = false;
bool isDragging = false; // Dragging The Mouse?

float zoom = 0.0; // distance from the planet center
float rotX = 0.0; // angle offset, up/down
float rotY = 0.0; // angle offset, left/right
float shiftX = 0.0;
float shiftY = 0.0;

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

void motionUpdate (){ // Perform Motion Updates Here
    if (rightButtonDown){ // If Right Mouse Clicked, Reset All Rotations
        Matrix3fSetIdentity(&lastRot); // Reset Rotation
        Matrix3fSetIdentity(&curRot); // Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&transform, &curRot); // Reset Rotation
    }

    if (!isDragging){ // Not Dragging
        if (leftButtonDown){ // First Click
            isDragging = true; // Prepare For Dragging
            lastRot = curRot; // Set Last Static Rotation To Last Dynamic One
            ArcBall.click(&MousePt); // Update Start Vector And Prepare For Dragging
        }
    }
    else{
        if (leftButtonDown){ // Still Clicked, So Still Dragging
            Quat4fT     ThisQuat;
            ArcBall.drag(&MousePt, &ThisQuat); // Update End Vector And Get Rotation As Quaternion
            //cout << "ThisQuat = (" << ThisQuat.s.X << "," << ThisQuat.s.Y << "," << ThisQuat.s.Z << "," << ThisQuat.s.W << ")" << endl;
            Matrix3fSetRotationFromQuat4f(&curRot, &ThisQuat); // Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&curRot, &lastRot); // Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&transform, &curRot); // Set Our Final transform's Rotation From This One
        }
        else // No Longer Dragging
            isDragging = false;
    }
}

float z(float x, float y){
    float x2 = x*x;
    float y2 = y*y;
    float r2 = radius*radius;
    if(x2 + y2 <= r2 * 0.5){
        return sqrt( r2 - (x2 + y2) );
    }
    else{
        return r2 * 0.5 / sqrt(x2 + y2);
    }
}

Vec3 trackballProject(float x, float y){
    x = x - window_width * 0.5 - center[0];
    y = y - window_height * 0.5 - center[1];
    return Vec3(x, y, z(x,y)).normalized();
}

void trackballRotate(float x1, float y1, float x2, float y2){
    Vec3 v1 = trackballProject(x1, y1);
    Vec3 v2 = trackballProject(x2, y2);
    Vec3 normal = v1 ^ v2;
    float theta = acos(v1*v2);
    rotQuat.fromAxis(normal, theta);
 //cout << "Axis of rotation = " << normal << endl;
    curQuat = lastQuat * rotQuat;
    lastQuat = curQuat;
}

void initGL(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
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

void buildPlanet(){
    myPlanet.init();
}

void drawScene(void){
    motionUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
    glLoadIdentity(); // Reset The Current Modelview Matrix
    glTranslatef(0.0f,0.0f,-6.0f); // Move Into The Screen 6.0

    glPushMatrix(); // NEW: Prepare Dynamic transform
 //glMultMatrixf((GLfloat*)curQuat.getMatrix().mat); // NEW: Apply Dynamic transform
        glMultMatrixf(transform.M);    
        if(drawAxis) myPlanet.renderAxis();
        if(drawPlanet){
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            myPlanet.renderEarth();
        }
    glPopMatrix(); // NEW: Unapply Dynamic transform
    
    glFlush (); // Flush The GL Rendering Pipeline
}

void display(){
    drawScene();
    glutSwapBuffers();
}

void resize(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.01, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ArcBall.setBounds((GLfloat)w, (GLfloat)h);
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
            break;
        case 's':
            rotX -= 0.05;
            break;
        case 'a'://GLUT_KEY_LEFT:
            rotY += 0.05;
            break;
        case 'd'://GLUT_KEY_RIGHT:
            rotY -= 0.05;
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

void mouseButton(int button, int state, int x, int y){
    //y = window_height - y;
    if(button == GLUT_LEFT_BUTTON){ // orbitCam
        if(state == GLUT_UP){
            leftButtonDown = false; 
            xi = -1;
            yi = -1;
        }
        else{
            leftButtonDown = true; 
            xi = x;
            yi = y;
            MousePt.s.X = x;
            MousePt.s.Y = y;
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
 //y = window_height - y;
    
    if(leftButtonDown){
 //trackballRotate(xi, yi, x, y);
        float deltaX = x - xi;
        float deltaY = y - yi;
        rotY += (deltaX * 0.1);
        rotX += (deltaY * 0.1);
        xi = x;
        yi = y;
        MousePt.s.X = x;
        MousePt.s.Y = y;
        cout << "MousePT = (" << x << "," << y << ")" << endl; 
        /*
        cout << "transform = |" << transform.s.M00 << "\t" << transform.s.M10 << "\t" <<transform.s.M20 << "\t" <<transform.s.M30 <<"\t|" << endl
             << "            |" << transform.s.M01 << "\t" << transform.s.M11 << "\t" <<transform.s.M21 << "\t" <<transform.s.M31 <<"\t|" << endl
             << "            |" << transform.s.M02 << "\t" << transform.s.M12 << "\t" <<transform.s.M22 << "\t" <<transform.s.M32 <<"\t|" << endl
             << "            |" << transform.s.M03 << "\t" << transform.s.M13 << "\t" <<transform.s.M23 << "\t" <<transform.s.M33 <<"\t|" << endl;
        */
        /*
        cout << "curRot = |" << curRot.s.M00 << "\t" << curRot.s.M10 << "\t" <<curRot.s.M20 <<"\t|" << endl
             << "         |" << curRot.s.M01 << "\t" << curRot.s.M11 << "\t" <<curRot.s.M21 <<"\t|" << endl
             << "         |" << curRot.s.M02 << "\t" << curRot.s.M12 << "\t" <<curRot.s.M22 <<"\t|" << endl;
        */
    }
    
    if(middleButtonDown){
        float deltaX = x - xi;
        float deltaY = y - yi;
        shiftX += (deltaX * 0.1);
        shiftY += (deltaY * 0.1);
        xi = x;
        yi = y;
    }
    
    if(rightButtonDown){
        float deltaY = y - yi;
        zoom += (deltaY * 0.1);
        yi = y;
    }
    
    
    glutPostRedisplay(); // let glut know to redraw the screen
}

void idle(){
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(window_width, window_height);
    
    glutCreateWindow("Global Thermonuclear War");
 //scene_spin_context.setWindow(scene_window, scene_animate);
    
    initGL();
    buildPlanet();

    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeyPress);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(idle);
    
    glutMainLoop();
    return 0;
}

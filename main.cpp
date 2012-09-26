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

// User Defined Variables

Mat4 transform ( 1.0f,  0.0f,  0.0f,  0.0f, // NEW: Final transform
                 0.0f,  1.0f,  0.0f,  0.0f,
                 0.0f,  0.0f,  1.0f,  0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f );

Mat3 lastRot ( 1.0f,  0.0f,  0.0f, // NEW: Last Rotation
               0.0f,  1.0f,  0.0f,
               0.0f,  0.0f,  1.0f );

Mat3 curRot ( 1.0f,  0.0f,  0.0f, // NEW: This Rotation
              0.0f,  1.0f,  0.0f,
              0.0f,  0.0f,  1.0f );
ArcBall myArcBall(640.0f, 480.0f);
Vec2 MousePt;

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
void drawBoundingBox();
void buildPlanet();
void drawScene();
void resize(int w, int h);
void translateX(float offset);
void translateY(float offset);
void translateZ(float offset);
void rotateX(float offset);
void rotateY(float offset);
void handleKeyPress(unsigned char key, int x, int y);
Vec3 getGLPos(int x, int y);
void motionUpdate();
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void idle();

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

void drawBoundingBox(){
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 1.0f, 1.0f);

        glVertex3f(1, 1, 1); 
        glVertex3f(1, -1, 1);
        glVertex3f(1, -1, 1);
        glVertex3f(-1, -1, 1);
        glVertex3f(-1, -1, 1);
        glVertex3f(-1, 1, 1);
        glVertex3f(-1, 1, 1);
        glVertex3f(1, 1, 1);

        glVertex3f(1, 1, -1);
        glVertex3f(1, -1, -1);
        glVertex3f(1, -1, -1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, 1, -1);
        glVertex3f(-1, 1, -1);
        glVertex3f(1, 1, -1);

        glVertex3f(1, 1, 1);
        glVertex3f(1, 1, -1);
        glVertex3f(1, -1, 1);
        glVertex3f(1, -1, -1);
        glVertex3f(-1, -1, 1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, 1, 1);
        glVertex3f(-1, 1, -1);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void buildPlanet(){
    myPlanet.init();
}

void drawScene(void){
    motionUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
    glLoadIdentity(); // Reset The Current Modelview Matrix
    glTranslatef(0.0f,0.0f,-3.0f); // Move Into The Screen 3.0
    glTranslatef(0.0f, 0.0f, -zoom);
        
    glPushMatrix(); // NEW: Prepare Dynamic transform
        glMultMatrixf(transform.n);
        if(drawAxis) myPlanet.renderAxis();
        if(drawWireframe) myPlanet.renderCells();
        if(drawPlanet){
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            myPlanet.renderEarth();
        }
        drawBoundingBox();
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
    myArcBall.setBounds((GLfloat)w, (GLfloat)h);
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
    
    /*
    viewport[1] = 0;
    viewport[0] = 0;

    gluUnProject( winX, winY, 0.0,
                  modelview, projection, viewport,
                  &posX, &posY, &posZ );
    Vec3 rayP( posX, posY, posZ );
    
    gluUnProject( winX, winY, 1.0,
                  modelview, projection, viewport,
                  &posX, &posY, &posZ );
    Vec3 rayDir( posX, posY, posZ );
    
    rayDir = rayDir - rayP;
    rayDir.normalize();
    
    return rayP;
    */
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ );
    return Vec3(posX, posY, posZ);
    
}
/*
// Returns a point on the planet's surface given a ray
bool rayHitPlanet( Vec3 p, Vec3 dir, Vec3 &result ){
    float a,b,c,d;
    a = dir * dir;
    b = (2.0f*dir) * p;
    c = p*p - (kPlanetRadius*kPlanetRadius);
    d = b*b - 4.0f*a*c;
    if (d <=0 ) return false;
    result = p + ((-b - sqrt(d)) / 2.0f*a)*dir;
    return true;
}

PlanetCell& selectCell(){
    // Make a ray from the mouse position
    Vec3 rayP, rayDir;

    // unproject at 2 depths
    GLdouble x, y, z;    

    // adjust the viewport for unProject to work    
    m_viewport[1] = 0;
    m_viewport[0] = 0;

    gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.001,
                  m_modelView, m_projection, m_viewport,
                  &x, &y, &z );
    rayP = Vec3( x, y, z );
    
    gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.01,
                  m_modelView, m_projection, m_viewport,
                  &x, &y, &z );
    rayDir = Vec3( x, y, z );
    
    // subtract them to get the ray
    rayDir = rayDir - rayP;
    rayDir = rayDir.normalize();

    Vec3 p;
    if (m_planet->rayHitPlanet( rayP, rayDir, p )){
        m_mouseSurfacePos = p;
        m_mouseOnSurface = true;
    }
    else{        
        m_mouseOnSurface = false;
    }            

    // Now, find the hex which contains the cursor
    if (m_mouseOnSurface)
    {
        m_cursorHex = m_planet->getHexIndexFromPoint( m_mouseSurfacePos );
    }
}
*/
void motionUpdate(){ // Perform Motion Updates Here
    if (middleButtonDown){ // If Right Mouse Clicked, Reset All Rotations
        lastRot.setIdentity(); // Reset Rotation
        curRot.setIdentity(); // Reset Rotation
        transform.setRot(curRot); // Reset Rotation
    }

    if (!isDragging){ // Not Dragging
        if (leftButtonDown){ // First Click
            isDragging = true; // Prepare For Dragging
            lastRot = curRot; // Set Last Static Rotation To Last Dynamic One
            myArcBall.click(MousePt); // Update Start Vector And Prepare For Dragging
        }
    }
    else{
        if (leftButtonDown){ // Still Clicked, So Still Dragging
            Quat curQuat;
            myArcBall.drag(MousePt, curQuat); // Update End Vector And Get Rotation As Quaternion
            //cout << "curQuat = (" << curQuat.x << "," << curQuat.y << "," << curQuat.z << "," << curQuat.w << ")" << endl;
            curRot = curQuat.toRotMat(); // Convert Quaternion Into Matrix3fT
            curRot *= lastRot; // Accumulate Last Rotation Into This One
            transform.setRot(curRot); // Set Our Final transform's Rotation From This One
        }
        else // No Longer Dragging
            isDragging = false;
    }
}

void mouseButton(int button, int state, int x, int y){
    //y = window_height - y;
    int myY = window_height - y;
    
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
            MousePt.x = x;
            MousePt.y = y;
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
            //xi = x;
            //yi = y;
            xi = x;
            yi = myY;
            
            /*
            glPushMatrix();
            Vec3 dotCenter = getGLPos(x, myY);
            dotCenter[2] = zoom;
            glTranslatef(dotCenter[0], dotCenter[1], dotCenter[2]);
            glColor3f(1.0, 0.0, 0.0);
            glutSolidSphere(0.1, 10, 10);
            //glPointSize(3.0);
            //glEnable(GL_POINT_SMOOTH);
            //glBegin(GL_POINTS);
            //glVertex3f(newX, newY, -zoom+1.0);
            cout << "dotcenter = (" << dotCenter[0] << "," << dotCenter[1] << "," << dotCenter[2] << ")" << endl;
            glEnd();
            glPopMatrix();
            */
        }
    }
    
    glutPostRedisplay();
}

void mouseMotion(int x, int y){
    //y = window_height - y;
    int myY = window_height - y;
    
    if(leftButtonDown){
        float deltaX = x - xi;
        float deltaY = y - yi;
        rotY += (deltaX * 0.1);
        rotX += (deltaY * 0.1);
        xi = x;
        yi = y;
        MousePt.x = x;
        MousePt.y = y;
        //cout << "MousePT = (" << x << "," << y << ")" << endl; 
        /*
        cout << "transform = |" << transform.m00 << "\t" << transform.m10 << "\t" <<transform.m20 << "\t" <<transform.m30 << "\t|" << endl
             << "            |" << transform.m01 << "\t" << transform.m11 << "\t" <<transform.m21 << "\t" <<transform.m31 << "\t|" << endl
             << "            |" << transform.m02 << "\t" << transform.m12 << "\t" <<transform.m22 << "\t" <<transform.m32 << "\t|" << endl
             << "            |" << transform.m03 << "\t" << transform.m13 << "\t" <<transform.m23 << "\t" <<transform.m33 << "\t|" << endl;
        */
        /*
        cout << "curRot = |" << curRot.m00 << "\t" << curRot.m10 << "\t" <<curRot.m20 <<"\t|" << endl
             << "         |" << curRot.m01 << "\t" << curRot.m11 << "\t" <<curRot.m21 <<"\t|" << endl
             << "         |" << curRot.m02 << "\t" << curRot.m12 << "\t" <<curRot.m22 <<"\t|" << endl;
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
        /*
        float deltaY = y - yi;
        zoom += (deltaY * 0.1);
        yi = y;
        */
        float deltaY = myY - yi;
        zoom += (deltaY * 0.1);
        yi = myY;
        float newX = ((float)x * 2.0 / window_width) - 1.0;
        float newY = ((float)myY * 2.0 / window_height) - 1.0;
        cout << "newXY = (" << newX << "," << newY << ")" << endl;
        //cout << "zoom = " << zoom << endl;
    }
    
    
    glutPostRedisplay();
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

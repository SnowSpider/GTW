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

#define PIOVER180 0.01745329252
#define PICK_TOL 1.0
#define PICK_BUFFER_SIZE 1024

// Window size
int window_width = 640, window_height = 480;
float window_aspect = (float)window_width / (float)window_height;

// ArcBall variables
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

// Planet parameters
Vec3 center(0.0,0.0,0.0); 
Vec3 axis(0.0, 1.0, 0.0); 
Vec3 longitude_zero(0.0, 0.0, 1.0); 
float radius = 1.0;
int k = 4;

Planet myPlanet(center, axis, longitude_zero, radius, k);

// Debugging flags
bool drawPlanet = true;
bool drawWireframe = false;
bool drawSelectionField = false;
bool drawAxis = true;
bool drawCell = false;
bool drawTempCell = false;
bool drawBoundingBox = false;
bool drawSkybox = true;

float xi, yi;
bool leftButtonDown = false;
bool middleButtonDown = false;
bool rightButtonDown = false;
bool isDragging = false; // Dragging The Mouse?

float zoom = 0.0; // distance from the planet center

Quat lastQuat, rotQuat, curQuat;
Vec3 cursorPos(0.0f, 0.0f, 0.0f);
Vec3 cursorDir;

size_t cursorCellId = 0;

Vec3 cursorRadial_selected;
Vec3 cursorRadial;

unsigned int PickBuffer[PICK_BUFFER_SIZE]; // picking buffer

size_t selectionCounter = 0;
bool siloSelected = false;
bool targetSelected = false;

float spin = 0.2 * PIOVER180;
bool worldSpins = true;

void initGL();
void viewOrtho(int x, int y);
void ViewPerspective(void);
void renderButtons();
void renderBoundingBox();
void renderSkybox();
void buildPlanet();
float z(float x, float y);
Vec3 trackballProject(float x, float y);
void trackballRotate(float x1, float y1, float x2, float y2);
void trackballRotate_mouse(Vec3 v1, Vec3 v2);
void rotateScene_hrot(float offset);
void rotateScene_vrot(float offset);
void drawScene();
void resize(int w, int h);
void translateX(float offset);
void translateY(float offset);
void translateZ(float offset);
void rotateX(float offset);
void rotateY(float offset);
void handleKeyPress(unsigned char key, int x, int y);
Vec3 getGLPos(int x, int y);
void getGLPosDir(int x, int y, Vec3& rayPos, Vec3& rayDir);
void motionUpdate();
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void passiveMotion(int x, int y);
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
    
    // GL selection buffer.
    glSelectBuffer( PICK_BUFFER_SIZE, PickBuffer );
}

void viewOrtho(int x, int y){							// Set Up An Ortho View
	glMatrixMode(GL_PROJECTION);					// Select Projection
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, x , y , 0, -1, 1 );				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
}

void viewPerspective(void){							// Set Up A Perspective View
	glMatrixMode( GL_PROJECTION );					// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );					// Select Modelview
	glPopMatrix();							// Pop The Matrix
}

void renderButtons(){
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, window_width, window_height, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);

    glClear(GL_DEPTH_BUFFER_BIT);

    Image* image;
    image = loadBMP("data/texture/button/exit.bmp");
    GLuint buttonTexId = loadTexture(image);
    delete image;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, buttonTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBegin(GL_QUADS);
        // Red Button
        //glColor3f(1.0f, 0.0f, 0.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0, 1);
        glVertex2f(10.0, 10.0);
        glTexCoord2f(1, 1);
        glVertex2f(90.0, 10.0);
        glTexCoord2f(1, 0);
        glVertex2f(90.0, 90.0);
        glTexCoord2f(0, 0);
        glVertex2f(10.0, 90.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    // Making sure we can render 3d again
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
    
    /*
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
        // Red Button
        glColor3f(1, 0, 0);
        glVertex3f();
        glVertex3f();
        glVertex3f();
        glVertex3f();
        // Green Button
        glColor3f(0, 1, 0);
        glVertex3f();
        glVertex3f();
        glVertex3f();
        glVertex3f();
        // Blue Button
        glColor3f(0, 0, 1);
        glVertex3f();
        glVertex3f();
        glVertex3f();
        glVertex3f();
    glEnd();
    glEnable(GL_LIGHTING);
    */
}

void renderBoundingBox(){
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

void renderSkybox(){
    
}

void buildPlanet(){
    myPlanet.init();
}

float z(float x, float y){
    float x2 = x*x;
    float y2 = y*y;
    float r2 = 1.0;
    if(x2 + y2 <= r2 * 0.5){
        return sqrt( r2 - (x2 + y2) );
    }
    else{
        return r2 * 0.5 / sqrt(x2 + y2);
    }
}

Vec3 trackballProject(float x, float y){
    //x = x - window_width * 0.5 - center[0];
    //y = y - window_height * 0.5 - center[1];
    return Vec3(x, y, z(x,y)).normalize();
}

void trackballRotate(float x1, float y1, float x2, float y2){
    Vec3 v1 = trackballProject(x1, y1);
    Vec3 v2 = trackballProject(x2, y2);
    Vec3 normal = v1 ^ v2;
    float theta = acos(v1*v2);
    rotQuat.fromAxis(normal, theta);
    curQuat = lastQuat * rotQuat;
    lastQuat = curQuat;
}

void trackballRotate_mouse(Vec3 v1, Vec3 v2){
    Vec3 normal = v1 ^ v2;
    float theta = acos(v1*v2);
    rotQuat.fromAxis(normal, theta);
    curQuat = lastQuat * rotQuat;
    lastQuat = curQuat;
}

void rotateScene_hrot(float offset){
    trackballRotate(0.0, 0.0, offset, 0.0);
}

void rotateScene_vrot(float offset){
    trackballRotate(0.0, 0.0, 0.0, offset);
}

void drawScene(void){
    motionUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
    glLoadIdentity(); // Reset The Current Modelview Matrix
    
    glTranslatef(0.0f, 0.0f, -3.0f); // Move Into The Screen 3.0
    glTranslatef(0.0f, 0.0f, -zoom);
    
    if(worldSpins){
        Mat3 rotY(cos(spin), 0, sin(spin),
                  0, 1, 0, 
                  -sin(spin), 0, cos(spin));
        curRot = curRot * rotY;
        transform.setRot(curRot);
    }
    
    /*
    glDisable(GL_LIGHTING);
    glPointSize(5.0);
    glBegin(GL_POINTS);
        glColor3f(1, 0, 0);
        glVertex3f(cursorPos.x, -cursorPos.y, cursorPos.z);
    glEnd();
    glPointSize(1.0);
    glEnable(GL_LIGHTING);
    */
    
    glPushMatrix(); // Prepare Dynamic transform
        glMultMatrixf(transform.n);
        //glMultMatrixf(curQuat.getMatrix().n);
        
        if(drawAxis) myPlanet.renderAxis();
        if(drawWireframe) myPlanet.renderWireframe();
        if(drawSelectionField) myPlanet.renderCells();
        if(drawPlanet){
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            myPlanet.renderEarth();
        }
        if(drawCell){
            cursorCellId = myPlanet.getCellIdAt(cursorRadial_selected);
            if(siloSelected) glColor3f(0.0,1.0,0.0);
            else if(targetSelected) glColor3f(1.0,0.0,0.0);
            glDisable(GL_LIGHTING);
            glLineWidth( 3.0f );
            myPlanet.renderCellBoundary(myPlanet.cells[cursorCellId]);
            glLineWidth( 1.0f );
            glEnable(GL_LIGHTING);
        }
        if(drawBoundingBox) renderBoundingBox();
        if(drawSkybox) renderSkybox();
        if(drawTempCell){
            cursorCellId = myPlanet.getCellIdAt(cursorRadial);
            glColor3f(1.0,1.0,1.0);
            glDisable(GL_LIGHTING);
            glLineWidth( 3.0f );
            myPlanet.renderCellBoundary(myPlanet.cells[cursorCellId]);
            glLineWidth( 1.0f );
            glEnable(GL_LIGHTING);
        }
        renderButtons();
        
    glPopMatrix(); // Unapply Dynamic transform
    
    glFlush(); // Flush The GL Rendering Pipeline
}

void display(){
    drawScene();
    glutSwapBuffers();
}

void resize(int w, int h){
    window_width = w;
    window_height = h;
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
            if(k > 0) k--; 
            myPlanet.complexity = k; 
            myPlanet.init(); 
            break;
        case ']': 
            if(k < 7) k++; 
            myPlanet.complexity = k; 
            myPlanet.init();
            break;
        case 'w':
            rotateScene_vrot(-1.0f * PIOVER180);
            break;
        case 's':
            rotateScene_vrot(1.0f * PIOVER180);
            break;
        case 'd':
            rotateScene_hrot(-1.0f * PIOVER180);
            break;
        case 'a':
            rotateScene_hrot(1.0f * PIOVER180);
            break;
        case 'f':
            if(drawSelectionField) drawSelectionField = false;
            else drawSelectionField = true;
            break;
        case 'g':
            if(drawPlanet) drawPlanet = false;
            else drawPlanet = true;
            break;
        case 'h':
            if(drawAxis) drawAxis = false;
            else drawAxis = true;
            break;
        case ' ': //#define SPACEBAR 32
            curQuat.clear();
            lastQuat.clear();
            if(worldSpins) worldSpins = false;
            else worldSpins = true;
            break;
        case ',':
            zoom -= 0.1;
            break;
        case '.':
            zoom += 0.1;
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
    // This code is less flexible
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
    cursorDir = rayDir;
    
    return rayP;
    */
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ );
    return Vec3(posX, posY, posZ);
    
}

void getGLPosDir(int x, int y, Vec3& rayPos, Vec3& rayDir){
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
    
    gluUnProject( winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ );
    rayPos = Vec3(posX, posY, posZ);
    
    gluUnProject( winX, winY, 1.0, modelview, projection, viewport, &posX, &posY, &posZ );
    rayDir = Vec3(posX, posY, posZ);
    rayDir = rayDir - rayPos;
    rayDir.normalize();
}

void motionUpdate(){ // Perform Motion Updates Here
    if (middleButtonDown){ // If Right Mouse Clicked, Reset All Rotations
        lastRot.setIdentity(); // Reset Rotation
        curRot.setIdentity(); // Reset Rotation
        transform.setRot(curRot); // Reset Rotation
    }

    if (!isDragging){ // Not Dragging
        if (rightButtonDown){ // First Click
            isDragging = true; // Prepare For Dragging
            lastRot = curRot; // Set Last Static Rotation To Last Dynamic One
            myArcBall.click(MousePt); // Update Start Vector And Prepare For Dragging
        }
    }
    else{
        if (rightButtonDown){ // Still Clicked, So Still Dragging
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
    MousePt.x = x;
    MousePt.y = y;
    y = window_height - y;
    
    if(button == GLUT_LEFT_BUTTON){ // orbitCam
        if(state == GLUT_UP){
            //glRenderMode( GL_RENDER );
            leftButtonDown = false; 
            xi = -1;
            yi = -1;
        }
        else{
            //glRenderMode( GL_SELECT );
            leftButtonDown = true; 
            xi = x;
            yi = y;
            //cursorPos = getGLPos(x, y);
            getGLPosDir(x, y, cursorPos, cursorDir);
            //cursorDir.x = cursorDir.x;
            //cursorDir.y = cursorDir.y;
            if(myPlanet.rayHitPlanet(cursorPos, cursorDir, cursorRadial_selected)){
                cursorRadial_selected.y = -cursorRadial_selected.y;
                cursorRadial_selected = curRot*cursorRadial_selected;
                drawCell = true;
                if(selectionCounter == 0){
                    selectionCounter = 1;
                    siloSelected = true;
                    targetSelected = false;
                }
                else if(selectionCounter = 1){
                    selectionCounter = 0;
                    siloSelected = false;
                    targetSelected = true;
                }
            }
            else{
                //drawCell = false; //deselect
            }
            drawTempCell = false;
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
        drawTempCell = false;
    }
    
    glutPostRedisplay();
}

void mouseMotion(int x, int y){
    MousePt.x = x;
    MousePt.y = y;
    y = window_height - y;
    
    if(leftButtonDown){
        float deltaX = x - xi;
        float deltaY = y - yi;
        xi = x;
        yi = y;
        //cursorPos = getGLPos(x, y);
        getGLPosDir(x, y, cursorPos, cursorDir);
        //cursorDir.x = cursorDir.x;
        //cursorDir.y = cursorDir.y;
        //cout << "cursorPos = (" << cursorPos.x << "," << cursorPos.y << "," << cursorPos.z << ")" << endl;
    }
    
    if(middleButtonDown){
        float deltaX = x - xi;
        float deltaY = y - yi;
        xi = x;
        yi = y;
    }
    
    if(rightButtonDown){
        float deltaX = x - xi;
        float deltaY = y - yi;
        //zoom += (deltaY * 0.1);
        xi = x;
        yi = y;
        
        //float newX = ((float)x * 2.0 / window_width) - 1.0;
        //float newY = ((float)y * 2.0 / window_height) - 1.0;
        //cout << "newXY = (" << newX << "," << newY << ")" << endl;
        //cout << "zoom = " << zoom << endl;
    }    
    glutPostRedisplay();
}

void passiveMotion(int x, int y){
    y = window_height - y;
    getGLPosDir(x, y, cursorPos, cursorDir);
    if(myPlanet.rayHitPlanet(cursorPos, cursorDir, cursorRadial)){
        cursorRadial.y = -cursorRadial.y;
        cursorRadial = curRot*cursorRadial;
        drawTempCell = true;
    }
    else{
        drawTempCell = false;
    }
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
    glutPassiveMotionFunc(passiveMotion);
    glutIdleFunc(idle);
    
    glutMainLoop();
    return 0;
}

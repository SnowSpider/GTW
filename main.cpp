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

#include "vec.h"
#include "mat.h"
#include "quat.h"
#include "planet.h"
#include "arcball.h"

using namespace std;

// Window size
int window_width = 640, window_height = 480;
float window_aspect = (float)window_width / (float)window_height;
int currentButton = 0; // 0 for SP, 1 for MP, 2 for Options, 3 for Credits, 4 for Quit

enum {
    Game_MAIN,
    Game_CAMPGAIGN,
    Game_MULTIPLAYER,
    Game_LOADING,
    Game_PLANET,
    Game_BRIEFING,
    Game_THEATRE,
    Game_DEBRIEFING,
    Game_OPTIONS,
    Game_ACHIEVEMENTS,
    Game_CREDITS,
    Game_MOVIE
};
/*
enum
{
	MOUSE_LEFT_BUTTON = 0,
	MOUSE_MIDDLE_BUTTON = 1,
	MOUSE_RIGHT_BUTTON = 2,
	MOUSE_SCROLL_UP = 3,
	MOUSE_SCROLL_DOWN = 4
};
*/
int gameState = Game_PLANET; // 0 for main menu, 1 for game, 2 for cinematics

GLuint texId_day;
GLuint texId_night;
GLuint texId_pop;
GLuint texId_terrain;
GLuint texId_spec;

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

ArcBall myArcBall(window_width, window_height);
Vec2 MousePt;

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

size_t selectionCounter = 0;
bool siloSelected = false;
bool targetSelected = false;
bool drawTempCell = false;

// Planet parameters
Vec3 m_center(0.0,0.0,0.0); 
Vec3 m_axis(0.0, 1.0, 0.0); 
Vec3 m_longitude_zero(0.0, 0.0, 1.0); 
float m_radius = 1.0;
int m_subdLevel = 4;

//Planet myPlanet(center, axis, longitude_zero, radius, k);
Planet m_planet( m_subdLevel );

void initGL();
void viewOrtho(int x, int y);
void viewPerspective();
void renderButtons();
void drawMain();
void initPlanet(int k);
void drawPlanet();
void drawTheatre();
void drawMovie();
Vec3 getGLPos(int x, int y);
void getGLPosDir(int x, int y, Vec3& rayPos, Vec3& rayDir);
void resize(int w, int h);
void display();
void motionUpdate();
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void passiveMotion(int x, int y);
void idle();
void handleKeyPress(unsigned char key, int x, int y);
void processSpecialKeys(int key, int x, int y);


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

void viewOrtho(int x, int y){							// Set Up An Ortho View
	glMatrixMode(GL_PROJECTION);					// Select Projection
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, x , y , 0, -1, 1 );				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
}

void viewPerspective(){							// Set Up A Perspective View
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
    
}

void drawMain(){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
    glLoadIdentity(); // Reset The Current Modelview Matrix
    
    glPushMatrix(); // Prepare Dynamic transform
        
        renderButtons();
        
    glPopMatrix(); // Unapply Dynamic transform
    
    glFlush(); // Flush The GL Rendering Pipeline
}

void initPlanet( int k ){
    m_planet = Planet( k );
    
    // Load texture images
    Image* image;
    
    image = loadBMP("data/texture/earth/earth_day.bmp");
    texId_day = loadTexture(image);
    
    image = loadBMP("data/texture/earth/earth_night.bmp");
    texId_night = loadTexture(image);
    
    image = loadBMP("data/texture/earth/earth_pop.bmp");
    texId_pop = loadTexture(image);
    
    image = loadBMP("data/texture/earth/earth_terrain.bmp");
    texId_terrain = loadTexture(image);
    
    image = loadBMP("data/texture/earth/earth_spec.bmp");
    texId_spec = loadTexture(image);
    
    delete image;
}

void drawPlanet(){
    motionUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
    glLoadIdentity(); // Reset The Current Modelview Matrix
    glTranslatef(0.0f, 0.0f, -3.0f); // Move Into The Screen 3.0
    glTranslatef(0.0f, 0.0f, -zoom); 
    
    //m_planet.draw( Planet::DrawMode_MESH );
    
    glPushMatrix(); // Prepare Dynamic transform
        glMultMatrixf(transform.n);
        //glMultMatrixf(curQuat.getMatrix().n);
        
        m_planet.draw( Planet::DrawMode_MESH );
        
    glPopMatrix(); // Unapply Dynamic transform
    
    glFlush(); // Flush The GL Rendering Pipeline
}

void drawTheatre(){
    
}

void drawMovie(){
    
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

void display(){
    if (gameState == Game_MAIN){
        drawMain();
    }
    else if (gameState == Game_PLANET){
        drawPlanet();
    }
    else if (gameState == Game_LOADING){
        drawMovie();
    }
    glutSwapBuffers();
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
    
    if(button == GLUT_LEFT_BUTTON){
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
            if(m_planet.rayHitPlanet(cursorPos, cursorDir, cursorRadial_selected)){
                cursorRadial_selected.y = -cursorRadial_selected.y;
                cursorRadial_selected = curRot*cursorRadial_selected;
                //drawCell = true;
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
    if(button == GLUT_MIDDLE_BUTTON){
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
    if(button == GLUT_RIGHT_BUTTON){
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
    if(button == 3){ // MOUSE_SCROLL_UP
        zoom -= 0.01;
    }
    if(button == 4){ // MOUSE_SCROLL_DOWN
        zoom += 0.01;
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
    if(m_planet.rayHitPlanet(cursorPos, cursorDir, cursorRadial)){
        cursorRadial.y = -cursorRadial.y;
        cursorRadial = curRot*cursorRadial;
        drawTempCell = true;
        cout << "";
    }
    else{
        drawTempCell = false;
        cout << "";
    }
}

void idle(){
    glutPostRedisplay();
}

void handleKeyPress(unsigned char key, int x, int y){
    switch (key) {
        case '[': 
            if(m_subdLevel > 0) m_subdLevel--;  
            initPlanet(m_subdLevel); 
            break;
        case ']': 
            if(m_subdLevel < 6) m_subdLevel++; 
            initPlanet(m_subdLevel);
            break;
        /*
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
        */
        case 27: //Escape key
            exit(0);
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y){
    switch (key) {
        case GLUT_KEY_UP : 
            //if(gameState == 0)
            cout << "Keyboard input: UP" << endl;
            break;
        case GLUT_KEY_DOWN : 
            cout << "Keyboard input: DOWN" << endl;
            break;
        case GLUT_KEY_LEFT : 
            cout << "Keyboard input: LEFT" << endl;
            break;
        case GLUT_KEY_RIGHT : 
            cout << "Keyboard input: RIGHT" << endl;
            break;
        case GLUT_KEY_F1 : 
            cout << "Keyboard input: F1" << endl; //Open in-game menu
            break;
        case GLUT_KEY_F2 : 
            cout << "Keyboard input: F2" << endl; 
            break;
        case GLUT_KEY_F3 : 
            cout << "Keyboard input: F3" << endl; 
            break;
        case GLUT_KEY_F4 : 
            cout << "Keyboard input: F4" << endl; 
            break;
        case GLUT_KEY_F5 : 
            cout << "Keyboard input: F5" << endl; //Quick save
            break;
        case GLUT_KEY_F6 : 
            cout << "Keyboard input: F6" << endl; 
            break;
        case GLUT_KEY_F7 : 
            cout << "Keyboard input: F7" << endl; 
            break;
        case GLUT_KEY_F8 : 
            cout << "Keyboard input: F8" << endl; //Quick load
            break;
        case GLUT_KEY_F9 : 
            cout << "Keyboard input: F9" << endl; //Adjust brightness
            break;
        case GLUT_KEY_F10 : 
            cout << "Keyboard input: F10" << endl; //Adjust contrast
            break;
        case GLUT_KEY_F11 : 
            cout << "Keyboard input: F11" << endl; //Adjust gamma
            break;
        case GLUT_KEY_F12 : 
            cout << "Keyboard input: F12" << endl; //Take screenshot
            break;
    }
}

int main(int argc, char** argv) {
    //cout << "Hello World!" << endl; 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Global Thermonuclear War");
    
    initGL();
    initPlanet( m_subdLevel );
    
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(handleKeyPress);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(passiveMotion);
    glutIdleFunc(idle);
    
    //Main Menu
    
    //Single Player
    //Multi Player
    //Options
    //Credits
    //Quit
    
    glutMainLoop();
    
    return 0;
}

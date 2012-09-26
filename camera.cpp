#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "camera.h"
#include "mat.h"

//#include <windows.h>
//#include <gl/glu.h>
//#include <gl/gl.h>

const float PIOVER180 = 3.14159265/180.f;

Camera::Camera(float rotspeed, float movespeed)
: rotation(0.0f, 0.0f, 0.0f, 1.0f), rotspeed(rotspeed), movespeed(movespeed){

}

void Camera::init(){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

void Camera::resize(int width, int height){
    if (height <= 0)
        height = 1;

    int aspectratio = width / height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(52.0f, aspectratio, 0.2f, 255.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -4.0f);
}

const Vec3& Camera::getPosition() const{
    return position;
}

const Quaternion& Camera::getRotation() const{
    return rotation;
}

Mat4 Camera::getViewMatrix() const{
    return rotation.getMatrix();
}

void Camera::movex(float xmod){
    position += rotation * Vec3(xmod, 0.0f, 0.0f);
}

void Camera::movey(float ymod){
    position.y -= ymod;
}

void Camera::movez(float zmod){
    position += rotation * Vec3(0.0f, 0.0f, -zmod);
}

void Camera::rotatex(float xmod){
    Quaternion nrot(1.0f, 0.0f, 0.0f, xmod * PIOVER180);
    rotation = rotation * nrot;
}

void Camera::rotatey(float ymod){
    Quaternion nrot(0.0f, 1.0f, 0.0f, ymod * PIOVER180);
    rotation = nrot * rotation;
}

void Camera::update(float xrot, float yrot, float xmove, float ymove, float zmove, float delta){
    if (xrot != 0.0f){
        rotatex(xrot * delta * rotspeed);
    }

    if (yrot != 0.0f) {
        rotatey(yrot * delta * rotspeed);
    }

    if (xmove != 0.0f) {
        movex(xmove * delta * movespeed);
    }

    if (ymove != 0.0f) {
        movey(ymove * delta * movespeed);
    }

    if (zmove != 0.0f) {
        movez(zmove * delta * movespeed);
    }
}


void Camera::render(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glLoadMatrixf((GLfloat*)rotation.getMatrix().mat);
    glTranslatef(0.0f, 0.0f, -4.0f);
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, -0.5f, 0.0f);    // A
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f, -0.5f, 0.0f);    // B
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 0.0f,  0.5f, 0.0f);    // C
    glEnd();
    
}

#ifndef __PROJECTOR_H__
#define __PROJECTOR_H__

#include <iostream>
#include <vector>
#include "vec.h"

#define PI 3.1415926535897932384626433832795

using namespace std;

class Projector{
    public:
    Vec3 p; // projector position
    Vec3 n; // unit plane normal
    Vec3 p0; // base of the perpendicular line from p to the plane
    float d; // distance to the image plane
    Vec3 v; // the vertex of the object being rendered
    
    // (s,t) texture coordinates for v
    // pv = p + ((d*(v-p))/((v-p)*n)); // the projection of v to the plane in world coordinates
    // convert Pv to plane coordinates in the coord. system (n × up, up) with origin at p0 to get (s,t)
    // if necessary rescale s and t to give the image the desired size on the object
    
    // default constructor
    // parametrized constructor
    // copy constructor
    // copy assignment operator
    
    Projector(){
        
    }
    /*
    Projector(){
        
    }
    */
    Projector( const Projector& other ){
        
    }
    
    Projector& operator=( const Projector& other ){
        v = other.v;
        return *this;
    }
    
    /*
    Using OpenGL, single texture coordinates
    tell OpenGL we are using texture generation in object coords for S
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR;
    setup the equation for computing the S coord
        float [] planeS = { 1.0f, 0.0f, 0.0f, 0.0f };
        glTexGenfv(GL_S, GL_OBJECT_PLANE, planeS);
    before drawing the objects make sure the texture coord generation is
    enabled and disable it after:
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_GEN_S);
        …
        glDisable(GL_TEXTURE_GEN_S);
    */
    
    /*
    The previous code results in texture coordinate s computed
    automatically as
        S = 1*x + 0*y + 0*z + 1 = x + 1 for a vertex (x,y,z) (in object coords)
    Similarly, you can enable T computed as
        T = 0*x+1*y +0*z + 1 = y + 1
    This is not very interesting – you may as well use TexCoord2(x+1,y+1)
    But using different planes for S and T allows you to change the
    projection direction; you can think about S and T as being (scaled)
    distance to the plane with equation
        x0*x + y0*y + z0*z + w0 = 0, with w0 setting the scale.
    The two planes need to be parallel to the projection direction
    */
};

#endif

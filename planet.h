#ifndef __DRAWPLANET_H__
#define __DRAWPLANET_H__

#include <vector>
#include "vec3.h"

using namespace std;

// Note: The planet spins around the Y axis.
// The equator lies on the xz plane.


//2PI = 6.283185307179586232
/*
V = 10k^2 + 2
F = 20k^2
E = 30k^2
where:
V = number of vertices
F = number of faces
E = number of edges
k = frequency of subdivision
*/

#define PI 3.1415926535897932384626433832795

class Planet{
    public:
    int k; // frequency of subdivision
    //int nv; // number of vertices
    //int nf; // number of faces
    //int ne; // number of edges
    Vec3 center;
    Vec3 axis; // axis vector
    Vec3 longZero; //longitude zero
    float radius;
    //float spinAngle;
    bool overlayWireframe;
    GLuint _textureId;
    
    Planet(Vec3 _center, Vec3 _axis, float _radius){
        center = _center;
        axis = _axis;
        radius = _radius;
        //k = _k;
        //nv = 0;
        //nf = 0;
        //ne = 0;
        //spinAngle = 0.0;
        longZero = Vec3(0, 0, 1);
        overlayWireframe = false;
        
        
    }
    
    class PlanetVertex: public Vec3{
        public:
        
        float altitude;
        float longitude;
        float latitude;
        vector<int> neighbors;
        
        PlanetVertex( const float x, const float y, const float z ){
            n[0] = x; 
            n[1] = y;
            n[2] = z;
            Vec3 w = Vec3(n[0], 0.0, n[2]);
		    if(n[0]>0) longitude = (2.0*PI - w.angle(Vec3(0, 0, 1)))/PI/2.0;
		    else longitude = w.angle(Vec3(0, 0, 1))/PI/2.0;
		    if(!(longitude>=0&&longitude<=1)) longitude = 0.0; //longitude is -1.#IND00; yes, I know my solution is ugly.
            latitude = this->angle(Vec3(0, 1, 0)) / PI;
        }
        
        PlanetVertex( const Vec3& v ){
            n[0] = v[0]; 
            n[1] = v[1]; 
            n[2] = v[2];
            
            Vec3 w = Vec3(n[0], 0.0, n[2]);
		    if(n[0]>0) longitude = (2.0*PI - w.angle(Vec3(0, 0, 1)))/PI/2.0;
		    else longitude = w.angle(Vec3(0, 0, 1))/PI/2.0;
		    if(!(longitude>=0&&longitude<=1)) longitude = 0.0; //longitude is -1.#IND00; yes, I know my solution is ugly.
            latitude = this->angle(Vec3(0, 1, 0)) / PI;
        }
        
    };
    
    vector<PlanetVertex> vertices;
    
    class PlanetFace{
        public:
        //int myVertices[3];
        Vec3 faceCenter;
        Vec3 normal;
        float angle;
        
        PlanetFace(const PlanetVertex& a, const PlanetVertex& b, const PlanetVertex& c){
            //myVertices[0] = a.id;
            //myVertices[1] = b.id;
            //myVertices[2] = c.id;
            faceCenter = (a + b + c)/3.0;
            normal = (b-a).cross(c-a);
            //normal.normalize();
            angle = faceCenter.angle(normal);
        }
    };
    
    vector< vector<PlanetFace> > faces; //faces_k
    
    void init();
    void refine();
    void subdivide(const PlanetVertex& a, const PlanetVertex& b, const PlanetVertex& c, const int& _k);
    void drawFace (const Vec3& a, const Vec3& b, const Vec3& c);
    PlanetVertex midpointOnSphere (const PlanetVertex& a, const  PlanetVertex& b);
    
    void render();
    
};

#endif

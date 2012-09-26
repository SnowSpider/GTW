#ifndef __DRAWPLANET_H__
#define __DRAWPLANET_H__

#include <vector>
#include "vec3.h"

using namespace std;

// Note: The planet spins around the Y axis.
// The equator lies on the xz plane.
// Post a question on the OpenGL Forum.

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

class PlanetVertex: public Vec3{
    public:
    unsigned int id;
    float altitude;
    float longitude;
    float latitude;
    vector<int> neighbors;
    
    PlanetVertex(){
        id = -1;
        altitude = 0;
        longitude = 0;
        latitude = 0;
    }
    
    PlanetVertex( const float x, const float y, const float z ){
        id = -1;
        n[0] = x; 
        n[1] = y;
        n[2] = z;
        altitude = 0;
        longitude = 0;
        latitude = 0;
    }
    
    PlanetVertex( const Vec3& v ){
        id = -1;
        n[0] = v[0]; 
        n[1] = v[1]; 
        n[2] = v[2];
        altitude = 0;
        longitude = 0;
        latitude = 0;
    }
        
    PlanetVertex( const PlanetVertex& v ){
        id = v.id; //critical
        n[0] = v[0]; 
        n[1] = v[1]; 
        n[2] = v[2];
    }
        
    PlanetVertex& operator=( const PlanetVertex& v ){
        id = v.id; //critical
        n[0] = v[0]; 
        n[1] = v[1]; 
        n[2] = v[2];
        altitude = v.altitude;
        longitude = v.longitude;
        latitude = v.latitude;
        return *this;
    }
    
    bool equals(const PlanetVertex& v) {
        if(n[0]==v[0] && n[1]==v[1] && n[2]==v[2]) return true;
        else return false;
    }
};

class PlanetFace{
    public:
    unsigned int id;
    int v[3];
    Vec3 center;
    Vec3 normal;
    float angle;
    
    PlanetFace(){
        v[0] = -1;
        v[1] = -1;
        v[2] = -1;
        center = Vec3(0,0,0);
        normal = Vec3(0,0,0);
        float angle = 0;
    }
    
    /*
    PlanetFace(int a, int b, int c){
        v[0] = a;
        v[1] = b;
        v[2] = c;
        center = (vertices[a] + vertices[b] + vertices[c])/3.0;
        normal = (vertices[b]-vertices[a])^(vertices[c]-vertices[a]);
        angle = center.angle(normal);
    }
    */
    
    PlanetFace(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
        v[0] = a.id;
        v[1] = b.id;
        v[2] = c.id;
        center = (a + b + c)/3.0;
        normal = (b-a)^(c-a);
        angle = center.angle(normal);
    }
        
    PlanetFace(const PlanetFace& f){
        v[0] = f.v[0];
        v[1] = f.v[1];
        v[2] = f.v[2];
        center = f.center;
        normal = f.normal;
        angle = f.angle;
    }
        
    PlanetFace& operator=(const PlanetFace& f){
        id = f.id;
        v[0] = f.v[0];
        v[1] = f.v[1];
        v[2] = f.v[2];
        center = f.center;
        normal = f.normal;
        angle = f.angle;
        return *this;
    }    
};

class PlanetCell{
    public:
    unsigned int id;
    vector<int> vertices;
    int normal;
    int owner;
    vector<int> neighbors;
    
    PlanetCell(){
        normal = -1;
        owner = -1;
    }
    
    void setOwner(int him){
        owner = him;
    }
};

class VertexList{
    public:
    vector<PlanetVertex> _vertices;
    unsigned int currentId;
    
    VertexList(){
        currentId = 0;
    }
    
    void add(PlanetVertex& v){
        v.id = currentId++;
        _vertices.push_back(v);
    }
    
    PlanetVertex& operator []( int i ){ 
	    return _vertices[i];
	}
	
	PlanetVertex operator []( int i ) const { 
	    return _vertices[i]; 
	}
	
	int size(){
	    return _vertices.size();
	}
	
	void clear(){
	    _vertices.clear();
	    currentId = 0;
	}
};

class FaceList{
    public:
    vector<PlanetFace> _faces;
    unsigned int currentId;
    
    FaceList(){
        currentId = 0;
    }
    
    void add(PlanetFace& f){
        f.id = currentId++;
        _faces.push_back(f);
    }
    
    PlanetFace& operator []( int i ){ 
	    return _faces[i];
	}
	
	PlanetFace operator []( int i ) const { 
	    return _faces[i]; 
	}
	
	int size(){
	    return _faces.size();
	}
	
	void clear(){
	    _faces.clear();
	    currentId = 0;
	}
};

class CellList{
    public:
    vector<PlanetCell> _cells;
    unsigned int currentId;
    
    CellList(){
        currentId = 0;
    }
    
    void add(PlanetCell& c){
        c.id = currentId++;
        _cells.push_back(c);
    }
    
    PlanetCell& operator []( int i ){ 
	    return _cells[i];
	}
	
	PlanetCell operator []( int i ) const { 
	    return _cells[i]; 
	}
};

class Planet{
    public:
    int k; // frequency of subdivision
    int numVert; // number of vertex
    int numFace; // number of faces
    int numCell; // number of cells

    VertexList vertices;
    FaceList faces;
    CellList cells;
    
    Vec3 center;
    Vec3 axis; // axis vector
    Vec3 longZero; //longitude zero
    float radius;
    float spinAngle;
    bool overlayWireframe;
    GLuint _textureId;
    
    Planet(Vec3 _center, Vec3 _axis, Vec3 _longZero, float _radius, int _k){
        center = _center;
        axis = _axis;
        radius = _radius;
        k = _k;
        longZero = _longZero;
        overlayWireframe = false;
    }
    
    void init();
    void subdivide (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c, int _k);
    void mapFace (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c);
    void mapFaces();
    void drawFace (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c);
    PlanetVertex midpointOnSphere (PlanetVertex& a, PlanetVertex& b);
    void render();
};

#endif

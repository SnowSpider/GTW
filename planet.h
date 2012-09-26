#ifndef __PLANET_H__
#define __PLANET_H__

#include <iostream>
#include <vector>
#include "vec.h"

#define PI 3.1415926535897932384626433832795

using namespace std;

class PlanetVertex: public Vec3{
    public:
    size_t id;
    float altitude;
    float longitude;
    float latitude;
    vector<size_t> neighbors;
    bool positive;
    
    PlanetVertex(){
        altitude = 0;
        longitude = 0;
        latitude = 0;
        positive = true;
    }
    
    PlanetVertex( const float a, const float b, const float c ){
        x = a;
        y = b;
        z = c;
        altitude = 0;
        longitude = 0;
        latitude = 0;
        positive = true;
    }
    
    PlanetVertex( const Vec3& v ){
        x = v.x; 
        y = v.y; 
        z = v.z;
        altitude = 0;
        longitude = 0;
        latitude = 0;
        positive = true;
    }
    
    PlanetVertex( const PlanetVertex& v ){
        id = v.id; //critical
        x = v.x;
        y = v.y;
        z = v.z;
        altitude = v.altitude;
        longitude = v.longitude;
        latitude = v.latitude;
        positive = v.positive;
        if(positive) neighbors = v.neighbors;
    }
    
    PlanetVertex( Vec3& v ){
        x = v.x; 
        y = v.y; 
        z = v.z;
        altitude = 0;
        longitude = 0;
        latitude = 0;
        positive = true;
    }
        
    PlanetVertex( PlanetVertex& v ){
        id = v.id; //critical
        x = v.x;
        y = v.y;
        z = v.z;
        altitude = v.altitude;
        longitude = v.longitude;
        latitude = v.latitude;
        positive = v.positive;
        if(positive) neighbors = v.neighbors;
    }
    
    PlanetVertex& operator=( const PlanetVertex& v ){
        id = v.id; //critical
        x = v.x; 
        y = v.y; 
        z = v.z;
        altitude = v.altitude;
        longitude = v.longitude;
        latitude = v.latitude;
        positive = v.positive;
        if(positive) neighbors = v.neighbors;
        return *this;
    }
    
    bool equals(const PlanetVertex& v) const {
        if(x==v.x && y==v.y && z==v.z) return true;
        else return false;
    }
    
    operator bool() const {
        return positive;
    }
    
    void marry(PlanetVertex& v){
        if(positive && v.positive){
            bool isNew = true;
            for(int i=0;i<neighbors.size();i++){
                if(neighbors[i]==v.id){
                    isNew = false;
                    break;
                }
            }
            if(isNew){
                neighbors.push_back(v.id);
                v.neighbors.push_back(id);
            }
        }
    }
    
    bool isMarriedTo(PlanetVertex& v){
        for(int i=0;i<neighbors.size();i++){
            if(neighbors[i]==v.id) return true;
        }
        return true;
    }
};

class PlanetFace{
    public:
    size_t id;
    int v[3];
    Vec3 center;
    Vec3 normal;
    float angle;
    bool positive;
    
    PlanetFace(){
        v[0] = -1;
        v[1] = -1;
        v[2] = -1;
        center = Vec3(0,0,0);
        normal = Vec3(0,0,0);
        float angle = 0;
        positive = true;
    }
    
    PlanetFace(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
        v[0] = a.id;
        v[1] = b.id;
        v[2] = c.id;
        center = (a + b + c)/3.0;
        normal = (b-a)^(c-a);
        angle = center.angle(normal);
        positive = true;
    }
        
    PlanetFace(const PlanetFace& f){
        v[0] = f.v[0];
        v[1] = f.v[1];
        v[2] = f.v[2];
        center = f.center;
        normal = f.normal;
        angle = f.angle;
        positive = f.positive;
    }
        
    PlanetFace& operator=(const PlanetFace& f){
        id = f.id;
        v[0] = f.v[0];
        v[1] = f.v[1];
        v[2] = f.v[2];
        center = f.center;
        normal = f.normal;
        angle = f.angle;
        positive = f.positive;
        return *this;
    }
    
    operator bool() const {
        return positive;
    }
};

class PlanetCell{
    public: 
    size_t id; 
    size_t centerId;
    size_t owner; //id of the player who owns the territory
    vector<size_t> neighbors; //same as the center vertex neighbors
    vector<size_t> paramVerts; //parameter vertices
    float altitude;
    float longitude;
    float latitude;
    //bool water; //???
    
    PlanetCell(){
        id = -1;
        owner = -1;
        altitude = 0;
        longitude = 0;
        latitude = 0;
    }
    
    PlanetCell(PlanetVertex& v){
        //id = v.id; //not a good idea
        centerId = v.id;
        owner = -1;
        neighbors = v.neighbors;
        altitude = v.altitude;
        longitude = v.longitude;
        latitude = v.latitude;
    }
    
    PlanetCell( const PlanetCell& c ){
        id = c.id; //critical
        centerId = c.centerId;
        owner = c.owner;
        neighbors = c.neighbors;
        paramVerts = c.paramVerts;
        altitude = c.altitude;
        longitude = c.longitude;
        latitude = c.latitude;
    }
        
    PlanetCell( PlanetCell& c ){
        id = c.id; //critical
        centerId = c.centerId;
        owner = c.owner;
        neighbors = c.neighbors;
        paramVerts = c.paramVerts;
        altitude = c.altitude;
        longitude = c.longitude;
        latitude = c.latitude;
    }
    
    PlanetCell& operator=( const PlanetCell& c ){
        id = c.id; //critical
        centerId = c.centerId;
        owner = c.owner;
        neighbors = c.neighbors;
        paramVerts = c.paramVerts;
        altitude = c.altitude;
        longitude = c.longitude;
        latitude = c.latitude;
    }
    
    void setOwner(size_t him){
        owner = him;
    }
};

class VertexList{
    public:
    vector<PlanetVertex> _vertices;
    size_t currentId;
    
    VertexList(){
        currentId = 0;
    }
    
    void add(PlanetVertex& v){
        v.id = currentId++; // DO NOT SWITCH THE ORDER OF THESE 2 LINES
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
    size_t currentId;
    
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
    size_t currentId;
    
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
	
	int size(){
	    return _cells.size();
	}
	
	void clear(){
	    _cells.clear();
	    currentId = 0;
	}
};

class Planet {
    public:
    VertexList vertices;
    FaceList faces;
    CellList cells;
    
    Vec3 center;
    Vec3 axis; 
    Vec3 longitude_zero; 
    float radius;
    int complexity;
    
    float spinAngle;
    GLuint texId_day;
    GLuint texId_night;
    GLuint texId_pop;
    GLuint texId_terrain;
    GLuint texId_spec;
    
    Planet(Vec3 c, Vec3 a, Vec3 lz, float r, int k){
        center = c;
        axis = a;
        longitude_zero = lz;
        radius = r;
        complexity = k;
    }
    
    void init();
    void subdivide (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c, int _k);
    void mapVertex(PlanetVertex& v);
    void fixSeamHelper (PlanetFace& f);
    void fixSeam();
    void mapTerrain(const char* filename, float unitHeight);
    PlanetVertex midpointOnSphere (PlanetVertex& a, PlanetVertex& b);
    PlanetVertex midpointOnSphere (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c);
    void drawFace (PlanetFace& f);
    void renderEarth();
    void renderWireframe();
    void genCells();
    void renderCellBoundary(PlanetCell& c);
    PlanetCell& getCellAt(float longitude, float latitude);
    PlanetCell& getCellAt( Vec3 surfPos );
    void renderAxis();
};

#endif

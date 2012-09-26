#ifndef __PLANET_H__
#define __PLANET_H__

#include <iostream>
#include <vector>
#include "vec.h"

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
    vector<unsigned int> neighbors;
    bool positive;
    
    PlanetVertex(){
        id = -1;
        altitude = 0;
        longitude = 0;
        latitude = 0;
        positive = true;
    }
    
    PlanetVertex( const float a, const float b, const float c ){
        id = -1;
        x = a;
        y = b;
        z = c;
        altitude = 0;
        longitude = 0;
        latitude = 0;
        positive = true;
    }
    
    PlanetVertex( const Vec3& v ){
        id = -1;
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
        id = -1;
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
    unsigned int id;
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
    unsigned int id; 
    unsigned int centerId;
    unsigned int owner; //id of the player who owns the territory
    vector<unsigned int> neighbors; //same as the center vertex neighbors
    vector<unsigned int> paramVerts; //parameter vertices
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
    
    void setOwner(unsigned int him){
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
	
	int size(){
	    return _cells.size();
	}
	
	void clear(){
	    _cells.clear();
	    currentId = 0;
	}
};

class Projectile{
    public:
    unsigned int id;
    string name;
    bool guided;
    float blastRadius;
    
    
};

class Weapon{
    public:
    unsigned int id;
    string name;
    int range;
    int reloadTime; //in miliseconds
    int damage;
    
};

class Species{
    public: 
    unsigned int id;
    bool mobile;
    
    string name;
    string designation;
    string description;
    //Weapon weapon;
    
    int weaponId;
    int shockArmor;
    int pierceArmor;
    int cutArmor;
    int thermalArmor;
    int chemicalArmor;
    int radiationArmor;
    
    float sightDistance;
    float radarDistance;
    int cost;
    int population;
    int time; //in miliseconds
    float maxLife;
    float maxStamina;
    float maxFuel;
    float maxVelocity;
    float brakeRate;
    float acceleration;
    float turnRate;
    float minWaterDepth;
    float maxWaterDepth;
    float serviceCeiling;
    
    //3d model
    //idle animation
    //move animation
    //attack animation
    //death animation
    //chear animation
    //items
    //aura
    //trample damage
    
};

class Piece{
    public: 
    unsigned int id;
    unsigned int owner;
    unsigned int speciesId;
    float longitude;
    float latitude;
    float altitude;
    
    float range;
    int reloadTime; //in miliseconds
    int damage;
    int shockArmor;
    int pierceArmor;
    int cutArmor;
    int thermalArmor;
    int chemicalArmor;
    int radiationArmor;
    
    float sightDistance;
    float radarDistance;
    
    float life;
    float stamina;
    int experience;
    
    bool mobile;
    bool alive;
    
    Piece(){
        
    }
    
    Piece( const Piece& p ){
        
    }
    
    Piece& operator=( const Piece& p ){
        
        return *this;
    }
    
    void teleport(float x, float y){
        longitude = x;
        latitude = y;
    }
    
};

class Planet{
    public:
    int k; // frequency of subdivision
    int numVert; // number of vertex
    int numFace; // number of faces
    int numCell; // number of cells

    VertexList vertices;
    //VertexList rogueVertices; //not used
    FaceList faces;
    CellList cells;
    
    vector<int> owners;
    
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
    void mapVertex(PlanetVertex& v);
    void mapFace (PlanetFace& f);
    void mapFaces();
    void mapTerrain(const char* filename, float unitHeight);
    void drawFace (PlanetFace& f);
    PlanetVertex midpointOnSphere (PlanetVertex& a, PlanetVertex& b);
    PlanetVertex midpointOnSphere (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c);
    PlanetVertex midpoint (PlanetVertex& a, PlanetVertex& b);
    PlanetVertex midpoint (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c);
    void render();
    void renderWireframe();
    void generateCells();
    void renderCell(PlanetCell& c);
    void renderCells();
    void renderSelectionCells();
    void generateBorder();
    PlanetCell& findCell(float longitude, float latitude);
    void highlightCell(PlanetCell& target);
    void place(Piece& p, float x, float y);
    bool rayHitPlanet( Vec3 p, Vec3 dir, Vec3& result );
    PlanetCell& getCellFromPoint( Vec3 surfPos );
};

#endif

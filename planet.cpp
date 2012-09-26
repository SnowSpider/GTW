#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include "planet.h"
#include "imageloader.h"
#include <iostream>

using namespace std;

GLuint _textureId; //The id of the texture
GLuint id_darkSide;

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
    GLuint textureId;
    glGenTextures(1, &textureId); //Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                   //as unsigned numbers
                 image->pixels);               //The actual pixel data
    return textureId; //Returns the id of the texture
}

void Planet::init(){ //char* textureFile, char* terrainFile, float altitudeMultiplier
    vertices.clear();
    faces.clear();
    cells.clear();
    Image* image = loadBMP("earth.bmp");
    _textureId = loadTexture(image);
    delete image;
    
    Image* darkSide = loadBMP("earthlights1k.bmp");
    id_darkSide = loadTexture(darkSide);
    delete darkSide;
    
    const float sqrt5 = sqrt (5.0f);
    const float phi = (1.0f + sqrt5) * 0.5f;
    const float cRadius = sqrt (10.0f + (2.0f * sqrt5)) / (4.0f * phi); // circumscribed radius
    double a = (radius / cRadius) * 0.5;
    double b = (radius / cRadius) / (2.0f * phi);
    
    PlanetVertex v0 ( 0,  b, -a);
    PlanetVertex v1 ( b,  a,  0); //North pole ( 0,  1,  0)
    PlanetVertex v2 (-b,  a,  0);
    PlanetVertex v3 ( 0,  b,  a);
    PlanetVertex v4 ( 0, -b,  a);
    PlanetVertex v5 (-a,  0,  b);
    PlanetVertex v6 ( 0, -b, -a);
    PlanetVertex v7 ( a,  0, -b);
    PlanetVertex v8 ( a,  0,  b);
    PlanetVertex v9(-a,  0, -b);
    PlanetVertex v10( b, -a,  0);
    PlanetVertex v11(-b, -a,  0); //South pole ( 0, -1,  0)
    
    /*
    cout << "v1 = {" << v1[0] << ","
                     << v1[1] << ","
                     << v1[2] << "}" << endl;
    */
    
    //rotate everything
    float offset = atan(b/a);
    //cout << "offset = " << offset << endl; 
    v0.rotateZ(offset);
    v1.rotateZ(offset);
    v2.rotateZ(offset);
    v3.rotateZ(offset);
    v4.rotateZ(offset);
    v5.rotateZ(offset);
    v6.rotateZ(offset);
    v7.rotateZ(offset);
    v8.rotateZ(offset);
    v9.rotateZ(offset);
    v10.rotateZ(offset);
    v11.rotateZ(offset);
    
    /*
    cout << "v1 = {" << v1[0] << ","
                     << v1[1] << ","
                     << v1[2] << "}" << endl;
    */
    
    subdivide (v0, v1, v2   , k);
    subdivide (v3, v2, v1   , k);
    subdivide (v3, v4, v5   , k);
    subdivide (v3, v8, v4   , k);
    subdivide (v0, v6, v7   , k);
    subdivide (v0, v9, v6  , k);
    subdivide (v4, v10, v11 , k);
    subdivide (v6, v11, v10 , k); 
    subdivide (v2, v5, v9  , k);
    subdivide (v11, v9, v5 , k);
    subdivide (v1, v7, v8   , k);
    subdivide (v10, v8, v7    , k);
    subdivide (v3, v5, v2   , k);
    subdivide (v3, v1, v8   , k);
    subdivide (v0, v2, v9  , k);
    subdivide (v0, v7, v1   , k);
    subdivide (v6, v9, v11 , k);
    subdivide (v6, v10, v7  , k);
    subdivide (v4, v11, v5  , k);
    subdivide (v4, v8, v10  , k);
    
    /*
    cout << "init done" << endl;
    cout << "number of vertices = " << vertices.size() << endl;
    cout << "number of faces = " << faces.size() << endl;
    */
    /*
    for(int i=0;i<vertices.size();i++){
        cout << "vertex " << i << ": (" << vertices[i][0] << "," 
                                        << vertices[i][1] << "," 
                                        << vertices[i][2] << ")" << endl;
    }
    for(int i=0;i<faces.size();i++){
        cout << "face " << i << ": (" << faces[i].v[0] << "," 
                                      << faces[i].v[1] << "," 
                                      << faces[i].v[2] << ")" << endl;
    }
    */
    
    mapFaces();
    mapTerrain("terrain.bmp", 0.1);
    
    /*
    for(int i=0;i<vertices.size();i++){
        //cout << vertices[i].id << endl;
        cout << "v" << i << " has " << vertices[i].neighbors.size() << " neighbors." << endl;
    }
    */
    
    generateCells();
    cout << "number of vertex = " << vertices.size() << endl;
    cout << "number of face = " << faces.size() << endl;
    cout << "number of cell = " << cells.size() << endl;
}

void Planet::subdivide(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c, int _k){
    if(_k==0){
        //assign neighbors
        bool aNew = true;
        bool bNew = true;
        bool cNew = true;
        int aId, bId, cId;
        
        for(int i=0;i<vertices.size();i++){ //This makes init() unbearably slow, but I can't help it.
            PlanetVertex temp = vertices[i];
            
            //if(temp.equals(center)) cout << "FFFFFUUUUUUUU-" << endl;
            
            if(temp.equals(a)){
                aNew = false;
                aId = temp.id;
            }
            if(temp.equals(b)){
                bNew = false;
                bId = temp.id;
            }
            if(temp.equals(c)){
                cNew = false;
                cId = temp.id;
            }
        }
        
        if(aNew){
            mapVertex(a);
            vertices.add(a);
            aId = vertices.currentId - 1;
        }
        if(bNew){
            mapVertex(b);
            vertices.add(b);
            bId = vertices.currentId - 1;
        }
        if(cNew){
            mapVertex(c);
            vertices.add(c);
            cId = vertices.currentId - 1;
        }
        
        vertices[aId].marry(vertices[bId]); //Ugly.
        vertices[bId].marry(vertices[cId]);
        vertices[cId].marry(vertices[aId]);
        
        //PlanetFace tempFace(a, b, c);
        PlanetFace tempFace(vertices[aId], vertices[bId], vertices[cId]);
        
        /*
        cout << "tempFace: " << "v" << a.id << "," 
                             << "v" << b.id << ","
                             << "v" << c.id << endl;
        */
        faces._faces.push_back(tempFace);
        //nf++;
        
    }
    else{
        // find edge midpoints
        PlanetVertex ab = midpointOnSphere (a, b);
        PlanetVertex bc = midpointOnSphere (b, c);
        PlanetVertex ca = midpointOnSphere (c, a);
        
        // Create 4 sub-divided triangles and recurse
        subdivide ( a, ab, ca, _k-1);
        subdivide (ab,  b, bc, _k-1);
        subdivide (ca, bc,  c, _k-1);
        subdivide (ab, bc, ca, _k-1);
    }
}

void Planet::mapVertex(PlanetVertex& v){
    Vec3 w(v[0], 0.0, v[2]); 
    float latitude; 
    latitude = v.angle(axis)/PI; //north-south cout << "1...";
    float longitude; 
    if(v[0]>0) longitude = (2.0*PI - w.angle(longZero))/PI/2.0; //east-west 
    else longitude = w.angle(longZero)/PI/2.0; 
    if(!(longitude>=0&&longitude<=1)) longitude = 0.0; //longitude is -1.#IND00; yes, I know my solution is ugly.
    v.longitude = longitude;
    v.latitude = latitude;
}

void Planet::mapFace(PlanetFace& f){
    PlanetVertex& a = vertices[f.v[0]];
    PlanetVertex& b = vertices[f.v[1]];
    PlanetVertex& c = vertices[f.v[2]];
    
    float aLongitude = a.longitude;
    float bLongitude = b.longitude;
    float cLongitude = c.longitude;
    float aLatitude = a.latitude;
    float bLatitude = b.latitude;
    float cLatitude = c.latitude;
    
    // special cases for the poles
    // PlanetFaces needs bool touchsPole. need repair.
    
    /*
    if(aLongitude==0 && aLatitude==0){
        
    }
    else if(bLongitude==0 && bLatitude==0){
        
    }
    else if(cLongitude==0 && cLatitude==0){
        
    }
    */
    
    /*
    signed area = sum of 2-d cross product
    U x V = Ux*Vy-Uy*Vx
    http://howevangotburned.wordpress.com/2011/02/28/the-oddyssey-of-texturing-a-geodesic-dome/
    */
    
    if( (cLongitude*bLatitude-cLatitude*bLongitude) +
        (bLongitude*aLatitude-bLatitude*aLongitude) +
        (aLongitude*cLatitude-aLatitude*cLongitude) < 0 ){ //signed area is negative
        //glColor3f(1.0,0.0,0.0); //identify the reversed triangles
        
        /*
        if(c[0]<=0) cLongitude++;
        if(b[0]<=0) bLongitude++;
        if(a[0]<=0) aLongitude++;
        */
        
        if(c[0]<=0){
            PlanetVertex newC(c);
            newC.longitude++;
            newC.positive = false;
            vertices.add(newC);
            f.v[2] = newC.id;
        }
        if(b[0]<=0){
            PlanetVertex newB(b);
            newB.longitude++;
            newB.positive = false;
            vertices.add(newB);
            f.v[1] = newB.id;
        }
        if(a[0]<=0){
            PlanetVertex newA(a);
            newA.longitude++;
            newA.positive = false;
            vertices.add(newA);
            f.v[0] = newA.id;
        }
        f.positive = false;
    }
    
}

void Planet::mapFaces(){
    for(int i=0;i<faces.size();i++){
        /*
        mapFace( vertices[faces[i].v[0]],
                 vertices[faces[i].v[1]],
                 vertices[faces[i].v[2]] );
        */
        mapFace( faces[i] );
    }
}

void Planet::mapTerrain(const char* filename, float unitHeight){
    Image* image = loadBMP(filename);
    
    for(int i=0;i<vertices.size();i++){
        PlanetVertex& temp = vertices[i];
        int x = image->width * temp.longitude;
        int y = image->height * temp.latitude;
        unsigned char color = (unsigned char)image->pixels[3 * (y * image->width + x)];
        float h = unitHeight * ((color / 255.0f) - 0.5f);
        temp.altitude = h;
        Vec3 unitRadial = temp - center;
        unitRadial.normalize();
        Vec3 newRadial = center + (unitRadial * (radius + h));
        temp[0] = newRadial[0];
        temp[1] = newRadial[1];
        temp[2] = newRadial[2];
    }
    
    delete image;
}

void Planet::drawFace(PlanetFace& f){
    PlanetVertex& a = vertices[f.v[0]]; 
    PlanetVertex& b = vertices[f.v[1]]; 
    PlanetVertex& c = vertices[f.v[2]]; 
    
    Vec3 triCenter = (a + b + c)/ 3.0f; // face center
    Vec3 triNormal = triCenter - center; // face normal
    
    glBegin(GL_TRIANGLES);
        glNormal3d(triNormal[0], triNormal[1], triNormal[2]); //Normal for lighting
        glTexCoord2f(c.longitude, c.latitude);
        glVertex3d(c[0], c[1], c[2]); //Vertex c
        glTexCoord2f(b.longitude, b.latitude);
        glVertex3d(b[0], b[1], b[2]); //Vertex b
        glTexCoord2f(a.longitude, a.latitude);
        glVertex3d(a[0], a[1], a[2]); //Vertex a
    glEnd();
    
}

PlanetVertex Planet::midpointOnSphere (PlanetVertex& a, PlanetVertex& b){
    Vec3 midpoint = (a + b) * 0.5;
    Vec3 unitRadial = midpoint - center;
    unitRadial.normalize();
    PlanetVertex midPointOnSphere = center + (unitRadial * radius);
    //mapVertex(midPointOnSphere);
    return midPointOnSphere;
}

PlanetVertex Planet::midpointOnSphere (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
    Vec3 midpoint = (a + b + c)/3.0;
    Vec3 unitRadial = midpoint - center;
    unitRadial.normalize();
    PlanetVertex midPointOnSphere = center + (unitRadial * radius);
    return midPointOnSphere;
}

PlanetVertex Planet::midpoint (PlanetVertex& a, PlanetVertex& b){
    Vec3 midpoint = (a + b) * 0.5;
    return midpoint;
}

PlanetVertex Planet::midpoint (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
    Vec3 midpoint = (a + b + c)/3.0;
    return midpoint;
}

void Planet::render(){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    for(int i=0;i<faces.size();i++){
        
        /*
        //dark side
        PlanetFace& currentFace = faces[i];
        float currentLongitude = (vertices[currentFace.v[0]].longitude +
                                  vertices[currentFace.v[1]].longitude +
                                  vertices[currentFace.v[2]].longitude) / 3.0;
        if(currentLongitude > 0.5){
            glBindTexture(GL_TEXTURE_2D, id_darkSide);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else{
            glBindTexture(GL_TEXTURE_2D, _textureId);
        }
        */
        
        //if(!faces[i]) glColor3f(0.5,0.0,0.0);
        drawFace( faces[i] );
        //glColor3f(1,1,1);
    }
    glDisable(GL_TEXTURE_2D);
    
    /*
    for(int i=0;i<vertices.size();i++){
        if(!vertices[i]){
            glBegin(GL_LINES);
            glColor3f(1.0,0.0,0.0);
            glVertex3f(0,0,0);
            glVertex3f(vertices[i][0]*2,vertices[i][1]*2,vertices[i][2]*2);
            glEnd();
        }
    }
    */
    //cout << "numVerts = " << vertices.size() << endl;
}

void Planet::renderWireframe(){
    //glEnable(GL_COLOR_MATERIAL);
    glColor3f(1.0,0.0,0.0);
    for(int i=0;i<faces.size();i++){
        PlanetVertex& a = vertices[faces[i].v[0]];
        PlanetVertex& b = vertices[faces[i].v[1]];
        PlanetVertex& c = vertices[faces[i].v[2]];
        Vec3 triCenter = (a + b + c)/ 3.0f; // face center
        Vec3 triNormal = triCenter - center; // face normal
        glBegin(GL_LINES);
            glNormal3d(triNormal[0], triNormal[1], triNormal[2]); //Normal for lighting
            glVertex3d(c[0], c[1], c[2]); //Vertex c
            glVertex3d(b[0], b[1], b[2]); //Vertex b
            glVertex3d(a[0], a[1], a[2]); //Vertex a
            glVertex3d(c[0], c[1], c[2]); //Vertex c
        glEnd();
    }
}

void Planet::generateCells(){
    //cout << "step 0" << endl;
    for(int i=0;i<vertices.size();i++){
        if(vertices[i].neighbors.size()>0){
            PlanetCell tempCell(vertices[i]);
            vector<unsigned int> n = tempCell.neighbors;
            PlanetVertex newVert;
            for(int j=0;j<n.size();j++){
                PlanetVertex currentNeighbor = vertices[n[j]];
                //cout << "vertices[i] = (" << vertices[i][0] << "," << vertices[i][1] << "," << vertices[i][0] << ")" << endl;
                //cout << "vertices[n[j]] = (" << vertices[n[j]][0] << "," << vertices[n[j]][1] << "," << vertices[n[j]][0] << ")" << endl;
                newVert = midpoint(vertices[i], currentNeighbor);
                //cout << "tempVert = (" << tempVert[0] << "," << tempVert[1] << "," << tempVert[0] << ")" << endl;
                vertices.add(newVert); // NOT shared between cells
                tempCell.paramVerts.push_back(vertices.currentId - 1);
            }
            for(int j=0;j<faces.size();j++){
                if( vertices[faces[j].v[0]].equals(vertices[i]) || 
                    vertices[faces[j].v[1]].equals(vertices[i]) || 
                    vertices[faces[j].v[2]].equals(vertices[i]) ){
                    //newVert = faces[j].center;
                    newVert = midpoint( vertices[faces[j].v[0]], 
                                         vertices[faces[j].v[1]], 
                                         vertices[faces[j].v[2]] );
                    vertices.add(newVert);
                    tempCell.paramVerts.push_back(vertices.currentId - 1);
                }
            }
            //sorting the paramVerts
            vector<unsigned int> x = tempCell.paramVerts;
            vector<unsigned int> paramVerts_sorted;
            unsigned int tempId;
            int index = 0;
            float min;
            while(x.size()>0){
                tempId = x[index];
                x.erase(x.begin()+index);
                paramVerts_sorted.push_back(tempId);
                min = radius; //critical
                for(int j=0;j<x.size();j++){
                    float edgeLength = (vertices[tempId] - vertices[x[j]]).length();
                    if(edgeLength < min){
                        min = edgeLength;
                        index = j;
                    }
                }
            }
            tempCell.paramVerts = paramVerts_sorted;
            cells.add(tempCell);
            //cout << "number of paramVerts = " << tempCell.paramVerts.size() << endl;
        }
    }
    //cout << "generateCells() finished." << endl;
}

void Planet::renderCells(){
    /*
    glColor3f(0.0,1.0,0.0);
    for(int i=0;i<faces.size();i++){
        PlanetFace f = faces[i];
        PlanetVertex& a = vertices[f.v[0]]; 
        PlanetVertex& b = vertices[f.v[1]]; 
        PlanetVertex& c = vertices[f.v[2]]; 
        Vec3 triCenter = midpoint(a, b, c);
        Vec3 triNormal = triCenter - center;
        PlanetVertex p = midpoint(a,b);
        PlanetVertex q = midpoint(b,c);
        PlanetVertex r = midpoint(c,a);
        glBegin(GL_LINES);
            glNormal3d(triNormal[0], triNormal[1], triNormal[2]);
            glVertex3d(triCenter[0], triCenter[1], triCenter[2]); 
            glVertex3d(p[0], p[1], p[2]); 
        glEnd();
        glBegin(GL_LINES);
            glNormal3d(triNormal[0], triNormal[1], triNormal[2]);
            glVertex3d(triCenter[0], triCenter[1], triCenter[2]); 
            glVertex3d(q[0], q[1], q[2]); 
        glEnd();
        glBegin(GL_LINES);
            glNormal3d(triNormal[0], triNormal[1], triNormal[2]);
            glVertex3d(triCenter[0], triCenter[1], triCenter[2]); 
            glVertex3d(r[0], r[1], r[2]); 
        glEnd();
    }
    */
    
    glColor3f(0.0,1.0,0.0);
    for(int i=0;i<cells.size();i++){
        PlanetCell tempCell = cells[i];
        vector<unsigned int> p = tempCell.paramVerts;
        Vec3 triCenter = vertices[tempCell.centerId];
        Vec3 triNormal = triCenter - center;
        
        for(int j=0;j<p.size();j++){
            
            glBegin(GL_LINES);
            glNormal3d(triNormal[0], triNormal[1], triNormal[2]);
            glVertex3d(vertices[p[j]][0], vertices[p[j]][1], vertices[p[j]][2]);
            if(j+1==p.size()){
                glVertex3d(vertices[p[0]][0], vertices[p[0]][1], vertices[p[0]][2]);
            }
            else{
                glVertex3d(vertices[p[j+1]][0], vertices[p[j+1]][1], vertices[p[j+1]][2]);
            }
            glEnd();
            
            /*
            Vec3 temp = vertices[p[j]];
            glBegin(GL_LINES);
            glVertex3d(center[0], center[1], center[2]);
            glVertex3d(temp[0], temp[1], temp[2]);
            glEnd();
            */
            /*
            Vec3 a = vertices[p[j]];
            for(int l=0;l<p.size();l++){ // need optimization; paramVerts need to be sorted counterclockwise
                Vec3 b = vertices[p[l]];
                glBegin(GL_TRIANGLES);
                    glNormal3d(triNormal[0], triNormal[1], triNormal[2]);
                    glVertex3d(a[0], a[1], a[2]); 
                    glVertex3d(b[0], b[1], b[2]);
                    glVertex3d(triCenter[0], triCenter[1], triCenter[2]); 
                glEnd();
            }
            */
        }
    }
}

void Planet::generateBorder(){
    
}

PlanetCell& Planet::findCell(float longitude, float latitude){ //input in degrees
    /*
    First, the point normalized to a direction vector. 
    The angle between the point and the hex center can be found by 
    
    acos( (n dot hexpos ) / planetRadius ). 
    
    Whichever hex has the smallest such angle contains the point. 
    In my demo, checking every hex was fast enough even on a large subdivision level, 
    for a game you would probably want to add an acceleration structure 
    (perhaps as a 2D kd-tree in spherical coordinates).
    */
    longitude = longitude * PI / 180.0;
    latitude = latitude * PI / 180.0;
    
    Vec3 target( center[0] + (radius * cos(longitude) * sin(latitude)),
                 center[0] + (radius * sin(longitude) * sin(latitude)), 
                 center[0] + (radius * cos(latitude)));
    float min = PI;
    float temp;
    int tempId = -1;
    for(int i=0;i<cells.size();i++){
        temp = target.angle(vertices[cells[i].centerId]);
        if(temp < min){
            min = temp;
            tempId = i;
        }
    }
    return cells[tempId];
}

void Planet::highlightCell(PlanetCell& target){
    
}

void place(Piece& p, float x, float y){
    
}






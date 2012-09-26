#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>

#include "planet.h"
#include "imageloader.h"
#include "vec.h"

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

void Planet::init(){

    // Clear the datastore
    vertices.clear();
    faces.clear();
    cells.clear();
    
    // Load texture images
    Image* image;
    
    image = loadBMP("data/texture/earth/earth_night.bmp");
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
    
    // Compute the 3D vertices of an icosahedron
    const float sqrt5 = sqrt (5.0f);
    const float phi = (1.0f + sqrt5) * 0.5f;
    const float cRadius = sqrt (10.0f + (2.0f * sqrt5)) / (4.0f * phi); // circumscribed radius
    double a = (radius / cRadius) * 0.5;
    double b = (radius / cRadius) / (2.0f * phi);
    
    PlanetVertex v0 ( 0,  b, -a);
    PlanetVertex v1 ( b,  a,  0); 
    PlanetVertex v2 (-b,  a,  0);
    PlanetVertex v3 ( 0,  b,  a);
    PlanetVertex v4 ( 0, -b,  a);
    PlanetVertex v5 (-a,  0,  b);
    PlanetVertex v6 ( 0, -b, -a);
    PlanetVertex v7 ( a,  0, -b);
    PlanetVertex v8 ( a,  0,  b);
    PlanetVertex v9 (-a,  0, -b);
    PlanetVertex v10( b, -a,  0);
    PlanetVertex v11(-b, -a,  0); 
    
    //rotate everything
    float offset = atan(b/a);
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
    
    // Start subdivision
    subdivide (v0,  v1,  v2  , complexity);
    subdivide (v3,  v2,  v1  , complexity);
    subdivide (v3,  v4,  v5  , complexity);
    subdivide (v3,  v8,  v4  , complexity);
    subdivide (v0,  v6,  v7  , complexity);
    subdivide (v0,  v9,  v6  , complexity);
    subdivide (v4,  v10, v11 , complexity);
    subdivide (v6,  v11, v10 , complexity); 
    subdivide (v2,  v5,  v9  , complexity);
    subdivide (v11, v9,  v5  , complexity);
    subdivide (v1,  v7,  v8  , complexity);
    subdivide (v10, v8,  v7  , complexity);
    subdivide (v3,  v5,  v2  , complexity);
    subdivide (v3,  v1,  v8  , complexity);
    subdivide (v0,  v2,  v9  , complexity);
    subdivide (v0,  v7,  v1  , complexity);
    subdivide (v6,  v9,  v11 , complexity);
    subdivide (v6,  v10, v7  , complexity);
    subdivide (v4,  v11, v5  , complexity);
    subdivide (v4,  v8,  v10 , complexity);
    
    // Apply the terrain map.
    // The order is important in the next two function calls. 
    // If you call fixSeam() first, you will see rifts in the polar regions,
    // because fixSeam adds some vertices to the vertex list.
    //mapTerrain("data/texture/earth/earth_terrain.bmp", 0.1);

    // Fix the seam.
    fixSeam();
    
    // Create cells.
    genCells();
    
    // Debugging.
    cout << "number of vertex = " << vertices.size() << endl;
    cout << "number of face = " << faces.size() << endl;
    cout << "number of cell = " << cells.size() << endl;
    cout << "k = " << complexity << endl;
}

void Planet::subdivide(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c, int k){
    if(k==0){
        //assign neighbors
        bool aNew = true;
        bool bNew = true;
        bool cNew = true;
        int aId, bId, cId;
        
        // OPT NOTE: This makes init() unbearably slow.
        for(int i=0;i<vertices.size();i++){ 
            PlanetVertex temp = vertices[i];
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
        
        // OPT NOTE: Ugly.
        vertices[aId].marry(vertices[bId]); 
        vertices[bId].marry(vertices[cId]);
        vertices[cId].marry(vertices[aId]);
        
        PlanetFace tempFace(vertices[aId], vertices[bId], vertices[cId]);
        faces.add(tempFace);
    }
    else{
        // Find edge midpoints.
        PlanetVertex ab = midpointOnSphere (a, b);
        PlanetVertex bc = midpointOnSphere (b, c);
        PlanetVertex ca = midpointOnSphere (c, a);
        
        // Create 4 subdivided triangles and recurse.
        subdivide ( a, ab, ca, k-1);
        subdivide (ab,  b, bc, k-1);
        subdivide (ca, bc,  c, k-1);
        subdivide (ab, bc, ca, k-1);
    }
}

void Planet::mapVertex(PlanetVertex& v){
    Vec3 w(v[0], 0.0, v[2]); 
    float latitude; 
    latitude = v.angle(axis)/PI; 
    float longitude; 
    if(v[0]>0) longitude = (2.0*PI - w.angle(longitude_zero))/PI/2.0; 
    else longitude = w.angle(longitude_zero)/PI/2.0; 
    if(!(longitude>=0&&longitude<=1)) longitude = 0.0; //longitude is -1.#IND00
    v.longitude = longitude;
    v.latitude = latitude;
}

void Planet::fixSeamHelper(PlanetFace& f){
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
        (aLongitude*cLatitude-aLatitude*cLongitude) < 0 ){ 
        // Signed area is negative
        if(c[0]<=0){
            PlanetVertex newC(c);
            newC.longitude++;
            newC.positive = false;
            vertices.add(newC);
            f.v[2] = newC.id; //critical
        }
        if(b[0]<=0){
            PlanetVertex newB(b);
            newB.longitude++;
            newB.positive = false;
            vertices.add(newB);
            f.v[1] = newB.id; //critical
        }
        if(a[0]<=0){
            PlanetVertex newA(a);
            newA.longitude++;
            newA.positive = false;
            vertices.add(newA);
            f.v[0] = newA.id; //critical
        }
        f.positive = false;
    }
}

void Planet::fixSeam(){
    for(int i=0;i<faces.size();i++){
        fixSeamHelper( faces[i] );
    }
}

void Planet::mapTerrain(const char* filename, float unitHeight){ //need repair: changes accumulate on old vertices. That's causing the poles to protrude.
    Image* image = loadBMP(filename);
    for(int i=0;i<vertices.size();i++){
        PlanetVertex& temp = vertices[i];
        int x = image->width * temp.longitude;
        int y = image->height * temp.latitude;
        unsigned char color = (unsigned char)image->pixels[3 * (y * image->width + x)];
        float h = unitHeight * ((color / 255.0f) - 0.5f);
        temp.altitude = h;
        Vec3 unitRadial = temp - center; //fix this line
        unitRadial.normalize();
        Vec3 newRadial = center + (unitRadial * (radius + h));
        temp[0] = newRadial[0];
        temp[1] = newRadial[1];
        temp[2] = newRadial[2];
    }
    delete image;    
}

PlanetVertex Planet::midpointOnSphere (PlanetVertex& a, PlanetVertex& b){
    Vec3 midpoint = (a + b) * 0.5;
    Vec3 unitRadial = midpoint - center;
    unitRadial.normalize();
    PlanetVertex midPointOnSphere = center + (unitRadial * radius);
    return midPointOnSphere;
}

PlanetVertex Planet::midpointOnSphere (PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
    Vec3 midpoint = (a + b + c)/3.0;
    Vec3 unitRadial = midpoint - center;
    unitRadial.normalize();
    PlanetVertex midPointOnSphere = center + (unitRadial * radius);
    return midPointOnSphere;
}

void Planet::drawFace(PlanetFace& f){
    PlanetVertex& a = vertices[f.v[0]]; 
    PlanetVertex& b = vertices[f.v[1]]; 
    PlanetVertex& c = vertices[f.v[2]]; 
    
    Vec3 triCenter = (a + b + c)/ 3.0f; // face center
    Vec3 triNormal = triCenter - center; // face normal
    
    //glLoadName(f.id);
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

void Planet::renderEarth(){
    //glInitNames();
    //cout << "Rendering...";
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId_day);
    
    //blocky texture mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    for(int i=0;i<faces.size();i++){
        
        /*
        //dark side
        PlanetFace& currentFace = faces[i];
        float currentLongitude = (vertices[currentFace.v[0]].longitude +
                                  vertices[currentFace.v[1]].longitude +
                                  vertices[currentFace.v[2]].longitude) / 3.0;
        if(currentLongitude < 0.5){
            glBindTexture(GL_TEXTURE_2D, texId_night);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else{
            glBindTexture(GL_TEXTURE_2D, texId_night);
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
    //cout << "done." << endl;
}

void Planet::renderWireframe(){
    glColor3f(1.0,0.0,0.0);
    for(int i=0;i<faces.size();i++){
        PlanetVertex& a = vertices[faces[i].v[0]];
        PlanetVertex& b = vertices[faces[i].v[1]];
        PlanetVertex& c = vertices[faces[i].v[2]];
        Vec3 triCenter = (a + b + c)/ 3.0f; 
        Vec3 triNormal = triCenter - center; 
        glBegin(GL_LINES);
            glNormal3d(triNormal[0], triNormal[1], triNormal[2]); 
            glVertex3d(c[0], c[1], c[2]); 
            glVertex3d(b[0], b[1], b[2]); 
            glVertex3d(a[0], a[1], a[2]); 
            glVertex3d(c[0], c[1], c[2]); 
        glEnd();
    }
}

void Planet::genCells(){
    for(int i=0;i<vertices.size();i++){
        if(vertices[i].neighbors.size()>0){
            //cout << "neighbors.size() = " << vertices[i].neighbors.size() << endl;
            PlanetCell tempCell(vertices[i]);
            vector<size_t> n = tempCell.neighbors;
            PlanetVertex newVert;
            for(int j=0;j<n.size();j++){
                PlanetVertex currentNeighbor = vertices[n[j]];
                newVert = midpoint(vertices[i], currentNeighbor);
                mapVertex(newVert); //necessary for the sorting part
                vertices.add(newVert); // NOT shared between cells
                tempCell.paramVerts.push_back(vertices.currentId - 1);
            }
            for(int j=0;j<faces.size();j++){
                if( vertices[faces[j].v[0]].equals(vertices[i]) || 
                    vertices[faces[j].v[1]].equals(vertices[i]) || 
                    vertices[faces[j].v[2]].equals(vertices[i]) ){
                    //newVert = faces[j].center; //this variable is not updated
                    newVert = midpoint( vertices[faces[j].v[0]], 
                                        vertices[faces[j].v[1]], 
                                        vertices[faces[j].v[2]] );
                    mapVertex(newVert);
                    vertices.add(newVert); //necessary for the sorting part
                    tempCell.paramVerts.push_back(vertices.currentId - 1);
                }
            }
            // Sort the paramVerts.
            vector<size_t> x = tempCell.paramVerts;
            vector<size_t> paramVerts_sorted;
            size_t tempId;
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
            //At this point, paramVerts may have been sorted clockwise OR counterclockwise.
            tempCell.paramVerts = paramVerts_sorted;
            cells.add(tempCell);
        }
    }
}

void Planet::renderCellBoundary(PlanetCell& c){
    vector<size_t> p = c.paramVerts;
    Vec3 triCenter = vertices[c.centerId];
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
        //for debugging
        Vec3 temp = vertices[p[j]];
        glBegin(GL_LINES);
        glVertex3d(center[0], center[1], center[2]);
        glVertex3d(temp[0], temp[1], temp[2]);
        glEnd();
        */
        /*
        //for debugging
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

PlanetCell& Planet::getCellAt(float longitude, float latitude){ //input in degrees
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
    
    Vec3 targetRadial( center[0] + (radius * cos(longitude) * sin(latitude)),
                       center[0] + (radius * sin(longitude) * sin(latitude)), 
                       center[0] + (radius * cos(latitude)));
    float min = PI;
    float temp;
    int tempId = -1;
    for(int i=0;i<cells.size();i++){
        temp = targetRadial.angle(vertices[cells[i].centerId]);
        if(temp < min){
            min = temp;
            tempId = i;
        }
    }
    return cells[tempId];
}

PlanetCell& Planet::getCellAt( Vec3 p ){
    Vec3 targetRadial = p - center;
    float minAngle = 2 * PI;
    float currentAngle;
    int bestCell;
    for(int i=0;i<cells.size();i++){
        currentAngle = targetRadial.angle(vertices[cells[i].centerId]);
        if(currentAngle < minAngle){
            minAngle = currentAngle;
            bestCell = i;
        }
    }
    return cells[bestCell];
}

size_t Planet::getCellIdAt( Vec3 p ){
    Vec3 targetRadial = p - center;
    float minAngle = 2 * PI;
    float currentAngle;
    int bestCell;
    for(int i=0;i<cells.size();i++){
        currentAngle = targetRadial.angle(vertices[cells[i].centerId]);
        if(currentAngle < minAngle){
            minAngle = currentAngle;
            bestCell = i;
        }
    }
    return bestCell;
}

void Planet::renderAxis(){
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    
    glColor3f(1.0,0.0,0.0);
    glVertex3f(center[0],center[1],center[2]);
    glVertex3f(center[0]+1.0,center[1],center[2]);
    
    glColor3f(0.0,1.0,0.0);
    glVertex3f(center[0],center[1],center[2]);
    glVertex3f(center[0],center[1]+1.0,center[2]);
    
    glColor3f(0.0,0.0,1.0);
    glVertex3f(center[0],center[1],center[2]);
    glVertex3f(center[0],center[1],center[2]+1.0);
    
    glEnd();
    glEnable(GL_LIGHTING);
}

void Planet::renderCells(){
    glColor3f(1.0,1.0,1.0);
    for(int i=0;i<cells.size();i++){
        renderCellBoundary(cells[i]);
    }
}

bool Planet::rayHitPlanet( Vec3 p, Vec3 dir, Vec3 &result ){
    float a,b,c,d;
    a = dir.dot(dir);
    b = (2.0f*dir).dot(p);
    c = p.dot(p) - (radius*radius);
    d = b*b - 4.0f*a*c;
    if (d <=0 ) return false;
    result = p + ((-b - sqrt(d)) / 2.0f*a)*dir;
    return true;
}



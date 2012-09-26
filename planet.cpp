#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "planet.h"
#include "imageloader.h"
#include <iostream>

using namespace std;

GLuint _textureId; //The id of the texture

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
    vertices.clear();
    faces.clear();
    Image* image = loadBMP("earth.bmp");
	_textureId = loadTexture(image);
	delete image;
	
	const float sqrt5 = sqrt (5.0f);
    const float phi = (1.0f + sqrt5) * 0.5f;
    const float cRadius = sqrt (10.0f + (2.0f * sqrt5)) / (4.0f * phi); // circumscribed radius
    double a = (radius / cRadius) * 0.5;
    double b = (radius / cRadius) / (2.0f * phi);
    
    PlanetVertex v1 ( 0,  b, -a);
    PlanetVertex v2 ( b,  a,  0); //North pole ( 0,  1,  0)
    PlanetVertex v3 (-b,  a,  0);
    PlanetVertex v4 ( 0,  b,  a);
    PlanetVertex v5 ( 0, -b,  a);
    PlanetVertex v6 (-a,  0,  b);
    PlanetVertex v7 ( 0, -b, -a);
    PlanetVertex v8 ( a,  0, -b);
    PlanetVertex v9 ( a,  0,  b);
    PlanetVertex v10(-a,  0, -b);
    PlanetVertex v11( b, -a,  0);
    PlanetVertex v12(-b, -a,  0); //South pole ( 0, -1,  0)
    
    
    vertices.add(v1);
    vertices.add(v2);
    vertices.add(v3);
    vertices.add(v4);
    vertices.add(v5);
    vertices.add(v6);
    vertices.add(v7);
    vertices.add(v8);
    vertices.add(v9);
    vertices.add(v10);
    vertices.add(v11);
    vertices.add(v12);
    
    
	subdivide (v1, v2, v3   , k);
	subdivide (v4, v3, v2   , k);
	subdivide (v4, v5, v6   , k);
	subdivide (v4, v9, v5   , k);
	subdivide (v1, v7, v8   , k);
	subdivide (v1, v10, v7  , k);
	subdivide (v5, v11, v12 , k);
	subdivide (v7, v12, v11 , k); 
	subdivide (v3, v6, v10  , k);
	subdivide (v12, v10, v6 , k);
	subdivide (v2, v8, v9   , k);
	subdivide (v11, v9, v8	, k);
	subdivide (v4, v6, v3   , k);
	subdivide (v4, v2, v9   , k);
	subdivide (v1, v3, v10  , k);
	subdivide (v1, v8, v2   , k);
	subdivide (v7, v10, v12 , k);
	subdivide (v7, v11, v8  , k);
	subdivide (v5, v12, v6  , k);
	subdivide (v5, v9, v11  , k);
	
	cout << "init done" << endl;
	cout << "number of vertices = " << vertices.size() << endl;
	cout << "number of faces = " << faces.size() << endl;
	
	for(int i=0;i<faces.size();i++){
	    cout << "face " << i << ": (" << faces[i].v[0] << "," 
	                                  << faces[i].v[1] << "," 
	                                  << faces[i].v[2] << ")" << endl;
	}
	
}

void Planet::subdivide(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c, int _k){
    if(_k==0){
        
        /*
        bool aIsUnique = true;
        bool bIsUnique = true;
        bool cIsUnique = true;
        
        PlanetVertex* tempA = &a;
        PlanetVertex* tempB = &b;
        PlanetVertex* tempC = &c;
        
        for(int i=0;i<vertices.size();i++){
            if(vertices[i].equals(a)){
                tempA = &vertices[i];
                aIsUnique = false;
                break;
            }
            if(vertices[i].equals(b)){
                tempB = &vertices[i];
                bIsUnique = false;
                break;
            }
            if(vertices[i].equals(c)){
                tempC = &vertices[i];
                cIsUnique = false;
                break;
            }
        }
        
        if(aIsUnique) vertices.add(*tempA);
        if(bIsUnique) vertices.add(*tempB); 
        if(cIsUnique) vertices.add(*tempC);
        */
        /*
        vertices.add(a);
        vertices.add(b); 
        vertices.add(c);
        */
        /*
        a.neighbors.push_back(b.id);
        a.neighbors.push_back(c.id);
        b.neighbors.push_back(a.id);
        b.neighbors.push_back(c.id);
        c.neighbors.push_back(a.id);
        c.neighbors.push_back(b.id);
        */
        
        PlanetFace tempFace(a, b, c);
        cout << "tempFace: " << "v" << a.id << "," 
                             << "v" << b.id << ","
                             << "v" << c.id << endl;
        
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

void Planet::mapFace(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
    Vec3 faceCenter = (a + b + c)/ 3.0f; // face center
	Vec3 faceNormal = faceCenter - center; // face normal
	
    /*
    unsigned char color = (unsigned char)terrain->pixels[3 * (latitude * terrain->width + longitude)];
	float h = height * ((color / 255.0f) - 0.5f);
    */
    
    Vec3 cw = Vec3(c[0], 0.0, c[2]); 
	float cLatitude; 
	cLatitude = c.angle(axis)/PI; //north-south cout << "1...";
	float cLongitude; 
	if(c[0]>0) cLongitude = (2.0*PI - cw.angle(longZero))/PI/2.0; //east-west 
	else cLongitude = cw.angle(longZero)/PI/2.0; 
	if(!(cLongitude>=0&&cLongitude<=1)) cLongitude = 0.0; //longitude is -1.#IND00; yes, I know my solution is ugly.
    
	Vec3 bw = Vec3(b[0], 0.0, b[2]);
	float bLatitude;
	bLatitude = b.angle(axis)/PI; 
	float bLongitude;
	if(b[0]>0) bLongitude = (2.0*PI - bw.angle(longZero))/PI/2.0; 
	else bLongitude = bw.angle(longZero)/PI/2.0; 
	if(!(bLongitude>=0&&bLongitude<=1)) bLongitude = 0.0;
	
	Vec3 aw = Vec3(a[0], 0.0, a[2]);
	float aLatitude;
	aLatitude = a.angle(axis)/PI; 
	float aLongitude;
	if(a[0]>0) aLongitude = (2.0*PI - aw.angle(longZero))/PI/2.0; 
	else aLongitude = aw.angle(longZero)/PI/2.0;
	if(!(aLongitude>=0&&aLongitude<=1)) aLongitude = 0.0;
    
	/*
	signed area = sum of 2-d cross product
	U x V = Ux*Vy-Uy*Vx
	http://howevangotburned.wordpress.com/2011/02/28/the-oddyssey-of-texturing-a-geodesic-dome/
	*/
	
	if( (cLongitude*bLatitude-cLatitude*bLongitude) +
		(bLongitude*aLatitude-bLatitude*aLongitude) +
		(aLongitude*cLatitude-aLatitude*cLongitude) < 0 ){
		//glColor3f(1.0,0.0,0.0); //identify the reversed triangles
		if(c[0]<=0) cLongitude++;
		if(b[0]<=0) bLongitude++;
		if(a[0]<=0) aLongitude++;
	}
	
	// terrain mapping 

	/*
	glTexCoord2f(cLongitude, cLatitude);
	glVertex3d(c[0], c[1], c[2]); //Vertex c
	glTexCoord2f(bLongitude, bLatitude);
	glVertex3d(b[0], b[1], b[2]); //Vertex b
	glTexCoord2f(aLongitude, aLatitude);
	glVertex3d(a[0], a[1], a[2]); //Vertex a
	*/
	
	a.longitude = aLongitude;
	b.longitude = bLongitude;
	c.longitude = cLongitude;
	
	a.latitude = aLatitude;
	b.latitude = bLatitude;
	c.latitude = cLatitude;
}



void Planet::mapFaces(){
    for(int i=0;i<faces.size();i++){
        mapFace( vertices[faces[i].v[0]],
                 vertices[faces[i].v[1]],
                 vertices[faces[i].v[2]] );
    }
}

void Planet::drawFace(PlanetVertex& a, PlanetVertex& b, PlanetVertex& c){
	Vec3 triCenter = (a + b + c)/ 3.0f; // face center
	Vec3 triNormal = triCenter - center; // face normal
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	
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
	
	
	for(int i=0;i<vertices.size();i++){
	    if(vertices[i].equals(midPointOnSphere)){
	        return vertices[i];
	    }
	}
	
	vertices.add(midPointOnSphere);
	
	return midPointOnSphere;
}

void Planet::render(){
    for(int i=0;i<faces.size();i++){
        drawFace( vertices[faces[i].v[0]],
                  vertices[faces[i].v[1]],
                  vertices[faces[i].v[2]] );
    }
}




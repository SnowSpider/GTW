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

GLuint sphereDL2;
GLuint _textureId2; //The id of the texture

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture2(Image* image) {
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
    Image* image = loadBMP("earth.bmp");
	_textureId2 = loadTexture2(image);
	delete image;
	
    //Initialize a platonic solid
    //Here we go with an Icosahedron
    //Icosahedron stats:
    //Vertices: 12
    //Edges: 30
    //Faces: 20
    //Edges per face: 3
    //Edges shared per vertex: 5
    //Edge length = golden ratio
    const float sqrt5 = sqrt (5.0f);
    const float phi = (1.0f + sqrt5) * 0.5f;
	// Circumscribed radius
    const float cRadius = sqrt (10.0f + (2.0f * sqrt5)) / (4.0f * phi);
    //Now we define constants which will define our Icosahedron's vertices
    double a = (radius / cRadius) * 0.5;
    double b = (radius / cRadius) / (2.0f * phi);
    
    PlanetVertex v1(0, b, -a);
    PlanetVertex v2(b, a, 0); //North pole ( 0,  1,  0)
    PlanetVertex v3(-b, a, 0);
    PlanetVertex v4(0, b, a);
    PlanetVertex v5(0, -b, a);
    PlanetVertex v6(-a, 0, b);
    PlanetVertex v7(0, -b, -a);
    PlanetVertex v8(a, 0, -b);
    PlanetVertex v9(a, 0, b);
    PlanetVertex v10(-a, 0, -b);
    PlanetVertex v11(b, -a, 0);
    PlanetVertex v12(-b, -a, 0); //South pole ( 0, -1,  0)
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);
    vertices.push_back(v9);
    vertices.push_back(v10);
    vertices.push_back(v11);
    vertices.push_back(v12);
}

void Planet::refine(){
    cout << "k = " << k << endl;
    sphereDL2 = glGenLists(1);
	glNewList(sphereDL2,GL_COMPILE);
    subdivide (vertices[0], vertices[1], vertices[2], k);
	subdivide (vertices[3], vertices[2], vertices[1], k);
	subdivide (vertices[3], vertices[4], vertices[5], k);
	subdivide (vertices[3], vertices[8], vertices[4], k);
	subdivide (vertices[0], vertices[6], vertices[7], k);
	subdivide (vertices[0], vertices[9], vertices[6], k);
	subdivide (vertices[4], vertices[10], vertices[11], k);
	subdivide (vertices[6], vertices[11], vertices[10], k); 
	subdivide (vertices[2], vertices[5], vertices[9], k);
	subdivide (vertices[11], vertices[9], vertices[5], k);
	subdivide (vertices[1], vertices[7], vertices[8], k);
	subdivide (vertices[10], vertices[8], vertices[7], k);
	subdivide (vertices[3], vertices[5], vertices[2], k);
	subdivide (vertices[3], vertices[1], vertices[8], k);
	subdivide (vertices[0], vertices[2], vertices[9], k);
	subdivide (vertices[0], vertices[7], vertices[1], k);
	subdivide (vertices[6], vertices[9], vertices[11], k);
	subdivide (vertices[6], vertices[10], vertices[7], k);
	subdivide (vertices[4], vertices[11], vertices[5], k);
	subdivide (vertices[4], vertices[8], vertices[10], k);
	glEndList();
}

void Planet::subdivide(const PlanetVertex& a, const PlanetVertex& b, const PlanetVertex& c, const int& _k){
    cout << "subdivide..." << endl;
    if(_k==0){
        cout << "k==0" << endl;
        // draw triangle
        PlanetFace tempFace(a, b, c);
        cout << "tempFace constructed" << endl;
        //faces[_k].push_back(tempFace);
        faces.push_back(tempFace);
        cout << "drawFace..." << endl;
		drawFace (a, b, c);
		//drawTriangle (a, b, c);
		//nf++;
    }
    else{
        cout << "k!=0" << endl;
        // find edge midpoints
		const PlanetVertex ab = midpointOnSphere (a, b);
		const PlanetVertex bc = midpointOnSphere (b, c);
		const PlanetVertex ca = midpointOnSphere (c, a);

		// Create 4 sub-divided triangles and recurse
		subdivide ( a, ab, ca, _k-1);
		subdivide (ab,  b, bc, _k-1);
		subdivide (ca, bc,  c, _k-1);
		subdivide (ab, bc, ca, _k-1);
    }
}

//Draw a single triangle
void Planet::drawFace (const Vec3& a, const Vec3& b, const Vec3& c) {
	Vec3 triCenter = (a + b + c)/ 3.0f; // face center
	Vec3 triNormal = triCenter - center; // face normal
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId2);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //blocky texture mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glBegin(GL_TRIANGLES);
		glNormal3d(triNormal[0], triNormal[1], triNormal[2]); //Normal for lighting

		/*
		unsigned char color = (unsigned char)terrain->pixels[3 * (latitude * terrain->width + longitude)];
			float h = height * ((color / 255.0f) - 0.5f);
		*/

		Vec3 cw = Vec3(c[0], 0.0, c[2]);
		float cLatitude;
		cLatitude = c.angle(axis)/PI; //north-south
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
		if((cLongitude*bLatitude-cLatitude*bLongitude)+
			(bLongitude*aLatitude-bLatitude*aLongitude)+
			(aLongitude*cLatitude-aLatitude*cLongitude)<0){
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
		
		glTexCoord2f(cLongitude, cLatitude);
		glVertex3d(c[0], c[1], c[2]); //Vertex c
		glTexCoord2f(bLongitude, bLatitude);
		glVertex3d(b[0], b[1], b[2]); //Vertex b
		glTexCoord2f(aLongitude, aLatitude);
		glVertex3d(a[0], a[1], a[2]); //Vertex a
		
	glEnd();
}

//Project the mid-point of a triangle edge on the sphere
Planet::PlanetVertex Planet::midpointOnSphere (const PlanetVertex& a, const  PlanetVertex& b) {
	Vec3 midpoint = (a + b) * 0.5;
	Vec3 unitRadial = midpoint - center;
	unitRadial.normalize();
	PlanetVertex midPointOnSphere = center + (unitRadial * radius);

	return midPointOnSphere;
}

void Planet::drawTriangle(const PlanetVertex& a, const PlanetVertex& b, const PlanetVertex& c){
	Vec3 triCenter = (a + b + c)/ 3.0f; // face center
	Vec3 triNormal = triCenter - center; // face normal
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId2);
	
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

void Planet::render(){
    
    cout << "render..." << endl;
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glCallList(sphereDL2);
	
	
}





#include <iostream>
#include <vector>
#include <assert.h>

// must include windows.h before GL on win32
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "planet_gui.h"
#include <memory.h>
#include <math.h>
using namespace std;

float PlanetGui::_ident[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
PlanetGui *PlanetGui::s_theGUI = NULL;
const int PlanetGui::kMaxSubdivisionLevel = 6;

PlanetGui::PlanetGui( ) :
	m_subdLevel ( 3 ),
	m_drawMode( Planet::DrawMode_TERRAIN ),
	m_terrRandom( 0.17f ),
	m_terrWatery( 0.5f ),
	m_cursorHex( 0 ),
	m_mouseOnSurface( false ),
	m_paintTile( Cell::Terrain_DESERT ),
	m_beautyMode( 1 ),
	m_showStats( 1 ) {
	// singleton
	assert( s_theGUI == NULL );
	s_theGUI = this;

	// initialize planet rotation
	memcpy( m_rotPlanet, _ident, sizeof( _ident ) );
	memcpy( m_rotSun, _ident, sizeof( _ident ) );
	

	// Construct the planet
	m_planet  = new Planet( m_subdLevel );

	// Initialize OpenGL Resources
	m_planetDlist = glGenLists( 1 );
	
}

void PlanetGui::redraw() {
	if (m_beautyMode) {
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	}
	else {
		glClearColor( 0.2f, 0.2f, 0.6f, 1.0f );
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );

	
	
	// Add the sunlight before rotation :)
	if (!m_beautyMode) {
		glDisable( GL_LIGHTING );
	}
	else {
		glPushMatrix();
		// Rotate to the sun light's alignment		
		//glLoadIdentity();
		glMultMatrixf( m_rotSun );

		glDisable( GL_LIGHTING );				

		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
		glEnable( GL_LIGHT1 );
		glEnable( GL_LIGHT2 );

		glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
		glEnable( GL_COLOR_MATERIAL );		

		//printf( "sun: %f %f %f\n", 
					//m_rotSun[8], 
					//m_rotSun[9], 
					//m_rotSun[10] );
			
		GLfloat sun_intensity = 5.5f;
		GLfloat zero_amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };		
		GLfloat sun_color[] = { 0.961f,  0.882f, 0.699f, 1.0f };		
		GLfloat sun_amb[] = { 0.0f, 0.0f, 0.2f, 1.0f };		
		GLfloat sun_direction[] = { .97, 0.176, -0.1235, 0.0f };		

		sun_color[0] *= sun_intensity;
		sun_color[1] *= sun_intensity;
		sun_color[2] *= sun_intensity;

		// Rim  & fill don't track with sun rotation
		GLfloat fill_color[] = { 0.1f, 0.1f, 0.4f, 1.0f };
		GLfloat fill_dir[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		
		GLfloat rim_color[] = { 1.0f, 1.0f, 1.2f, 1.0f };
		GLfloat rim_dir[] = { -0.015461f, 0.64879f, -0.760808, 0.0f };
		
		// zero out other amb
		glLightfv(GL_LIGHT1, GL_AMBIENT,  zero_amb );
		glLightfv(GL_LIGHT2, GL_AMBIENT,  zero_amb );

		// Sun light
		glLightfv( GL_LIGHT0, GL_POSITION, sun_direction );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, sun_color );	
		glLightfv(GL_LIGHT0, GL_AMBIENT,  sun_amb );
		
		glPopMatrix();

		// Fill light
		glLightfv( GL_LIGHT1, GL_POSITION, fill_dir );
		glLightfv( GL_LIGHT1, GL_DIFFUSE, fill_color );		
	
		// Rim light
		glLightfv( GL_LIGHT2, GL_POSITION, rim_dir);
		glLightfv( GL_LIGHT2, GL_DIFFUSE, rim_color );	
	}
    
	// camera distance
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -2500.0f );				

	// draw the "atmosphere"	
	if (m_beautyMode) {
		glDepthMask( GL_FALSE );
		drawAtmosphere();		
	}

	// Rotate to the planet's alignment
	glMultMatrixf( m_rotPlanet );	

	if (m_beautyMode) {
		drawStarfield();
		glDepthMask( GL_TRUE );
	}

	//glPushMatrix();

	// grab the current MVP
	glGetDoublev( GL_MODELVIEW_MATRIX, m_modelView );
	glGetDoublev( GL_PROJECTION_MATRIX, m_projection );	
	glGetIntegerv( GL_VIEWPORT, m_viewport );	
		

	// Update the cursor, probably not the best place to
	// call this, but I don't really have a sim loop in
	// this crappy demo
	updateCursorPos();

	glColor3f( 1.0f, 1.0f, 1.0f );
	glLineWidth( 1.0f );

	// draw once with no lights
	glDisable( GL_LIGHTING );
		
	if (!m_beautyMode) {
		glColor3f( 1.0f, 1.0f, 1.0f );		
	}
	else {		
		glColor3f( 0.2f, 0.2f, 0.2f );
	}

	if (m_planetDirty) {		
		glNewList( m_planetDlist, GL_COMPILE_AND_EXECUTE );
		m_planet->draw( m_drawMode );	
		glEndList();

		m_planetDirty = false;
	}
	else {		
		glCallList( m_planetDlist );
	}

	if (m_beautyMode) {
		// GL regular lighting doesn't allow you to
		// get values brighter than the base texture,
		// overdraw with lighting instead to get more
		// range.
		glDepthMask( GL_FALSE );
		glEnable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		glColor4f( 0.5f, 0.5f, 0.5f, 0.2f );
		glEnable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE );		
		glutSolidSphere( 1020.0f, 30, 30 );
		

		// draw again at lower opacity to back off
		// the overblown lighting (stupid GL lighting)
		glDisable( GL_LIGHTING );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		
		glColor4f( 1.0f, 1.0f, 1.0f, 0.3f );
		glCallList( m_planetDlist );

		glDisable( GL_BLEND );
		glDepthMask( GL_TRUE );
		
	}

	// draw the "cursor" hex
	std::vector<size_t> nb;	
	if (m_mouseOnSurface) {
		std::vector<Vec3> cursorPoly;
		if (m_cursorHex < m_planet->m_cells.size() ) {
			m_planet->getPolygon( m_planet->m_cells[m_cursorHex], cursorPoly );

			glColor3f( 0.0f, 1.0f, 1.0f );
			glLineWidth( 3.0 );
			glDisable( GL_TEXTURE_2D );
			glBegin( GL_LINE_LOOP );
			for ( std::vector<Vec3>::iterator pi = cursorPoly.begin();
				  pi != cursorPoly.end(); pi++) {
				glVertex3f( (*pi).x, (*pi).y, (*pi).z );
				//printf("%f %f %f\n", (*pi).x, (*pi).y, (*pi).z );
			}
			glEnd();
			//printf("---\n" );
		}


		// DBG: Draw dots on neighbor hexes		
		m_planet->getNeighbors( m_cursorHex, nb );
#if 0
		glPointSize( 3.0f );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glBegin( GL_POINTS );
		for (std::vector<size_t>::iterator ni = nb.begin();
			 ni != nb.end(); ni++)
		{
			Vec3 v = m_planet->m_cells[*ni].m_vertPos;
			glVertex3f( v.x, v.y, v.z );
		}
		glEnd();
#endif
	}

	//glPopMatrix();

	// Draw status text
	glColor3f( 1.0f, 1.0f, 1.0f );
	if (m_showStats) {
		/*
		gfDrawStringFmt( "# of hexes: %d\n", m_planet->getNumHexes() );
		gfDrawStringFmt( "# of Triangles: %d\n", m_planet->m_hexdual.size() );
	    gfDrawStringFmt( "Curr Hex: %d\n", m_cursorHex );
		gfDrawStringFmt( "Curr Terrain: %d\n", 
		gfDrawStringFmt( "Num Neighbors: %d", nb.size() );
	    */
	}

	glDisable( GL_TEXTURE_2D );
}

void PlanetGui::drawAtmosphere() {	
	// disable the lighting for the atmosphere
	glDisable (GL_LIGHTING);
	glEnable( GL_BLEND );

	glBegin( GL_QUAD_STRIP );
	for ( float t = 0.0f; t <= 2.0f * PI + 0.001; t += PI / 10.0f ) {
		Vec3 p( cos(t), sin(t), 0.0f );
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glNormal3f( p[0], p[1], 0.0 );
		glVertex3f( p[0]*1000.0f, p[1]*1000.0f, 0.0f );

		glColor4f( 0.0f, 0.5f, 0.9f, 0.1f );
		glNormal3f( 0.0, 0.0, -1.0 );
		glVertex3f( p[0]*1300.0f, p[1]*1300.0f, 0.0f );
	}
	glEnd();

	glEnable (GL_LIGHTING);
}

// Draws a starfield
void PlanetGui::drawStarfield() {
	while (m_stars.size() < 200) {
		Vec3 spos;
		spos.x = ((float)rand() / (float)RAND_MAX)*2.0f - 1.0f;
		spos.y = ((float)rand() / (float)RAND_MAX)*2.0f - 1.0f;
		spos.z = ((float)rand() / (float)RAND_MAX)*2.0f - 1.0f;
			

		float d = spos.length2();
		
		// reject points outside of the unit sphere to
		// create an even distribution (no clumps at "corners")
		if (d > 1.0f) continue;

		Star s;
		spos.normalize();		
		s.p = spos * 3000.0f;
		s.radius = 500.0f + ((float)rand() / (float)RAND_MAX) * 500.0f;
		s.index = rand() % 8;
		m_stars.push_back( s );		
	}

	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, m_texStars );
	glColor3f( 0.5f, 0.5f, 0.5f );

	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );

	// draw the stars
	glDisable( GL_LIGHTING );
	glBegin( GL_QUADS );					
	
	// get billboard directions -- transpose of the planet rotation
	Vec3 right( m_rotPlanet[0], m_rotPlanet[4], m_rotPlanet[8] );
	Vec3 up( m_rotPlanet[1], m_rotPlanet[5], m_rotPlanet[9] );		

	float r;
	float sz = 1.0f / 8.0f;
	for (std::vector<Star>::iterator si = m_stars.begin();
		 si != m_stars.end(); si++) {
		Vec3 p;
		r = (*si).radius;

		p = (*si).p + right*r + up*r;
		glTexCoord2f( sz*((*si).index+1), 0.0f );
		glVertex3f( p.x, p.y, p.z );

		p = (*si).p + right*r - up*r;
		glTexCoord2f( sz*((*si).index+1), 1.0f );
		glVertex3f( p.x, p.y, p.z );

		p = (*si).p - right*r - up*r;
		glTexCoord2f( sz*(*si).index, 1.0f );
		glVertex3f( p.x, p.y, p.z );

		p = (*si).p - right*r + up*r;
		glTexCoord2f( sz*(*si).index, 0.0f );
		glVertex3f( p.x, p.y, p.z );
	}

	glEnd();
	glEnable( GL_LIGHTING );

	glDisable( GL_BLEND );
}

// Update the cursor pos
void PlanetGui::updateCursorPos() {
	// Make a ray from the mouse position
	Vec3 rayP, rayDir;

	// unproject at 2 depths
	GLdouble x, y, z;	

	// adjust the viewport for unProject to work	
	m_viewport[1] = 0;
	m_viewport[0] = 0;

	gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.001,
				  m_modelView, m_projection, m_viewport,
				  &x, &y, &z );
	rayP = Vec3( x, y, z );
	
	gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.01,
				  m_modelView, m_projection, m_viewport,
				  &x, &y, &z );
	rayDir = Vec3( x, y, z );
	
	// subtract them to get the ray
	rayDir = rayDir - rayP;
	rayDir.normalize();

	Vec3 p;
	if (m_planet->rayHitPlanet( rayP, rayDir, p )) {
		m_mouseSurfacePos = p;
		m_mouseOnSurface = true;
	}
	else {		
		m_mouseOnSurface = false;
	}			

	// Now, find the hex which contains the cursor
	if (m_mouseOnSurface) {
		m_cursorHex = m_planet->getHexIndexFromPoint( m_mouseSurfacePos );
	}
}


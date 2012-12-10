#include <cmath>
#include <iostream>

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "planet.h"

bool Planet::m_initStaticRes = false;
GLuint Planet::g_texTemplate;
GLuint Planet::g_texTileset;
GLuint Planet::g_texTilesetGrid;

float Planet::m_radius = 1.0f;

Cell::Cell( Vec3 p ) : m_vertPos( p ) {
	m_terrain = Cell::Terrain_DESERT;
	m_nrm = normalize(p);
}

Face::Face( size_t a, size_t b, size_t c) : m_cellA(a), m_cellB(b), m_cellC(c) {
	m_nbAB = NULL;
	m_nbBC = NULL;
	m_nbCA = NULL;
	
	// Mark newvert as uninitialized
	m_tmp.m_newvert = std::string::npos;
}

Vec3 Face::getCenter( const std::vector<Cell> &cells ) {
	Vec3 ret = cells[ m_cellA ].m_vertPos;
	ret += cells[ m_cellB ].m_vertPos;
	ret += cells[ m_cellC ].m_vertPos;
	ret /= 3.0f;
	return ret;
}

Planet::Planet( int subd_level ) : m_subdLevel(0) {
	// build initial (level 0) mesh
	buildLevel0();

	// subdivide until desired level
	while (m_subdLevel < subd_level) {
		subdivide();
	}

	// planetize if we're at level 0
	if (subd_level == 0) {
		projectToSphere();
	}
}

//=============================
// buildLevel0 -- builds the initial icosahedron
// for the planet mesh
//=============================
void Planet::buildLevel0()
{
	// hard code an icosahedron (20 sided die)
	m_cells.erase( m_cells.begin(), m_cells.end() );	
	m_cells.push_back( Cell( Vec3( 0.723606f, 0.0f, 1.17082f )));
	m_cells.push_back( Cell( Vec3( 0.0f, 1.17082f, 0.723606f )));
	m_cells.push_back( Cell( Vec3( -0.723606f, 0.0f, 1.17082f )));
	m_cells.push_back( Cell( Vec3( 0.0f, -1.17082f,  0.723606f )));
	m_cells.push_back( Cell( Vec3( 0.723606f, 0.0f, -1.17082f )));
	m_cells.push_back( Cell( Vec3( 0.0f, -1.17082f, -0.723606f )));
	m_cells.push_back( Cell( Vec3( -0.723606f, 0.0f, -1.17082f )));
	m_cells.push_back( Cell( Vec3( 0.0f, 1.17082f, -0.723606f )));
	m_cells.push_back( Cell( Vec3( 1.17082f, -0.723606f, 0.0f )));
	m_cells.push_back( Cell( Vec3( 1.17082f, 0.723606f, 0.0f )));
	m_cells.push_back( Cell( Vec3( -1.17082f, 0.723606f, 0.0f )));
	m_cells.push_back( Cell( Vec3( -1.17082f, -0.723606f,  0.0f )));

	m_hexdual.push_back( Face( 5, 11, 6 ));
	m_hexdual.push_back( Face( 1, 2, 0 ));
	m_hexdual.push_back( Face( 0, 2, 3 ));
	m_hexdual.push_back( Face( 5, 6, 4 ));
	m_hexdual.push_back( Face( 4, 6, 7 ));
	m_hexdual.push_back( Face( 9, 1, 0 ));
	m_hexdual.push_back( Face( 10, 2, 1 ));
	m_hexdual.push_back( Face( 2, 10, 11 ));
	m_hexdual.push_back( Face( 11, 3, 2 ));
	m_hexdual.push_back( Face( 8, 9, 0 ));
	m_hexdual.push_back( Face( 0, 3, 8 ));
	m_hexdual.push_back( Face( 11, 10, 6 ));
	m_hexdual.push_back( Face( 4, 7, 9 ));
	m_hexdual.push_back( Face( 9, 8, 4 ));
	m_hexdual.push_back( Face( 7, 6, 10 ));
	m_hexdual.push_back( Face( 1, 9, 7 ));
	m_hexdual.push_back( Face( 10, 1, 7 ));
	m_hexdual.push_back( Face( 5, 4, 8 ));
	m_hexdual.push_back( Face( 3, 11, 5 ));
	m_hexdual.push_back( Face( 8, 3, 5 ));	

	// make planet sized
	// projectToSphere();

	// assign neighbors
	findNeighbors();
}

// Used in subdivide
void createTrisFromEdge( std::vector< std::pair< size_t, size_t > > &edgeDone,
                         std::vector<Face> &trilist,
                         Face &tri, Face &otherTri,
                         size_t eA, size_t eB ) {
	std::pair<size_t, size_t> eid( std::min( eA, eB ), std::max( eA, eB ) );
	if (std::find( edgeDone.begin(), edgeDone.end(), eid ) == edgeDone.end() ) {
		trilist.push_back( Face( eA, tri.m_tmp.m_newvert, otherTri.m_tmp.m_newvert ) );					
		trilist.push_back( Face( tri.m_tmp.m_newvert, otherTri.m_tmp.m_newvert, eB ) );
		edgeDone.push_back( eid );
	}
}

//=============================
// subdivide()
// Perform sqrt(3) subdivision on
// the mesh
//=============================
void Planet::subdivide() {
	// Subdivide by creating two triangles in 
	// the next level mesh for every edge in the
	// src mesh. Keep track of which edges have
	// already been handled
	std::vector< std::pair< size_t, size_t > > edgeDone;

	// The new mesh that will be created	
	std::vector<Face> newHexdual;

	// Go through each triangle in the old mesh and create
	// a new vert at the center of each one
	for (std::vector<Face>::iterator ti = m_hexdual.begin();
		 ti != m_hexdual.end(); ti++ ) {
		// Create a new vert at the center of the triangle
		Vec3 pNewVert;
		(*ti).m_tmp.m_newvert = m_cells.size();
		pNewVert = (*ti).getCenter( m_cells );		

		// add it to the list of cells
		m_cells.push_back( Cell( pNewVert ) );
	}

	// Go through each triangle in the old mesh and create
	// a new pair of triangles for each edge
	for (std::vector<Face>::iterator ti = m_hexdual.begin();
		 ti != m_hexdual.end(); ti++ ) {
		Face &t = (*ti);

		// Create a pair for edge AB
		createTrisFromEdge( edgeDone, newHexdual, t, *(t.m_nbAB), t.m_cellA, t.m_cellB );

		// Create a pair for edge BC
		createTrisFromEdge( edgeDone, newHexdual, t, *(t.m_nbBC), t.m_cellB, t.m_cellC );
		
		// Create a pair for edge CA
		createTrisFromEdge( edgeDone, newHexdual, t, *(t.m_nbCA), t.m_cellC, t.m_cellA );
		
	}

	// replace the current set of cells with the dual
	m_hexdual = newHexdual;

	// find new neighbors
	findNeighbors();

	// reproject back to sphere
	projectToSphere();

	// note the subdivision
	m_subdLevel++;
}


//=============================
// findNeighbors() -- it would be more
// efficent to just keep track of the neighbors
// during subdivision, but this is easier
//=============================
bool edgeMatch( size_t a, size_t b,
				size_t otherA, size_t otherB, size_t otherC ) {
	if ( ((a==otherA) && (b==otherB)) ||
		 ((a==otherB) && (b==otherC)) ||
		 ((a==otherC) && (b==otherA)) ||
		 ((b==otherA) && (a==otherB)) ||
		 ((b==otherB) && (a==otherC)) ||
		 ((b==otherC) && (a==otherA)) ) return true;
	return false;
}

bool _cmpAngle( Face *a, Face *b ) {
	return a->m_tmp.m_angle < b->m_tmp.m_angle;
}

void Planet::findNeighbors() {
	// Clear the Face list on the cells
	for (std::vector<Cell>::iterator hi = m_cells.begin();
		hi != m_cells.end(); hi++ ) {
		(*hi).m_faces.erase( (*hi).m_faces.begin(), (*hi).m_faces.end() );
	}

	// Find edge adjacentcy -- slow and brute force. Should
	// do this as part of the subdivide step if this were a
	// non-prototype implementation.
	for (std::vector<Face>::iterator ti = m_hexdual.begin();
		ti != m_hexdual.end(); ti++) {
		// find the neighbors for ti
		for (std::vector<Face>::iterator tj = m_hexdual.begin();
			tj != m_hexdual.end(); tj++) {

			// Don't match ourselves
			if (ti==tj) continue;

			// Neighbor across edge AB
			if ( edgeMatch( (*ti).m_cellA, (*ti).m_cellB,
							(*tj).m_cellA, (*tj).m_cellB, (*tj).m_cellC ) ) {
				(*ti).m_nbAB = &(*tj);
			}

			// Neighbor across edge BC
			if ( edgeMatch( (*ti).m_cellB, (*ti).m_cellC,
							(*tj).m_cellA, (*tj).m_cellB, (*tj).m_cellC ) ) {
				(*ti).m_nbBC = &(*tj);
			}

			// Neighbor across edge CA
			if ( edgeMatch( (*ti).m_cellC, (*ti).m_cellA,
							(*tj).m_cellA, (*tj).m_cellB, (*tj).m_cellC ) ) {
				(*ti).m_nbCA = &(*tj);
			}
		}


		// Also as part of findNeighbors, set up the Face pointers
		m_cells[(*ti).m_cellA].m_faces.push_back( &(*ti) );
		m_cells[(*ti).m_cellB].m_faces.push_back( &(*ti) );
		m_cells[(*ti).m_cellC].m_faces.push_back( &(*ti) );
	}

	// Now sort the Face list on the cells by the angle
	// around the hex center
	for (std::vector<Cell>::iterator hi = m_cells.begin();
		hi != m_cells.end(); hi++ ) {
		// assign angles 
		for (std::vector<Face*>::iterator ti = (*hi).m_faces.begin();
			  ti != (*hi).m_faces.end(); ti++ ) {
			// arbitrarily use the first one as starting angle
			// it doesn't matter		
			Vec3 v1 = (*hi).m_faces.back()->getCenter( m_cells ) - (*hi).m_vertPos;			
			Vec3 nrm = (*ti)->getCenter( m_cells );
			Vec3 v2 = nrm - (*hi).m_vertPos;			
			nrm.normalize();
			v1.normalize();
			v2.normalize();

			float ang = acos( v1*v2 );
			float dir = nrm*( v1^v2 );
			if (dir < 0.0f) ang = M_PI + (M_PI - ang);

			(*ti)->m_tmp.m_angle = ang;
		}

		// Sort them
		std::sort( (*hi).m_faces.begin(), (*hi).m_faces.end(), _cmpAngle );
	}
	
}

//=============================
// projectToSphere()
//=============================
void Planet::projectToSphere() {
	for (std::vector<Cell>::iterator ti = m_cells.begin();
		 ti != m_cells.end(); ti++ ) {
		Vec3 p = (*ti).m_vertPos;
		p.normalize();
		p *= m_radius;
		(*ti).m_vertPos = p;
	}
}

//=============================
// draw()
//=============================
void Planet::draw( int draw_mode ) {
	//DBG: Draw axes
	glDisable(GL_LIGHTING);
	glBegin( GL_LINES );
	
	    glColor3f( 1.0f, 0.0f, 0.0f );
	    glVertex3f( 0.0f, 0.0f, 0.0f );
	    glVertex3f( 1.0f, 0.0f, 0.0f );
	
	    glColor3f( 0.0f, 1.0f, 0.0f );
	    glVertex3f( 0.0f, 0.0f, 0.0f );
	    glVertex3f( 0.0f, 1.0f, 0.0f );

	    glColor3f( 0.0f, 0.0f, 1.0f );
	    glVertex3f( 0.0f, 0.0f, 0.0f );
	    glVertex3f( 0.0f, 0.0f, 1.0f );

	glEnd();
	glEnable(GL_LIGHTING);
	
	
	
	
    /*	
	// Draw the cells
	//glColor3f( 1.0f, 1.0f, 1.0f );
	glEnable( GL_TEXTURE_2D );

	glBegin( GL_TRIANGLES );
	for ( std::vector<Face>::iterator hi = m_hexdual.begin();
		 hi != m_hexdual.end(); hi++ ) {
		Vec3 pA, pB, pC;
		Vec3 nA, nB, nC;
		pA = m_cells[ (*hi).m_cellA ].m_vertPos;
		pB = m_cells[ (*hi).m_cellB ].m_vertPos;
		pC = m_cells[ (*hi).m_cellC ].m_vertPos;	

		nA = m_cells[ (*hi).m_cellA ].m_nrm;
		nB = m_cells[ (*hi).m_cellB ].m_nrm;
		nC = m_cells[ (*hi).m_cellC ].m_nrm;	

		if (draw_mode == DrawMode_CONSTRUCTION) {
			// Template draw mode
			glTexCoord2f( 0.5f, 0.94999 );
			glNormal3f( nA.x, nA.y, nA.z );
			glVertex3f( pA.x, pA.y, pA.z );

			glTexCoord2f( 0.8897, 0.275 );
			glNormal3f( nB.x, nB.y, nB.z );
			glVertex3f( pB.x, pB.y, pB.z );

			glTexCoord2f( 0.1103f, 0.275 );
			glNormal3f( nC.x, nC.y, nC.z );
			glVertex3f( pC.x, pC.y, pC.z );		
		} 
		else { // terrain or terrain_Grid
		
			// Terrain draw mode
			Vec2 txA( 0.5f, 0.94999 ), 
				   txB( 0.8897, 0.275 ), 
				   txC( 0.1103f, 0.275 ),
				   txTileMin, txTileMax;

			// offset texture coords to fit tile
			int ndxA , ndxB, ndxC;
			ndxA = m_cells[ (*hi).m_cellA ].m_terrain;
			ndxB = m_cells[ (*hi).m_cellC ].m_terrain;
			ndxC = m_cells[ (*hi).m_cellB ].m_terrain;

			// TODO: Could use GL texture transform here
			int ndx = (ndxA*25)+(ndxB*5)+ndxC;
			int tile_y = ndx / 12;
			int tile_x = ndx % 12;		
		
			// tileset is 1020 pixels wide, 1020/1024 = 0.996...
			float sz = 0.99609375/12.0f; 

			txA.x = (txA.x * sz) + (tile_x*sz);
			txA.y = (txA.y * sz) + (tile_y*sz);

			txB.x = (txB.x * sz) + (tile_x*sz);
			txB.y = (txB.y * sz) + (tile_y*sz);

			txC.x = (txC.x * sz) + (tile_x*sz);
			txC.y = (txC.y * sz) + (tile_y*sz);

			glTexCoord2f( txA.x, txA.y );
			//glColor3f( 1.0f, 0.0f, 0.0f );
			glNormal3f( nA.x, nA.y, nA.z );
			glVertex3f( pA.x, pA.y, pA.z );

			glTexCoord2f( txB.x, txB.y  );
			//glColor3f( 0.0f, 1.0f, 0.0f );
			glNormal3f( nB.x, nB.y, nB.z );
			glVertex3f( pB.x, pB.y, pB.z );

			glTexCoord2f( txC.x, txC.y  );
			//glColor3f( 0.0f, 0.0f, 1.0f );
			glNormal3f( nC.x, nC.y, nC.z );
			glVertex3f( pC.x, pC.y, pC.z );		
		}
	}
	glEnd();	
    */

	// DBG: Draw the dual mesh
	glColor3f( 1.0f, 1.0f, 1.0f );
	//glLineWidth( 2.0f );	
	
	for ( std::vector<Face>::iterator hi = m_hexdual.begin();
          hi != m_hexdual.end(); hi++ ) {
		glBegin( GL_LINE_LOOP );
		Vec3 pA, pB, pC;
		pA = m_cells[ (*hi).m_cellA ].m_vertPos;
		pB = m_cells[ (*hi).m_cellB ].m_vertPos;
		pC = m_cells[ (*hi).m_cellC ].m_vertPos;
	    
	    Vec3 faceCenter = (pA + pB + pC) / 3.0f;
	    glNormal3d( faceCenter.x, faceCenter.y, faceCenter.z );
		glVertex3f( pA.x, pA.y, pA.z );
		glVertex3f( pB.x, pB.y, pB.z );
		glVertex3f( pC.x, pC.y, pC.z );		
		
		glEnd();
	}

}

size_t Planet::getNumCells() {
	return m_cells.size();
}

size_t Planet::getHexIndexFromPoint( Vec3 surfPos ) {
	size_t best_hex = 0;
	float best_dot;

	// normalize
	Vec3 p = surfPos;
	p.normalize();
	best_dot = acos( (m_cells[0].m_vertPos * p)  / m_radius );


	// clever cheat -- just use the dot product to find the 
	// smallest angle -- and thus the containing hex
	for (size_t ndx = 1; ndx < m_cells.size(); ndx++) {
		float d = acos( ( m_cells[ndx].m_vertPos * p ) / m_radius );
		if (d < best_dot) {
			best_hex = ndx;
			best_dot = d;
		}
	}

	return best_hex;
}

// returns the polygon representation of this
// hex. Usually 6-sided but could be a pentagon	
void Planet::getPolygon( Cell &tile, std::vector<Vec3> &poly, float offset ) {
	// clear list
	poly.erase( poly.begin(), poly.end() );

	// construct polygon
	for ( std::vector<Face*>::iterator ti = tile.m_faces.begin();
		  ti != tile.m_faces.end(); ti++ ) {
		Vec3 p = (*ti)->getCenter( m_cells );
		p.normalize();
		p *= m_radius + offset;
		poly.push_back( p );
	}
}

// returns the indices of the neighbors of this tile
// Usually 6, could be 5
void Planet::getNeighbors( size_t tileNdx, std::vector<size_t> &nbrs ) {
	// clear list
	nbrs.erase( nbrs.begin(), nbrs.end() );

	// find neighbors
	for ( std::vector<Face*>::iterator ti = m_cells[tileNdx].m_faces.begin();
		  ti != m_cells[tileNdx].m_faces.end(); ti++ ) {
		// Check all all the cells on neiboring 
		// Facees (except ourself), checking for dups

		// HEX A
		if ( ( ((*ti)->m_cellA) != tileNdx ) &&
			 (find( nbrs.begin(), nbrs.end(), ((*ti)->m_cellA) ) == nbrs.end() ) ) {
			nbrs.push_back( ((*ti)->m_cellA) );
		}

		// HEX B
		if ( ( ((*ti)->m_cellB) != tileNdx ) &&
			 (find( nbrs.begin(), nbrs.end(), ((*ti)->m_cellB) ) == nbrs.end() ) ) {
			nbrs.push_back( ((*ti)->m_cellB) );
		}
		
		// HEX C
		if ( ( ((*ti)->m_cellC) != tileNdx ) &&
			 (find( nbrs.begin(), nbrs.end(), ((*ti)->m_cellC) ) == nbrs.end() ) ) {
			nbrs.push_back( ((*ti)->m_cellC) );
		}
	}
}

// Returns a point on the planet's surface given a ray
bool Planet::rayHitPlanet( Vec3 p, Vec3 dir, Vec3 &result ) {
	float a,b,c,d;
	a = dir*dir;
	b = (2.0f*dir)*p;
	c = (p*p) - (m_radius*m_radius);
	d = b*b - 4.0f*a*c;
	if (d <=0 ) return false;
	result = p + ((-b - sqrt(d)) / 2.0f*a)*dir;
	return true;
}













#ifndef __PLANET_H__
#define __PLANET_H__

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>
#include "vec.h"
#include "imageloader.h"

#define PI 3.1415926535897932384626433832795

using namespace std;

struct Face;

struct Cell {
	Cell( Vec3 p );

	// the position of the vert in the tile,
	// and the center of the hex in space
	Vec3 m_vertPos;		
	Vec3 m_nrm;

	// Terrain Type
	enum {
		Terrain_WATER,
		Terrain_DESERT,
		Terrain_STEPPE,
		Terrain_FOREST,
		Terrain_MOUNTAIN,
		Terrain_SNOW,
		Terrain_MARSH,
		Terrain_ROCK,
		
		Owner_RED,
		Owner_GREEN,
		Owner_BLUE,
		Owner_WHITE,
		Owner_BLACK,
		Owner_CYAN,
		Owner_MAGENTA,
		Owner_YELLOW,
		Owner_GAIA,
		
		Radiation_SAFE,
		Radiation_LOW,
		Radiation_MODERATE,
		Radiation_HIGH,
		Radiation_DEADLY
	};

	int m_terrain;
	int m_owner;
	int m_raditaion;
	
	// Triangle that share this hex
	std::vector<Face*> m_faces;	
};

// A Face is an element of the dual
// of the (mostly) hex tiling
struct Face {
	Face( size_t a, size_t b, size_t c );

	// indices into the hex list
	size_t m_cellA, m_cellB, m_cellC;
	
	// Connectivity, links to neighbors
	// i.e. nbCA = neighbor across edge A<->C
	Face *m_nbAB, *m_nbBC, *m_nbCA;

	// Returns the center of the tri
	Vec3 getCenter( const std::vector<Cell> &cells  );

	// Temporaries that don't need to stick around	
	union {
		// Just used in the construction process
		size_t m_newvert;

		// angle around tile center
		float m_angle;
	} m_tmp;
	

	// Dbg color
	//Imath::C4f m_dbgColor;
};

class Planet {
    public:
	Planet( int subd_level );

	enum {
		DrawMode_MESH,
		DrawMode_TEXTURED
	};

	void draw( int draw_mode );

	size_t getNumCells();

	void subdivide();

	// returns the polygon representation of this
	// hex. Usually 6-sided but could be a pentagon	
	void getPolygon( Cell &tile, std::vector<Vec3> &poly, float offset=0.0f );

	// returns the indices of the neighbors of this tile
	// Usually 6, could be 5. These aren't in any order
	void getNeighbors( size_t tileNdx, std::vector<size_t> &nbrs );
	
	// Returns a point on the planet's surface given a ray
	bool rayHitPlanet( Vec3 p, Vec3 dir, Vec3 &result );

	// Get a hex index from a 3d point .. projects the point
	// down to the planet surface and returns the index of the
	// hex containing it
	size_t getHexIndexFromPoint( Vec3 surfPos );

    //protected:
	int m_subdLevel;

	// construction operations
	void buildLevel0();	
	void projectToSphere();
	void findNeighbors();
	int getRandomTerrain();

	// data
	std::vector<Cell> m_cells;
	std::vector<Face> m_hexdual;

	// static resources
	static bool m_initStaticRes;
	static GLuint g_texTemplate;
	static GLuint g_texTileset;
	static GLuint g_texTilesetGrid; // Tileset with outline

	static float m_radius;
};

#endif

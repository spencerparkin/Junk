// Polygon.h

#ifndef __Polygon_h__
#define __Polygon_h__

#include "Vector.h"
#include "Plane.h"
#include "List.h"
#include "MemMgr.h"

#define MAX_POLYGON_VERTICES		32

struct Polygon
{
	Polygon *next, *prev;
	List *list;
	Vector *vtx[MAX_POLYGON_VERTICES];	
	Vector *norm[MAX_POLYGON_VERTICES];
	Vector *tex_coord[MAX_POLYGON_VERTICES];
	int num_vertices;
	Plane plane;
	// put texture reference here.
};

struct PolygonSoup
{
	List pgon_list;				// The list of polygons in the soup.
	MemPool vtx_mp;				// Pool memory allocated on the free-store for soup vertices and extra vertices on splits.
};

void CalcPolygonPlane(Polygon *pgon);
int LinePolygonIntersection(Polygon *pgon, Vector *pt0, Vector *pt1, GLfloat *t);
int PolygonIsConvex(Polygon *pgon);
int SplitPolygon(Plane *plane, Polygon *pgon, Polygon *pgon_front, Polygon *pgon_back, MemPool *vtx_mp);
void LoadPolygonSoup(PolygonSoup *pgon_soup, char *filename);
void FreePolygonSoup(PolygonSoup *pgon_soup);
void RenderPolygonSoup(PolygonSoup *pgon_soup);
void RenderPolygon(Polygon *pgon, int flags);

// Polygon rendering flags.
#define PGON_DRAW_TEXTURE		1
#define PGON_DRAW_LIT			2
#define PGON_DRAW_SMOOTH		4

// One of these is returned by SplitPoygon().
#define PGON_IN_PLANE			0
#define PGON_ON_PLANE_FRONT		1
#define PGON_ON_PLANE_BACK		2
#define PGON_STRADDLES_PLANE	3

#endif //__Polygon_h__

// endof Polygon.h
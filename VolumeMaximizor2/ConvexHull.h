// ConvexHull.h

#ifndef __ConvexHull_h__
#define __ConvexHull_h__

#include <stdio.h>
#include "Vector.h"
#include "Plane.h"
#include "List.h"
#include "MemMgr.h"

struct Edge
{
	Edge *next, *prev;
	List *list;
	Vector *vtx[2];
};

#define MAX_POLYGON_VERTICES		32

struct Polygon
{
	Polygon *next, *prev;
	List *list;
	Vector *vtx[MAX_POLYGON_VERTICES];
	int num_vertices;
	Vector norm;		// for convenience.
};

struct CloudPoint
{
	CloudPoint *next, *prev;
	List *list;
	Vector *pt;
};

// these are maintained by the hull-finder algorithm.
extern List EdgeList;
extern List PolygonList;
extern List PointList;

// this is populated by the user.
#define MAX_POINTS		512
extern Vector pt_cloud[MAX_POINTS];
extern int num_cloud_pts;

// the edge matrix is used to keep
// track of existing edges and look
// them up faster.
extern Edge *EdgeMatrix[MAX_POINTS][MAX_POINTS];
void WipeEdgeMatrix(void);
void RegisterEdge(Edge *e);
Edge *LookUpEdge(Vector *vtx0, Vector *vtx1);

// algorithm function prototypes:
void Find3DConvexHull(void);
Polygon *Find2DConvexHull(Plane *plane);
void Reset3DConvexHullFinder(void);
int Iterate3DConvexHullFinder(void);
void AddPolygon(Polygon *polygon);
int PointsAreColinear(Vector *pt0, Vector *pt1, Vector *pt2);
double Calc3DConvexHullVolume(void);
double Calc3DConvexHullSurfaceArea(void);
void Print3DConvexHull(FILE *fp);

#endif //__ConvexHull_h__

// endof ConvexHull.h
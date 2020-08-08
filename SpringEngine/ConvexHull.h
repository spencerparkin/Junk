// ConvexHull.h

#ifndef __ConvexHull_h__
#define __ConvexHull_h__

#include "Vector.h"
#include "Plane.h"
#include "List.h"
#include "Polygon.h"
#include "MemMgr.h"

struct Edge
{
	Edge *next, *prev;
	List *list;
	Vector *vtx[2];
};

struct CloudPoint
{
	CloudPoint *next, *prev;
	List *list;
	Vector *pt;
};

// these are maintained by the hull-finder algorithm.
extern List EdgeList;
extern List FinalEdgeList;
extern List PolygonList;
extern List PointList;

// this is populated by the user.
extern Vector *pt_cloud;		// Pointer to user supplied cloud-point buffer.  (Must be contiguous.)
extern int num_cloud_pts;		// Number of points in the buffer.

// the edge matrix is used to keep
// track of existing edges and look
// them up faster.
extern Edge ***EdgeMatrix;
void AllocEdgeMatrix(void);
void DeallocEdgeMatrix(void);
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
//int PointsAreCoplanar(Vector *pt0, Vector *pt1, Vector *pt2, Vector *pt3);
double Calc3DConvexHullVolume(void);
double Calc3DConvexHullSurfaceArea(void);

#endif //__ConvexHull_h__

// endof ConvexHull.h
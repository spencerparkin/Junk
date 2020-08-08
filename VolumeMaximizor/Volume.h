// Volume.h

// This software was written by Spencer T. Parkin, Copyright (c) 2007, All Rights Reserved.

#ifndef __Volume_h__
#define __Volume_h__

#include "Vector.h"
#include "Plane.h"
#include "List.h"

struct CandidatePoint
{
	CandidatePoint *next, *prev;
	List *list;
	Vector *pt;
	int flag;
};

struct Triangle
{
	Triangle *next, *prev;
	List *list;
	Vector *vtx[3];
	// it would be helpful to compute and store the
	// plane associated with each triangle.
};

struct Tetrahedron
{
	Tetrahedron *next, *prev;
	List *list;
	Triangle *tri[4];
	Vector *v[4];
	double six_vol;
};

// we can handle up to this many cloud-points.
#define MAX_CLOUD_POINTS	4096

// all bits that apply are set by the
// TetSideOfPlane() routine.
#define TET_PT_ON_PLANE_FRONT		1
#define TET_PT_ON_PLANE_BACK		2
#define TET_PT_ON_PLANE				4

// Algorithm control function prototypes:
void ResetVolumeAlgorithm(Vector *PointCloud, int NumCloudPoints);
int PerformVolumeAlgorithmIteration(void);
void AddTetrahedron(Tetrahedron *tet);

Triangle *FindTriangle(Vector *v0, Vector *v1, Vector *v2);
void RegisterTriangle(Triangle *tri);

extern struct List TriList, TetList;
extern struct List TriMeshList;
extern struct List CandPtList;

double CalcVolume(Vector *PointCloud, int NumCloudPoints);
double CalcVolume(void);
double CalcVolumeFromTriMeshList(void);
double CalcVolumeFromTetList(void);
double CalcSixTetrahedronVol(Tetrahedron *tet);

int TetSideOfPlane(Plane *plane, Tetrahedron *tet);
void MakePlaneFromTriangle(Plane *plane, Triangle *tri);
int DoTetrahedronsIntersect(Tetrahedron *tet0, Tetrahedron *tet1);
int DoesLineSegIntersectTriangle(Triangle *tri, Plane *plane, Vector *pt0, Vector *pt1);
double AreaOfTriangle(Vector *v0, Vector *v1, Vector *v2);
int ArePointsColinear(Vector *pt0, Vector *pt1, Vector *pt2);
int ArePointsCoplanar(Vector *pt0, Vector *pt1, Vector *pt2, Vector *pt3);

#endif //__Volume_h__

// endof Volume.h
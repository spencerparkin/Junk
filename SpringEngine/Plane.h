// Plane.h

#ifndef __Plane_h__
#define __Plane_h__

#include "Vector.h"

#define ON_PLANE_FRONT		0
#define ON_PLANE_BACK		1
#define ON_PLANE			2

struct Plane
{	
	Vector norm;
	double D;

	// Though this is not needed to uniquely
	// represent the plane, it is helpful to
	// keep a point that is on the plane.
	// This point is also considered to be
	// the center point of the plane.
	Vector pt;
};

void CopyPlane(Plane *dst, Plane *src);
void ReversePlane(Plane *dst, Plane *src);
void MakePlane(Plane *plane, Vector *pt, Vector *vec);
void MakePlane(Plane *plane, Vector *v0, Vector *v1, Vector *v2);
int SideOfPlane(Plane *plane, Vector *pt);
double LinePlaneIntersection(Plane *plane, Vector *v0, Vector *v1);
double DistanceToPlane(Plane *plane, Vector *pt);
void RotatePlane(Plane *plane, Vector *axis, double angle);
void PushToPlaneSurface(Plane *plane, Vector *v, Vector *v0);

#endif //__Plane_h__

// endof Plane.h
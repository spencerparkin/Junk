// Plane.cpp

#include "Plane.h"
#include <math.h>
#include <assert.h>

void CopyPlane(Plane *dst, Plane *src)
{
	VectorCopy(&dst->norm, &src->norm);
	VectorCopy(&dst->pt, &src->pt);
	dst->D = src->D;
}

// Swap the front/back faces of the given plane.
void ReversePlane(Plane *dst, Plane *src)
{
	VectorScale(&dst->norm, &src->norm, -1.f);
	VectorCopy(&dst->pt, &src->pt);
	dst->D = -src->D;
}

// Setup a plane from a point and a direction.
void MakePlane(Plane *plane, Vector *pt, Vector *vec)
{
	if(vec != &plane->norm)
		VectorCopy(&plane->norm, vec);
	VectorNormalize(&plane->norm, &plane->norm);	// For convenience.
	plane->D = VectorDot(pt, &plane->norm);
	VectorCopy(&plane->pt, pt);						// For convenience.
}

// Setup a plane from the given triangle with
// vertices given in CCW order.  The front of
// the plane will be the CCW orientation.  The
// center of the plane will be the first vertex given.
void MakePlane(Plane *plane, Vector *v0, Vector *v1, Vector *v2)
{
	Vector e01, e02;
	VectorSub(&e01, v1, v0);
	VectorSub(&e02, v2, v0);
	VectorCross(&plane->norm, &e01, &e02);
	VectorNormalize(&plane->norm, &plane->norm);	// For convenience.
	plane->D = VectorDot(v0, &plane->norm);
	VectorCopy(&plane->pt, v0);						// For convenience.
}

// Determine the side of the given plane that
// the given point lies on.
int SideOfPlane(Plane *plane, Vector *pt)
{
	double D = VectorDot(pt, &plane->norm);
	double delta = D - plane->D;	
	if(delta > EPS)
		return ON_PLANE_FRONT;
	if(delta < -EPS)
		return ON_PLANE_BACK;
	return ON_PLANE;
}

// Determine the point of intersection between the
// line determined by the given line-segment and
// the given plane.  Return this information in the
// form of the percentage by which one would linearly
// interpolate from the first point of the line-segment
// to the second.  If the return value is not in the
// range [0,1], then the line-segment does not inter-
// sect the plane.
double LinePlaneIntersection(Plane *plane, Vector *v0, Vector *v1)
{
	double t;
	double dot0, dot1;
	dot0 = VectorDot(&plane->norm, v0);
	dot1 = VectorDot(&plane->norm, v1);
	if(dot0 == dot1)	// In this case we weren't given a line-segment, but a point.
		return 0.f;
	t = (plane->D - dot0) / (dot1 - dot0);
	return t;
}

// Compute and return the shortest distance
// the given point is to the given plane.
double DistanceToPlane(Plane *plane, Vector *pt)
{
	// Another method is to project the
	// vector onto the norm of the plane.
	// That might be faster.
	Vector vec;
	VectorSub(&vec, pt, &plane->pt);
	double len = VectorLength(&vec);
	if(len < EPS)
		return 0.f;
	double angle = VectorAngle(&vec, &plane->norm);
	return len * fabs(cos(angle));
}

// Rotate the given plane about its center point.
void RotatePlane(Plane *plane, Vector *axis, double angle)
{
	VectorRotate(&plane->norm, &plane->norm, axis, angle);
	MakePlane(plane, &plane->pt, &plane->norm);
}

void PushToPlaneSurface(Plane *plane, Vector *v, Vector *v0)
{
	Vector vec;
	GLfloat dist = DistanceToPlane(plane, v0);
	VectorScale(&vec, &plane->norm, dist);
	VectorAdd(v, v0, &vec);
}

// endof Plane.cpp
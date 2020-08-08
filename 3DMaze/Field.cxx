// Field.cxx - By Spencer T. Parkin

#include <GL/glut.h>
#include <math.h>
#include "vec3d.h"
#include "Field.h"

#define PI	3.1415926536

// Construct a field of vision with the given vertices
// forming the edges of each perifery.
void MakeField(Field *f, vec3d *eye, vec3d *v0, vec3d *v1)
{
	double a0, a1;

	vec3d ray0, ray1;
	vec3d x_axis = {1.f, 0.f, 0.f};

	vec3d_sub(&ray0, v0, eye);
	vec3d_sub(&ray1, v1, eye);

	a0 = vec3d_angle(&ray0, &x_axis);
	a1 = vec3d_angle(&ray1, &x_axis);

	// Both angles should be somewhere between 0 and pi.
	// We want the angles to stay relative and be somewhere
	// between 0 and 2pi.  One of the two angles may need
	// to go below 0 or above 2pi to keep them relative to
	// one another.

	if(ray0.y < 0) a0 = 2.f * PI - a0;
	if(ray1.y < 0) a1 = 2.f * PI - a1;

	if(a0 < PI / 2.f && a1 > 3.f * PI / 2.f)
		a1 = a0 - vec3d_angle(&ray0, &ray1);

	if(a1 < PI / 2.f && a0 > 3.f * PI / 2.f)
		a0 = a1 - vec3d_angle(&ray0, &ray1);

	f->la = MIN(a0, a1);
	f->ga = MAX(a0, a1);
}

// Construct a field of vision from the given angles
void MakeField(Field *f, GLfloat look_angle, GLfloat fovi_angle)
{
	f->la = look_angle - fovi_angle / 2.f;
	f->ga = look_angle + fovi_angle / 2.f;
}

// Tell us if the given fields overlap one another in any way.
int FieldOverlap(Field *f0, Field *f1)
{
	Field a, b;

	a = *f0;
	b = *f1;

	if(a.la >= b.la && a.la <= b.ga)
		return 1;

	if(a.ga >= b.la && a.ga <= b.ga)
		return 1;

	if(b.la >= a.la && b.la <= a.ga)
		return 1;

	if(b.ga >= a.la && b.ga <= a.ga)
		return 1;

	if(a.la < b.la)
	{
		a.la += 2.f * PI;
		a.ga += 2.f * PI;
	}
	else
	{
		b.la += 2.f * PI;
		b.ga += 2.f * PI;
	}

	if(a.la >= b.la && a.la <= b.ga)
		return 1;

	if(a.ga >= b.la && a.ga <= b.ga)
		return 1;

	if(b.la >= a.la && b.la <= a.ga)
		return 1;

	if(b.ga >= a.la && b.ga <= a.ga)
		return 1;

	return 0;
}

// Compute the intersecting field of the two given fields if any.
// We assume that the given fields do overlap one another.
void FieldIntersection(Field *fdest, Field *fsrc0, Field *fsrc1)
{
	fdest->la = MAX(fsrc0->la, fsrc1->la);
	fdest->ga = MIN(fsrc0->ga, fsrc1->ga);
}

// endof Field.cxx
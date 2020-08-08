// fovi.cc -- 2D field of vision routines.

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "line.h"
#include "macros.h"
#include "fovi.h"

// Draw the given fovi.  This is
// a routine for debugging perposes.
void draw_fovi(fovi *f, float radius, float z)
{
	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(f->x, f->y, z);

	int k;
	float angle;
	float t;
	const int num_tris = 8;
	for(k = 0; k < num_tris; k++)
	{
		t = float(k) / float(num_tris - 1);
		angle = f->heading - f->angle + t * 2.f * f->angle;
		glVertex3f(f->x + radius * cos(angle), f->y + radius * sin(angle), z);
	}

	glEnd();
}

// Return the angle determined by measuring
// from the positive x-axis to the line
// through the origin and the given point.
// The angle will be in [0,2pi).
float quadrant_angle(float x, float y)
{
	// I just discovered the atan2 function!  Yay!
	// But I want mine to be in [0,2pi).
	return MOD_2PI(atan2(y, x));
}

// Return 1 if the given point is in
// the given field of vision.
// Return 0 otherwise.
int is_point_in_fovi(fovi *f, float x, float y)
{
	x -= f->x;
	y -= f->y;

	float angle = quadrant_angle(x, y);
	
	if(fabs(angle - f->heading) <= f->angle)
		return 1;

	if(f->heading + f->angle > 2.f * PI &&
		fabs(angle - (f->heading - 2.f * PI)) < f->angle)
		return 1;

	if(f->heading - f->angle < 0.f &&
		fabs(angle - (f->heading + 2.f * PI)) < f->angle)
		return 1;

	return 0;
}

// Return 1 if the given point is on the
// edge of the given fovi.  Otherwise, return 0.
int is_point_on_fovi_edge(fovi *f, float x, float y)
{
	float angle0 = quadrant_angle(x - f->x, y - f->y);
	float angle1 = f->heading + f->angle;

	make_within_pi(&angle0, &angle1);

	if(fabs(angle1 - angle0) < EPSILON)
		return 1;

	angle1 = f->heading - f->angle;
	make_within_pi(&angle0, &angle1);

	if(fabs(angle1 - angle0) < EPSILON + 0.001f)
		return 1;

	return 0;
}

// Return 1 if the given line is in
// the given field of vision.
// Return 0 otherwise.
int is_line_in_fovi(fovi *f, line *l)
{
	if(is_point_in_fovi(f, l->x0, l->y0) ||
		is_point_in_fovi(f, l->x1, l->y1))
		return 1;

	const float radius = 100.f;		// Just more than we'd ever need.
	line v;
	v.x0 = f->x;
	v.y0 = f->y;
	v.x1 = f->x + radius * cos(f->heading);
	v.y1 = f->y + radius * sin(f->heading);

	return lines_intersect(l, &v);
}

// Return 1 if the given circle is in
// the given field of vision.
// Return 0 otherwise.
int is_circle_in_fovi(fovi *f, float x, float y, float radius)
{
	line l;
	float vec_x = x - f->x;
	float vec_y = y - f->y;
	float angle = quadrant_angle(vec_x, vec_y) - PI / 2.f;
	l.x0 = x + radius * cos(angle);
	l.y0 = y + radius * sin(angle);
	l.x1 = x - radius * cos(angle);
	l.y1 = y - radius * sin(angle);
	return is_line_in_fovi(f, &l);
}

// A convenient routine for changing
// the heading of a fovi.
void rotate_fovi(fovi *f, float delta_heading)
{
	f->heading += delta_heading;
	f->heading = MOD_2PI(f->heading);
}

// Remove from the given fovi the portions
// of it that do not intersect the given
// clipper fovi.  The eye-points
// of both fovis are not referenced and
// are assumed to be at the origin.
// Bogus stuff may happen if the field
// of vision for any given fovi is
// not in the range of [0,pi/2].  (fovi.angle, I mean.)
// Return 1 if the fovi in f has
// a non-zero degree field of vision.
// Return 0 otherwise.
int clip_fovi(fovi *f, fovi *clip)
{
	make_within_pi(&f->heading, &clip->heading);

	float a_min, a_max;

//	if(f->heading < clip->heading)
//	{
		a_min = MAX(clip->heading - clip->angle, f->heading - f->angle);
		a_max = MIN(f->heading + f->angle, clip->heading + clip->angle);
//	}
//	else
//	{
//		a_min = MAX(f->heading - f->angle, clip->heading - clip->angle);
//		a_max = MIN(clip->heading + clip->angle, f->heading + f->angle);
//	}

//	clip->heading = MOD_2PI(clip->heading);

	if(a_min >= a_max)
		return 0;

	f->heading = MOD_2PI((a_min + a_max) * 0.5f);
	f->angle = (a_max - a_min) * 0.5f;
	
	// See if this helps to keep lines out of
	// the fovi when only one end-point of the
	// line is touching the edge of the fovi.
//	f->angle -= EPSILON;

	return 1;
}

// Create a fovi that views the
// given line from the given eye-point.
void make_fovi(fovi *f, line *l, float x, float y)
{
	f->x = x;
	f->y = y;

	float a0 = quadrant_angle(l->x0 - x, l->y0 - y);
	float a1 = quadrant_angle(l->x1 - x, l->y1 - y);

	make_within_pi(&a0, &a1);

	f->heading = MOD_2PI((a0 + a1) * 0.5f);
	f->angle = ABS(a0 - a1) * 0.5f;
}

// Any two angles (working in mod 2pi)
// are within at most pi of one another.
// Make this the case even if we were
// not working in mod 2pi.
void make_within_pi(float *a0, float *a1)
{
	if(*a0 < *a1)
		while(*a1 - *a0 > PI)
			*a0 += 2.f * PI;
	else
		while(*a0 - *a1 > PI)
			*a1 += 2.f * PI;
}

// Cut off the portions of the given line segment
// that do not fall within the given field of vision.
// This may change the order of the end-points of
// the line-segment.
void clip_line(fovi *f, line *l, float *t0, float *t1)
{
	float angle0 = quadrant_angle(l->x0 - f->x, l->y0 - f->y);
	float angle1 = quadrant_angle(l->x1 - f->x, l->y1 - f->y);

	make_within_pi(&angle0, &f->heading);
	make_within_pi(&angle1, &f->heading);

	angle0 = f->heading - angle0;
	angle1 = f->heading - angle1;
		
	if(angle0 < angle1)
	{
		SWAP(float, l->x0, l->x1);
		SWAP(float, l->y0, l->y1);
	}

	line l_copy = *l;

	if(intersection_point(&l_copy, f->x, f->y, f->heading - f->angle, t0))
	{
		if(0.f < *t0 && *t0 < 1.f)
			line_point(&l_copy, *t0, &l->x0, &l->y0);
		else
			*t0 = 0.f;
	}

	if(intersection_point(&l_copy, f->x, f->y, f->heading + f->angle, t1))
	{
		if(0.f < *t1 && *t1 < 1.f)
			line_point(&l_copy, *t1, &l->x1, &l->y1);
		else
			*t1 = 1.f;
	}
}

// Indicate, approximately, which quadrant
// the given angle is in.
int which_quadrant(float angle)
{
	angle = MOD_2PI(angle);
	if(0.f <= angle && angle < PI / 2.f)
		return 1;
	if(PI / 2.f <= angle && angle < PI)
		return 2;
	if(PI <= angle && angle < 3.f * PI / 2.f)
		return 3;
	return 4;
}

// endof fovi.cc

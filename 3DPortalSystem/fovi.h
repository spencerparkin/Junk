// fovi.h -- 2D field of vision routines

// Programmed by Spencer T. Parkin

#ifndef __fovi_h__
#define __fovi_h__

#include "line.h"

// A field of vision:
struct fovi
{
	float x, y;			// Should always be global coordinates.
	float heading;		// Should be in [0,2pi)
	float angle;		// Should be in [0,pi/2]
};

// Prototypes:
void draw_fovi(fovi *f, float radius, float z = -0.1f);
int is_point_in_fovi(fovi *f, float x, float y);
int is_point_on_fovi_edge(fovi *f, float x, float y);
int is_line_in_fovi(fovi *f, line *l);
int is_circle_in_fovi(fovi *f, float x, float y, float radius);
void rotate_fovi(fovi *f, float delta_heading);
int clip_fovi(fovi *f, fovi *clip);
void make_fovi(fovi *f, line *l, float x, float y);
void make_within_pi(float *a0, float *a1);
float quadrant_angle(float x, float y);
void clip_line(fovi *f, line *l, float *t0, float *t1);
int which_quadrant(float angle);

#endif __fovi_h__

// endof fovi.h

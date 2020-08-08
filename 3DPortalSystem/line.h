// line.h -- 2D line routines

// Programmed by Spencer T. Parkin

#ifndef __line_h__
#define __line_h__

// In addition to the end-points of a
// line-segment, normal vector and
// normal angle data is included.
struct line
{
	float x0, y0;
	float x1, y1;
	float norm_x;
	float norm_y;
	float angle;
};

// Line function prototypes:
void draw_line(line *l);
void calc_line_norm(line *l, float x, float y);
int lines_intersect(line *l0, line *l1);
int point_line_side(line *l, float x, float y);
float distance_to_line(line *l, float x, float y);
float distance_to_line_midpt(line *l, float x, float y);
float line_sqr_dist(line *l);
int line_circle_intersect(line *l, float x, float y, float radius, int *num_pts, float *t0, float *t1);
void line_circle_adjust(line *l, float *x, float *y, float radius, int num_pts);
int is_point_on_line(line *l, float x, float y, float epsilon);
bool intersection_point(line *l, float x, float y, float angle, float *t);
void line_point(line *l, float t, float *x, float *y);

// This constant is used in the line_circle_adjust() routine
// so that the result produced is a non-intersecting case.
// If the fudge was zero, then the result produced would be
// a single point of intersection between the cirle and the line.
#define FUDGE		0.0005f

#endif __line_h__

// endof line.h

// line.cc

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "line.h"
#include "macros.h"
#include "fovi.h"

// Here we assume that we're
// inside a GL_LINES render block.
// This is a routine for
// debugging perposes.
void draw_line(line *l)
{
	glVertex2f(l->x0, l->y0);
	glVertex2f(l->x1, l->y1);

	// Draw another line to
	// indicate the orientation
	// of the line segment.
	float x = (l->x0 + l->x1) * 0.5f;
	float y = (l->y0 + l->y1) * 0.5f;
	glVertex2f(x, y);
	glVertex2f(x + l->norm_x, y + l->norm_y);
}

// Calculate the orientation of
// the given line segment such
// that it faces the given point.
void calc_line_norm(line *l, float x, float y)
{
	float vec_x, vec_y;

	vec_x = l->x1 - l->x0;
	vec_y = l->y1 - l->y0;
	
	if(l->x0 > l->x1 || (l->x0 == l->x1 && l->y0 < l->y1))
	{
		vec_x *= -1.f;
		vec_y *= -1.f;
	}

	l->angle = atan2(vec_y, vec_x);

	switch(point_line_side(l, x, y))
	{
		case 0:		// Below the line.
		{
			l->angle -= PI / 2.f;
			break;
		}
		case 1:		// Above the line.
		{
			l->angle += PI / 2.f;
			break;
		}
		default:
		{
			break;
		}
	}

	l->norm_x = cos(l->angle);
	l->norm_y = sin(l->angle);
}

// Return 1 if the given lines
// intersect.  Return 0 otherwise.
int lines_intersect(line *l0, line *l1)
{
	if(point_line_side(l0, l1->x0, l1->y0) == point_line_side(l0, l1->x1, l1->y1))
		return 0;

	if(point_line_side(l1, l0->x0, l0->y0) == point_line_side(l1, l0->x1, l0->y1))
		return 0;

	return 1;
}

// Return 1 if the given point is above
// the given line, 0 if below.
// Return -1 if I have no idea.
// If the line is vertical, then return 1
// if the given point is to the right of
// the given line, 0 if to the left.
// If the line is horizontal, above/below
// as stated earlier..
int point_line_side(line *l, float x, float y)
{
	if(l->x0 == l->x1)
	{
		if(l->y0 == l->y1)
			return -1;

		return(x > l->x0);
	}
	else if(l->y0 == l->y1)
	{
		return(y > l->y0);
	}

	float m = (l->y1 - l->y0) / (l->x1 - l->x0);

	return(y > m * (x - l->x0) + l->y0);
}

// Return the shortest distance the given point
// is to the given line segment.  Part of the
// math done in this routine is just a simple
// vector projection.
float distance_to_line(line *l, float x, float y)
{
	float proj_x, proj_y;
	float vec_x, vec_y;

	vec_x = l->x1 - l->x0;
	vec_y = l->y1 - l->y0;

	x -= l->x0;
	y -= l->y0;

	float scale = (x * vec_x + y * vec_y) / (vec_x * vec_x + vec_y * vec_y);

	proj_x = vec_x * scale;
	proj_y = vec_y * scale;

	x -= proj_x;
	y -= proj_y;

	return sqrt(x * x + y * y);
}

// Return the distance between the given point
// and the mid-point of the given line-segment.
float distance_to_line_midpt(line *l, float x, float y)
{
	float mx, my;
	mx = (l->x0 + l->x1) / 2.f;
	my = (l->y0 + l->y1) / 2.f;
	float dx, dy;
	dx = x - mx;
	dy = y - my;
	return sqrt(dx * dx + dy * dy);
}

// Return the square length of
// the given line segment.  The
// caller can take the square
// root if they want to.
float line_sqr_dist(line *l)
{
	float x = l->x1 - l->x0;
	float y = l->y1 - l->y0;

	return x * x + y * y;
}

// Return 1 if the given circle intersects
// the given line segment.  Return 0 otherwise.
// The math for this routine,
// (as for the others above), is easily
// worked out on a peice of scratch paper.
int line_circle_intersect(line *l, float x, float y, float radius, int *num_pts, float *t0, float *t1)
{
	line L;

	// Translate the circle to the origin and
	// the given line segment along with it.
	L.x0 = l->x0 - x;
	L.y0 = l->y0 - y;
	L.x1 = l->x1 - x;
	L.y1 = l->y1 - y;

	// A fast test at this point would be to find
	// out if one point of the line-segment is in
	// the circle and the other not.  This test,
	// however, does not cover all cases of intersection,
	// while the following test does.  So I will
	// just go ahead with the following test.

	// Calculate the vector for the line segment
	// that can be used in its parametric equation.
	float vec_x = L.x1 - L.x0;
	float vec_y = L.y1 - L.y0;

	// Calculate the determinant of the quadratic
	// formula giving us the points of intersection
	// of the circle and the infinite line determined
	// by the given line segment.
	float dot = L.x0 * vec_x + L.y0 * vec_y;
	float vec_mag2 = vec_x * vec_x + vec_y * vec_y;
	float pt_mag2 = L.x0 * L.x0 + L.y0 * L.y0;
	float D = dot * dot - vec_mag2 * (pt_mag2 - radius * radius);

	// Provide memory for this variable if the caller
	// is not interested in this information.
	static int k;
	if(!num_pts)
		num_pts = &k;

	// The determinant tells us if there are zero,
	// one, or two points of intersection.
	*num_pts = D < 0.f ? 0 : (D > 0.f ? 2 : 1);

	// Notice that this indicates the number of
	// intersections between the circle and the
	// infinite line determined by the line segment.
	// What we want is the number of intersections
	// between the circle and the line segment.
	// Calculate the parameters giving us the
	// points of intersection, if any.  Then determine
	// how many intersection points we really have.
	if(*num_pts > 0)
	{
		static float _t0, _t1;
		if(!t0)
			t0 = &_t0;
		if(!t1)
			t1 = &_t1;

		// The tangent case...
		if(*num_pts == 1)
		{
			*t0 = -dot / vec_mag2;
			*num_pts = (*t0 >= 0.f && *t0 <= 1.f);
		}
		else	// The secant case...
		{
			*t0 = (-dot - sqrt(D)) / vec_mag2;
			*t1 = (-dot + sqrt(D)) / vec_mag2;
			*num_pts = (*t0 >= 0.f && *t0 <= 1.f) +
						(*t1 >= 0.f && *t1 <= 1.f);			
		}
	}

	return(*num_pts > 0);
}

// Assuming that an intersection exists between the given circle
// and the given line-segment, adjust the position of the circle
// away from the line-segment so that there is possibly no longer
// any intersection.  In the two-point intersection case, this is
// garenteed.  In the one-point intersection case, the function
// garentees that in the result there is either no intersection,
// or a two-point intersection, in which case it may be tested for,
// and this routine called again if nessisary.
void line_circle_adjust(line *l, float *x, float *y, float radius, int num_pts)
{
	switch(num_pts)
	{
		case 0:
		{
			// Nothing to do here.
			break;
		}
		case 1:
		{
			// In this case, one of the end-points of
			// the given line-segment must reside inside
			// the given circle.  Determine which point
			// this is and move the circle away from the
			// line along the vector determined by this
			// end-point and the circle's center.
			float *ex, *ey;
			float dx = l->x0 - *x;
			float dy = l->y0 - *y;
			if(dx * dx + dy * dy < radius * radius)
			{
				ex = &l->x0;
				ey = &l->y0;
			}
			else
			{
				ex = &l->x1;
				ey = &l->y1;
			}

			// Find the vector we want to move circle along.
			float vec_x, vec_y;
			vec_x = *x - *ex;
			vec_y = *y - *ey;

			// Resize the vector to the appropriate length.
			float len = (radius + FUDGE) / sqrt(vec_x * vec_x + vec_y * vec_y);
			vec_x *= len;
			vec_y *= len;

			// Finally, move the circle away from the line-segment.
			*x = *ex + vec_x;
			*y = *ey + vec_y;

			// Notice that at this point, the circle may
			// intersect the line at two points now, instead
			// of at one point.  This needs to be checked by
			// the caller, and if detected, this routine
			// should be called again to handle that condition.
			break;
		}
		case 2:
		{
			// The line-segments normal vector determines
			// the front side of the line.  Here we assume
			// that the circle is on the front side.
			float dist = distance_to_line(l, *x, *y);
			float ds = radius - dist + FUDGE;		// Should be a positive number.
			*x += ds * cos(l->angle);
			*y += ds * sin(l->angle);
			break;
		}
	}
}

// Return 1 if the given point is approximately
// on the given line.  If the given epsilon is
// zero, then return 1 if the given point is
// exactly on the given line.  0 is returned
// if the point is not, or is approximately not
// on the line.
int is_point_on_line(line *l, float x, float y, float epsilon)
{
	if(l->x0 == l->x1)
	{
		if(x != l->x0)
			return 0;
		float y_min = MIN(l->y0, l->y1);
		float y_max = MAX(l->y0, l->y1);
		return(-epsilon + y_min <= y && y <= y_max + epsilon);
	}

	// Notice that the epsilon here does not
	// nessisarily measure a distance orthogonal
	// to the given line.
	float m = (l->y1 - l->y0) / (l->x1 - l->x0);
	float Lx = m * (x - l->x0) + l->y0;
	return(fabs(Lx - y) <= epsilon);
}

// Return the parametric parameter that can be used to
// determine the point of intersection between the
// given ray and the infinite line determined by the
// given line segment.  So if the percentage returned
// is in the range [0,1], then there is an intersection,
// and if not in [0,1], there isn't one.  False is
// returned if there is no solution to this linear
// algebra problem, otherwise true is returned.  If false
// is returned, the given parameter is left untouched.
bool intersection_point(line *l, float x, float y, float angle, float *t)
{
	float a = l->x1 - l->x0;
	float b = -cos(angle);
	float c = l->y1 - l->y0;
	float d = -sin(angle);

	float det = a * d - b * c;

	if(det == 0.f)
		return false;

	*t = ((x - l->x0) * d - (y - l->y0) * b) / det;
	return true;
}

// Linearly interpolate from point 0 to
// point 1 for the given line segment.
// The given percentage need not be in [0,1].
// No check is performed to see if the given
// xy-pointers point to any one of the line
// segment end-points.
void line_point(line *l, float t, float *x, float *y)
{
	*x = l->x0 + (l->x1 - l->x0) * t;
	*y = l->y0 + (l->y1 - l->y0) * t;
}

// endof line.cc

// surface.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "surface.h"
#include "entity.h"
#include "cell.h"
#include "wall.h"
#include "camera.h"

Surface::Surface()
{
}

Surface::~Surface()
{
}

// Return true if this surface is
// visible to the given fovi.
bool Surface::IsVisible(fovi *f)
{
	line l;

	// Convert from local to
	// global coordinates.
	c->LtoG(&l.x0, &l.y0, this->l.x0, this->l.y0);
	c->LtoG(&l.x1, &l.y1, this->l.x1, this->l.y1);

	if(is_line_in_fovi(f, &l))
		return true;

	return false;
}

// By default, surfaces are not see-through.
// So return false.
bool Surface::IsEntityVisible(fovi *f, Entity *e)
{
	return false;
}

// Handle a possible collision between
// the given entity and this surface,
// which is assumed to be coporial.
// If it is non-coporial, then a virtual
// overload of this function should be
// implimented to handle that case.
// This is what portals do.  Return
// true if a collision was handled,
// return false otherwise.
bool Surface::HandleCollision(Entity *e)
{
	int num_pts;

	if(line_circle_intersect(&l, e->x, e->y, e->radius, &num_pts, 0, 0))
	{
		line_circle_adjust(&l, &e->x, &e->y, e->radius, num_pts);

		// In the case of a one-point-intersection, the adjustment
		// routine only garentees that the result leaves the circle
		// in a no-intersection state, or a two-point intersection state.
		// In the two-point intersection case, the adjustment routine
		// garentees that the result leaves the circle in a no-
		// intersection state.
		if(num_pts == 1)
			if(line_circle_intersect(&l, e->x, e->y, e->radius, &num_pts, 0, 0))
				if(num_pts == 2)
					line_circle_adjust(&l, &e->x, &e->y, e->radius, num_pts);		

		return true;
	}

	return false;
}

// By default, a surface is coporial.
// So this routine always returns false.
// Propgations are only possible by passing
// through the boundry points of non-
// coporial surfaces.
bool Surface::HandlePropagation(class Entity *e, line *m)
{
	return false;
}

// endof surface.cpp
// portal.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "portal.h"
#include "cell.h"
#include "camera.h"
#include "macros.h"
#include "main.h"
#include "map.h"

// This is used by the entity-visibility-determination algorithm.
Portal *spy_portal;

Portal::Portal()
{
	adj_c = 0;
	no_pass = false;
}

Portal::~Portal()
{
}

// Determine if the given entity is visible
// through this portal and in the adjacent
// cell with the given field of vision.
bool Portal::IsEntityVisible(fovi *f, class Entity *e)
{
	if(!(adj_c->flags & VIS_FLAG) && IsVisible(f))
	{
		// Get the global coordinates of this portal's
		// boundry.  Note that we are redoing some work
		// already done by the IsVisible() routine.
		// That's retarded, but can't be helped cleanly.
		line l;
		c->LtoG(&l.x0, &l.y0, this->l.x0, this->l.y0);
		c->LtoG(&l.x1, &l.y1, this->l.x1, this->l.y1);

		// Clip the given fovi appropriately.
		// Then if there is anything to see,
		// (which there should be), then recurse
		// into the adjacent cell.
		fovi clipper, f_copy;
		make_fovi(&clipper, &l, f->x, f->y);
		f_copy = *f;
		if(clip_fovi(&f_copy, &clipper))
		{
			// For graph traversal algorithms in dyanmic cell
			// mode, we must calculate the adjacent cell's
			// position and angle before recursing if we
			// want to be able to use that information.
			if(cell_mode == DYNAMIC_CELL_MODE)
				adj_c->CalcPosAngle(this);
			
			// Go see if the given entity is visible
			// in the adjacent cell.
			if(adj_c->IsEntityVisible(&f_copy, e))
			{
				// This pointer may be overwritten several
				// times through tail recursion.  This is
				// okay.  I'm interested in the last value
				// that this pointer gets set to.
				spy_portal = this;
				return true;
			}
		}
	}

	return false;
}

// If the given entity crosses this
// portial's boundry, propagate the
// given entity to the adjacent cell.
bool Portal::HandleCollision(Entity *e)
{
	bool ret = false;
	if(!(adj_c->flags & COLL_FLAG))
	{
		if(line_circle_intersect(&l, e->x, e->y, e->radius, 0, 0, 0))
		{		
			// Partially and temporarily propagate to
			// the adjacent cell, do collision detection
			// there as we normally would, then propagate
			// back to the original cell.  I'm not calling
			// the e->Propagate() routine here because we
			// really don't want to commit to a full
			// propagation, because it's not nessiary.
			// We only need the coordinate conversion.
			e->c->LtoG(&e->x, &e->y, e->x, e->y);
			if(cell_mode == DYNAMIC_CELL_MODE)
				adj_c->CalcPosAngle(this);
			adj_c->GtoL(&e->x, &e->y, e->x, e->y);
			ret = adj_c->HandleCollisions(e);
			adj_c->LtoG(&e->x, &e->y, e->x, e->y);
			e->c->GtoL(&e->x, &e->y, e->x, e->y);
		}
	}

	return ret;
}

// If there is a portal boundry crossing,
// then a propagation from the currently
// occupied cell to the adjacent cell occurs.
bool Portal::HandlePropagation(Entity *e, line *m)
{
	// BUG: If one end-point of the line-segment of
	//      motion lands exactly on a portal boundry,
	//      then a propagation occurs.  But then this
	//      also means that the next movement, which
	//      may or may not be into the original cell,
	//      must also cause a propagation.  This cannot
	//      be fixed by requiring one of the end-points
	//      to not lie on the portal boundry, because
	//      this causes other types of bugs that are worse.

	if(lines_intersect(&l, m)) // && !is_point_on_line(&l, m->x0, m->y0, 0.f))
		return e->Propagate(this);

	return false;
}

// Portals don't do anything when you act upon them.
bool Portal::HandleAction(class Entity *e)
{
	return false;
}

// Draw the adjacent cell.  If this function
// has been called, then it has already been
// determined that this portal is visible.
// Appropriately clip a copy of the given fovi,
// and recursively draw the adjacent cell with
// that fovi.
void Portal::Draw(fovi *f)
{
	if(cam->mode == CAM_MODE_TOPOLOGICAL)
	{
		// Draw a grey line to indicate
		// that this is a portal boundry.
		glBegin(GL_LINES);
		glColor3f(0.5f, 0.5f, 0.5f);
		draw_line(&l);
		glEnd();
	}

	// Do not recurse back into the
	// cell we came from.  We don't
	// want infinite recursion on
	// our hands.
	if(adj_c->flags & DRAW_FLAG)
		return;

	// If there is a texture associated with this
	// portal, then draw it here.  The texture may
	// have some transparency in it or it may be
	// completely opeque and we want this portal
	// to access a hidden area or something like that.
	//...(do that here)...

	GLfloat matrix[16];
	if(cell_mode == STATIC_CELL_MODE)
	{
		// Save the current modelview matrix.		
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		glPopMatrix();
	}

	line l;

	// Convert from local to
	// global coordinates.
	c->LtoG(&l.x0, &l.y0, this->l.x0, this->l.y0);
	c->LtoG(&l.x1, &l.y1, this->l.x1, this->l.y1);

	// Make a clipping fovi, copy the
	// given fovi, and then clip out of
	// the periferial portions determined
	// by what is outside of the clipping fovi.
	fovi clipper, f_copy;
	make_fovi(&clipper, &l, f->x, f->y);
	f_copy = *f;
	if(draw_all || clip_fovi(&f_copy, &clipper))
	{
		// In dynamic cell mode, calculate the location
		// and orientation of the adjacent cell WRT to
		// the cell containing this portal before we go
		// drawing the adjacent cell.
		if(cell_mode == DYNAMIC_CELL_MODE)
		{
			adj_c->CalcPosAngle(this);

			glPushMatrix();
			glTranslatef(dx, dy, 0.f);
			glRotatef(RADS_TO_DEGS(da), 0.f, 0.f, 1.f);
		}

		// Now go draw the adacent cell!!
		adj_c->Draw(&f_copy);

		if(cell_mode == DYNAMIC_CELL_MODE)
			glPopMatrix();
	}

	if(cam->mode == CAM_MODE_TOPOLOGICAL && !draw_all)
	{
		if(cell_mode == DYNAMIC_CELL_MODE)
		{
			glPushMatrix();
			glLoadIdentity();
		}

		float dx = f->x - adj_c->x;
		float dy = f->y - adj_c->y;
		glColor3f(0.3f, 0.3f, 0.3f);
		draw_fovi(&f_copy, sqrt(dx * dx + dy * dy));

		if(cell_mode == DYNAMIC_CELL_MODE)
			glPopMatrix();
	}

	if(cell_mode == STATIC_CELL_MODE)
	{
		// Restore the modelview matrix.
		glPushMatrix();
		glLoadMatrixf(matrix);
	}
}

// endof portal.cpp
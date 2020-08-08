// cell.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "buffer.h"
#include "cell.h"
#include "texture.h"
#include "macros.h"
#include "wall.h"
#include "map.h"
#include "portal.h"
#include "main.h"
#include "camera.h"

Cell::Cell()
{
	WipeBuffer((void **)e, ENTITIES_PER_CELL);
	WipeBuffer((void **)s, SURFACES_PER_CELL);
	WipeBuffer((void **)a, ARTIFACTS_PER_CELL);

	flags = 0;
	dpy_list = 0;
	t_ceil = 0;
	t_floor = 0;
	angle = 0.f;
	parent = 0;
	calc_tex_coords = true;
	stretch = false;
	clip = false;
}

Cell::~Cell()
{
}

// Return true if the given entity is visible
// within this cell with the given field of vision.
bool Cell::IsEntityVisible(fovi *f, Entity *e)
{
	flags |= VIS_FLAG;
	bool ret = false;
	int k;

	// Scan for the given entity in this cell.
	// If it's in here, determine if it's visible
	// with the given field of vision.
	for(k = 0; k < ENTITIES_PER_CELL && !ret; k++)
		if(this->e[k] == e)
			ret = this->e[k]->IsVisible(f);

	// If the entity wasn't found, then it may
	// still be visible through a portal to
	// an adjacent cell.
	for(k = 0; k < SURFACES_PER_CELL && !ret; k++)
		if(s[k])
			ret = s[k]->IsEntityVisible(f, e);

	flags &= ~VIS_FLAG;
	return ret;
}

// Calculate the location and orientation of
// this cell WRT the cell occupied by the given portal.
// It is assumed here that the cell of the given
// portal is adjacent to this cell, though it
// is not nessisary for the calculation to come out right.
// However, the result in that case would perhaps not be what you want.
void Cell::CalcPosAngle(Portal *p)
{
	p->c->LtoG(&x, &y, p->dx, p->dy);
	angle = p->c->angle + p->da;
}

// Draw the visible contents of this cell
// as well as any adjacent cells that are visible.
void Cell::Draw(fovi *f)
{
	flags |= DRAW_FLAG;

	if(cell_mode == STATIC_CELL_MODE)
	{
		glPushMatrix();
		glTranslatef(x, y, 0.f);

		if(angle != 0.f)
			glRotatef(RADS_TO_DEGS(angle), 0.f, 0.f, 1.f);
	}

	// Draw the floor and ceiling.
	if(cam->mode != CAM_MODE_TOPOLOGICAL)
		DrawFloorCeiling(f);

	int k;

	// Draw any visible surfaces.
	for(k = 0; k < SURFACES_PER_CELL; k++)
		if(s[k] && (draw_all || s[k]->IsVisible(f)))
			s[k]->Draw(f);

	// Draw any visible artifacts.
	for(k = 0; k < ARTIFACTS_PER_CELL; k++)
		if(a[k] && (draw_all || a[k]->IsVisible(f)))
			a[k]->Draw(f);

	// Draw any visible entities.  Try to
	// draw entities last because they have
	// alpha and try to blend with what's
	// already in the frame-buffer.  Notice
	// that we do not draw the camera here,
	// or else we'd have infinite recursion.
	// Might've programmed the camera's virtual
	// overload to always return false, but
	// I do have need of determining if the
	// camera is visible by other entities.
	for(k = 0; k < ENTITIES_PER_CELL; k++)
		if(e[k] && e[k] != cam && (draw_all || e[k]->IsVisible(f)))
			e[k]->Draw(f);

	if(cell_mode == STATIC_CELL_MODE)
		glPopMatrix();

	// Not only do we want to reset this flag in
	// preparation for the next frame, but we may
	// also recurse into the same cell more than once.
	flags &= ~DRAW_FLAG;
}

// This routine is not quite like one that
// find a convex hull.  It makes several
// assumptions and tries to order the points
// in a CCW fasion about their mid-point.
static void order_points_ccw(float *x, float *y, int *order, int num_points)
{
	int i, j, k;

	// Find the mid-point.
	float x_mid, y_mid;
	x_mid = y_mid = 0.f;
	for(k = 0; k < num_points; k++)
	{
		x_mid += x[k];
		y_mid += y[k];
	}
	x_mid /= float(num_points);
	y_mid /= float(num_points);

	// Now go determine the order.
	// Yes, this algorithm is slow.
	float angle_min, angle_max;
	float angle;
	angle_min = 0.f;
	for(i = 0; i < num_points; i++)
	{
		angle_max = 2.f * PI;
		for(j = 0; j < num_points; j++)
		{
			angle = quadrant_angle(x[j] - x_mid, y[j] - y_mid);
			if(angle == 0.f)
				angle = 2.f * PI;
			if(angle > angle_min && angle <= angle_max)
			{
				angle_max = angle;
				k = j;
			}
		}
		order[i] = k;
		angle_min = angle_max;
	}
}

// This is retarded, but we need to make
// sure that this point is not a duplicate
// since line-segments share vertices.
static bool maybe_put_in_point_list(float *x_list, float *y_list, float x, float y, int *num_points)
{
	// Check the given point against every
	// point in the list so far.
	float dx, dy;
	int k;
	for(k = 0; k < *num_points; k++)
	{
		dx = x - x_list[k];
		dy = y - y_list[k];
		if(dx * dx + dy * dy < EPSILON)
			break;
	}

	// If the given point is not a
	// duplicate of any point already
	// in the list, then put into it.
	if(k == *num_points)
	{
		x_list[*num_points] = x;
		y_list[*num_points] = y;
		(*num_points)++;
		return true;
	}

	return false;
}

// Draw the parts of the floor or ceiling of
// this cell that are visible within the
// given field of vision.  This is an ugly
// task, and was not thought out before the
// program was designed.  Had it been, then
// a much cleaner and faster solution may
// have been found.
void Cell::DrawFloorCeiling(fovi *f)
{
	if(clip)
	{
		CalcFloorCeilTexCoords();

		// Make a list of points that
		// determine the visible floor/ceiling polygon.
		// Notice that there are 2 vertices per surface.
		// Then there may be 4 points of intersection
		// with the given fovi, yet another vertex may
		// be needed for the camera if it occupies this cell.
		static float x_list[SURFACES_PER_CELL * 2 + 4 + 1 + 100];
		static float y_list[SURFACES_PER_CELL * 2 + 4 + 1 + 100];
		int num_points = 0;
		line l;
		float fx, fy, t;
		int k;
		for(k = 0; k < SURFACES_PER_CELL; k++)
		{
			if(s[k])
			{
				LtoG(&l.x0, &l.y0, s[k]->l.x0, s[k]->l.y0);
				LtoG(&l.x1, &l.y1, s[k]->l.x1, s[k]->l.y1);				

				if(is_point_in_fovi(f, l.x0, l.y0))
					maybe_put_in_point_list(x_list, y_list, s[k]->l.x0, s[k]->l.y0, &num_points);
				if(is_point_in_fovi(f, l.x1, l.y1))
					maybe_put_in_point_list(x_list, y_list, s[k]->l.x1, s[k]->l.y1, &num_points);

				if(is_line_in_fovi(f, &l))
				{
					if(intersection_point(&l, f->x, f->y, f->heading - f->angle, &t))
					{
						if(t > 0.f && t < 1.f)
						{
							line_point(&s[k]->l, t, &fx, &fy);
							maybe_put_in_point_list(x_list, y_list, fx, fy, &num_points);
						}
					}

					if(intersection_point(&l, f->x, f->y, f->heading + f->angle, &t))
					{
						if(t > 0.f && t < 1.f)
						{
							line_point(&s[k]->l, t, &fx, &fy);
							maybe_put_in_point_list(x_list, y_list, fx, fy, &num_points);
						}
					}
				}

				if(intersection_point(&l, f->x, f->y, f->heading - f->angle, &t))
				{
					if(t > 0.f && t < 1.f)
					{
						line_point(&l, t, &fx, &fy);
						if(is_point_on_fovi_edge(f, fx, fy))
						{
							GtoL(&fx, &fy, fx, fy);
							maybe_put_in_point_list(x_list, y_list, fx, fy, &num_points);
						}
					}
				}

				if(intersection_point(&l, f->x, f->y, f->heading + f->angle, &t))
				{
					if(t > 0.f && t < 1.f)
					{
						line_point(&l, t, &fx, &fy);
						if(is_point_on_fovi_edge(f, fx, fy))
						{
							GtoL(&fx, &fy, fx, fy);
							maybe_put_in_point_list(x_list, y_list, fx, fy, &num_points);
						}
					}
				}
			}
		}

		// Throw in the camera vertex if the camera
		// occupies this cell.
		if(cam->c == this)
			maybe_put_in_point_list(x_list, y_list, cam->x, cam->y, &num_points);

		// This step is critical.  We need to
		// be able to draw the points in order.
		// Go sort the list of points.
		static int order[SURFACES_PER_CELL * 2 + 4 + 1 + 100];
		order_points_ccw(x_list, y_list, order, num_points);

		// Finally, go draw the polygon.
		// Hopefully it is convex.
		if(t_floor)
		{
			BeginTextureMapping(t_floor);
			glBegin(GL_POLYGON);
			float tx, ty;
			float x, y;
			for(k = 0; k < num_points; k++)
			{
				x = x_list[order[k]];
				y = y_list[order[k]];
				tx = x - x_min;
				ty = y - y_min;
				if(stretch)
				{
					tx /= (x_max - x_min);
					ty /= (y_max - y_min);
				}
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, -WALL_HEIGHT / 2.f + 0.001f /*hack*/);
			}
			glEnd();
			EndTextureMapping(t_floor);
		}
		// Yes, it's silly to redo the texture
		// coordinate calculations.
		if(t_ceil)
		{
			BeginTextureMapping(t_ceil);
			glBegin(GL_POLYGON);
			float tx, ty;
			float x, y;
			for(k = 0; k < num_points; k++)
			{
				x = x_list[order[k]];
				y = y_list[order[k]];
				tx = x - x_min;
				ty = y - y_min;
				if(stretch)
				{
					tx /= (x_max - x_min);
					ty /= (y_max - y_min);
				}
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, WALL_HEIGHT / 2.f - 0.001f /*hack*/);
			}
			glEnd();
			EndTextureMapping(t_ceil);
		}
	}
	else	// Draw without clipping.  This is faster, but may not look as nice.
	{
		// Barf if one of the textures is missing.
		if(!t_floor || !t_ceil)
			return;

		if(dpy_list)
		{
			// Indicate that these textures got used.
			UsedTexture(t_floor);
			UsedTexture(t_ceil);

			// Draw the floor and ceiling fast.
			// We're using the fact that our environment is
			// static for the most part.  The only things
			// dynamic are things like doors and switches.
			glCallList(dpy_list);
		}
		else
		{
			int k;

			CalcFloorCeilTexCoords();

			dpy_list = glGenLists(1);
			glNewList(dpy_list, GL_COMPILE_AND_EXECUTE);

			// Draw the floor.
			BeginTextureMapping(t_floor);
			glBegin(GL_TRIANGLES);		// Can't do triangle fan here.
			for(k = 0; k < SURFACES_PER_CELL; k++)
			{
				if(s[k])
				{
					glTexCoord2f(tx, ty);
					glVertex3f(0.f, 0.f, -WALL_HEIGHT / 2.f);
					glTexCoord2f(s[k]->t.x0, s[k]->t.y0);
					glVertex3f(s[k]->l.x0, s[k]->l.y0, -WALL_HEIGHT / 2.f);
					glTexCoord2f(s[k]->t.x1, s[k]->t.y1);
					glVertex3f(s[k]->l.x1, s[k]->l.y1, -WALL_HEIGHT / 2.f);
				}
			}
			glEnd();
			EndTextureMapping(t_floor);

			// Draw the ceiling.
			BeginTextureMapping(t_ceil);
			glBegin(GL_TRIANGLES);
			for(k = 0; k < SURFACES_PER_CELL; k++)
			{
				if(s[k])
				{
					glTexCoord2f(tx, ty);
					glVertex3f(0.f, 0.f, WALL_HEIGHT / 2.f);
					glTexCoord2f(s[k]->t.x0, s[k]->t.y0);
					glVertex3f(s[k]->l.x0, s[k]->l.y0, WALL_HEIGHT / 2.f);
					glTexCoord2f(s[k]->t.x1, s[k]->t.y1);
					glVertex3f(s[k]->l.x1, s[k]->l.y1, WALL_HEIGHT / 2.f);
				}
			}
			glEnd();
			EndTextureMapping(t_ceil);
				
			glEndList();
		}
	}
}

// Rotate the given point about the
// origin by the given angle.
static void rotate_point(float *x, float *y, float angle)
{
	// Use the following complex number to
	// transform the given point.
	float re = cos(angle);
	float im = sin(angle);

	// Complex multiplication is defined by the following formula.
	// (a+bi)(c+di) = ac+adi+bci+bdi2 = (ac-bd)+(ad+bc)i
	float tmp = *x;
	*x = *x * re - *y * im;
	*y = tmp * im + *y * re;
}

// Convert the given source coordinates from
// global coordinates to local coordinates.
void Cell::GtoL(float *x_dst, float *y_dst, float x_src, float y_src)
{
	x_src -= x;
	y_src -= y;
	rotate_point(&x_src, &y_src, -angle);

	// Return the local coordinates.
	*x_dst = x_src;
	*y_dst = y_src;
}

// Convert the given source coordinates from
// local coordinates to global coordinates.
void Cell::LtoG(float *x_dst, float *y_dst, float x_src, float y_src)
{
	rotate_point(&x_src, &y_src, angle);
	x_src += x;
	y_src += y;

	// Return the global coordinates.
	*x_dst = x_src;
	*y_dst = y_src;
}

// Calculate the texture coordinates for
// the floor and ceiling textures so that
// when the floor/ceiling texture is applied to the
// set of triangles forming the floor/ceiling
// geometry, the texture image blends seemlessly
// between each polygon edge.  Unfortunately,
// however, this doesn't handle the seems
// visible between adjacent cells.  It also
// stretches the textures out if the cell's
// geometry is longer in one dimension than
// in another.
void Cell::CalcFloorCeilTexCoords(void)
{
	if(!calc_tex_coords)
		return;
	calc_tex_coords = false;

	x_min = x_max = 0.f;
	y_min = y_max = 0.f;

	// Find the smallest rectangle that encloses
	// the geometry of this cell.
	int k;
	for(k = 0; k < SURFACES_PER_CELL; k++)
	{
		if(s[k])
		{
			x_min = MIN(x_min, MIN(s[k]->l.x0, s[k]->l.x1));
			x_max = MAX(x_max, MAX(s[k]->l.x0, s[k]->l.x1));
			y_min = MIN(y_min, MIN(s[k]->l.y0, s[k]->l.y1));
			y_max = MAX(y_max, MAX(s[k]->l.y0, s[k]->l.y1));
		}
	}

	float dx = x_max - x_min;
	float dy = y_max - y_min;

	// Now go through and calculate texture coordinates
	// for each vertex of each surface of this cell.
	for(k = 0; k < SURFACES_PER_CELL; k++)
	{
		if(s[k])
		{
			s[k]->t.x0 = s[k]->l.x0 - x_min;
			s[k]->t.x1 = s[k]->l.x1 - x_min;
			s[k]->t.y0 = s[k]->l.y0 - y_min;
			s[k]->t.y1 = s[k]->l.y1 - y_min;
			if(stretch)
			{
				s[k]->t.x0 /= dx;
				s[k]->t.x1 /= dx;
				s[k]->t.y0 /= dy;
				s[k]->t.y1 /= dy;
			}
		}
	}

	// Calculate the texture coordinates for the
	// center of this cell.
	tx = -x_min;
	ty = -y_min;
	if(stretch)
	{
		tx /= dx;
		ty /= dy;
	}
}

// Handle collisions, if any, between the surfaces of
// this cell and the bounding volume (cylinder) of
// the given entity.  True is returned if one or more
// collisions were handled, and false if not.  It is
// not assumed that the given entity is a member of
// this cell.  It is only assumed that the given
// entity's coordinates are local to this cell.
// Also note that collision detection only performed
// between surfaces and entities.  No entity-to-entity
// collision detection is done because I don't want
// entities getting stuck on one another.
bool Cell::HandleCollisions(Entity *e)
{
	flags |= COLL_FLAG;

	// Check for collisions between this
	// entity and the given cell.
	bool ret = false;
	int i, j = 0;
	for(i = 0; i < SURFACES_PER_CELL; i++)
	{
		// If a collision gets handled, then the entity's
		// position has changed once again and we need
		// to reset the loop and recheck some surfaces
		// for collisions again.
		if(s[i] && s[i]->HandleCollision(e))
		{
			// We have to be careful here that
			// this doesn't cause an infinite loop.
			// It shouldn't if the cell geometry is right,
			// and no entity has a radius that's too fat.
			// Even so, die if we've been here too long.
			if(j++ > 1000)
				fatal_error("Collision resolution loop has most likely gone infinite!");
			i = 0;
			ret = true;
		}
	}

	flags &= ~COLL_FLAG;
	return ret;
}

// Handle a possible propagation of the given
// entity with one of the surfaces of this cell.
// Notice that this is not a recursive routine.
// There will only be one propagation allowed
// per every entity movement.  This is insured
// by the collision detection code and also by
// the geometry of the loaded map.  Doing possibly
// multiple propagations recursively per move is
// possible to impliment, but unessisary since
// no single movement of the entity should result
// in croessing multiple portal boundries.
bool Cell::HandlePropagations(Entity *e, line *m)
{
	if(e->c != this)
		return false;

	int k;
	for(k = 0; k < SURFACES_PER_CELL; k++)
	{
		if(s[k] && s[k]->HandlePropagation(e, m))
			return true;
	}

	return false;
}

// Return the portal of this cell, if any,
// that points to the given cell which may
// or may not be adjacent to this cell.
Portal *Cell::ReturnPortal(Cell *adj_c)
{
	int k;
	Portal *p;
	for(k = 0; k < SURFACES_PER_CELL; k++)
	{
		if(s[k] && s[k]->IsPortal())
		{
			p = (Portal *)s[k];
			if(p->adj_c == adj_c)
				return p;
		}
	}
	return(0);
}

// Insert the given entity in this cell.
bool Cell::EnterEntity(Entity *e)
{
	if(!e->c)
	{
		if(InsertIntoBuffer((void **)this->e, ENTITIES_PER_CELL, e))
		{
			e->c = this;
			return true;
		}
	}

	return false;
}

// Remove the given entity from this cell.
bool Cell::ExitEntity(Entity *e)
{
	if(!e->c || e->c != this)
		return false;

	if(RemoveFromBuffer((void **)this->e, ENTITIES_PER_CELL, e))
	{
		e->c = 0;
		return true;
	}

	return false;
}

// Find an empty slot for the given surface
// in this cell's surface buffer.  We don't
// need something fancy like a linked-list
// or anything.  This array will do fine.
bool Cell::InsertSurface(Surface *s)
{
	if(InsertIntoBuffer((void **)this->s, SURFACES_PER_CELL, s))
	{
		s->c = this;
		return true;
	}

	return false;
}

// Remove the given surface from this cell,
// assuming it is part of this cell.
bool Cell::RemoveSurface(Surface *s)
{
	if(RemoveFromBuffer((void **)this->s, SURFACES_PER_CELL, s))
	{
		s->c = 0;
		return true;
	}

	return false;
}

bool Cell::InsertArtifact(Artifact *a)
{
	if(InsertIntoBuffer((void **)this->a, ARTIFACTS_PER_CELL, a))
	{
		a->c = this;
		return true;
	}

	return false;
}

bool Cell::RemoveArtifact(Artifact *a)
{
	if(RemoveFromBuffer((void **)this->a, ARTIFACTS_PER_CELL, a))
	{
		a->c = 0;
		return true;
	}
	
	return false;
}

// endof cell.cpp
// entity.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include <stdio.h>
#include "macros.h"
#include "cell.h"
#include "buffer.h"
#include "entity.h"
#include "artifact.h"
#include "buffer.h"
#include "map.h"
#include "portal.h"
#include "wall.h"
#include "camera.h"
#include "tether.h"
#include "main.h"

Entity::Entity()
{
	c = 0;

	WipeBuffer((void **)inventory, ARTIFACTS_PER_ENTITY);
	WipeBuffer((void **)teth, TETHERS_PER_ENTITY);

	dpy_list = 0;
	health = 1.f;

	t0 = 0.f;
	t1 = 1.f;

	t = 0;
}

Entity::~Entity()
{
}

// Is this entity in the given fovi?
bool Entity::IsVisible(fovi *f)
{
	if(c)
	{
		float x, y;
		c->LtoG(&x, &y, this->x, this->y);
		if(is_circle_in_fovi(f, x, y, radius))
			return true;
	}
	return false;
}

// This routine is virtual, so it
// can be overridden by specific types
// of entities, however, most entities
// are drawn as just a single sprite.
// Assuming this entity is visible,
// draw the sprite facing the camera.
void Entity::Draw(fovi *f)
{
	float angle = cam->f.heading + PI / 2.f + c->angle;
	float x = radius * cos(angle);
	float y = radius * sin(angle);
	float x0 = this->x - x;
	float y0 = this->y - y;
	float x1 = this->x + x;
	float y1 = this->y + y;

	BeginTextureMapping(t);
	glBegin(GL_QUADS);

	glTexCoord2f(t0, 0.f);
	glVertex3f(x0, y0, -WALL_HEIGHT / 2.f);

	glTexCoord2f(t1, 0.f);
	glVertex3f(x1, y1, -WALL_HEIGHT / 2.f);

	glTexCoord2f(t1, 1.f);
	glVertex3f(x1, y1, -WALL_HEIGHT / 2.f + 2.f * radius);

	glTexCoord2f(t0, 1.f);
	glVertex3f(x0, y0, -WALL_HEIGHT / 2.f + 2.f * radius);

	glEnd();
	EndTextureMapping(t);
}

// This is a neat function!
// What it does is determine if the
// given entity is in the line of
// site of this entity.  So entities
// can hide behind walls from other
// entities.  The capability of determining
// whether or not one entity is in the
// line of site of another entity gives me
// the capability to write computer controlled
// entities that can realistically track
// the player if in view.  If true is returned,
// the global spy_portal pointer is set to
// zero if the given entity is in the same
// cell, or set to the portal leading to
// the given entity.
bool Entity::CanSee(Entity *e)
{
	if(!c)
		return false;

	if(cell_mode == DYNAMIC_CELL_MODE)
	{
		c->x = 0.f;
		c->y = 0.f;
		c->angle = 0.f;
	}

	c->LtoG(&f.x, &f.y, x, y);

	spy_portal = 0;
	return c->IsEntityVisible(&f, e);
}

// Move this entity within its cell.
// Handle collision-detection and
// scene-graph traversal.  Return true
// if this move resulted in a portal
// boundry crossing, and therefore a
// propagation from one cell to an
// adjacent cell.
bool Entity::Move(float dx, float dy)
{
	line m;

	// This must be done inpreparation for
	// performing the collision detection
	// and propagation algorithms when in
	// dyanmic cell mode.  This is done in
	// preparation for any algorithm that
	// traverses the graph and examines
	// coordinate data when in dynamic cell mode.
	if(cell_mode == DYNAMIC_CELL_MODE)
	{
		c->x = 0.f;
		c->y = 0.f;
		c->angle = 0.f;
	}

	// We can't let the entity move further
	// than it's bounding cylinder volume or
	// the collision detection code will not
	// be able to handle it.
	float mag2 = dx * dx + dy * dy;
	if(mag2 > radius * radius)
	{
		float scalar = radius / sqrt(mag2);
		dx *= scalar;
		dy *= scalar;
	}

	// Record the initial position.
	m.x0 = x;
	m.y0 = y;

	// Move in the desired direction.
	x += dx;
	y += dy;

	// Modify our position to correct for a
	// detected collision, if any.
	c->HandleCollisions(this);

	// Record the final position.
	m.x1 = x;
	m.y1 = y;

	// After all collisions with coporial
	// surfaces, now handle boundry crossings
	// of non-coporial surfaces.
	bool ret = c->HandlePropagations(this, &m);

	// Make coordinates local to the entities cell
	// also global coordinates.
	if(cell_mode == DYNAMIC_CELL_MODE)
	{
		c->x = 0.f;
		c->y = 0.f;
		c->angle = 0.f;
	}

	// Calculate global coordinates from local coordaintes.
	c->LtoG(&f.x, &f.y, x, y);

//	printf("(x,y) = (%f,%f)\n", x, y);

	return ret;
}

// Handle the relocation of this entity from
// its currently occupied cell to the one given
// as the cell pointed to by the given portal.
bool Entity::Propagate(Portal *p)
{
	// If this entity is attached to a
	// tether, then update it since things
	// have changed.
	int k;
	for(k = 0; k < TETHERS_PER_ENTITY; k++)
		if(teth[k])
			MaintainTether(teth[k], this, p);

	c->LtoG(&x, &y, x, y);

	if(cell_mode == DYNAMIC_CELL_MODE)
	{
		p->adj_c->CalcPosAngle(p);
		f.heading -= p->da;
	}

	// Leave the old cell and enter the new cell.
	c->ExitEntity(this);
	p->adj_c->EnterEntity(this);

	c->GtoL(&x, &y, x, y);

	return true;
}

// Strafe left/right.
bool Entity::Strafe(float ds)
{
	float angle = f.heading - PI / 2.f;
	return Move(ds * (float)cos(angle), ds * (float)sin(angle));
}

// Move forward/backward.
bool Entity::MoveForward(float ds)
{
	float angle = f.heading;
	return Move(ds * (float)cos(angle), ds * (float)sin(angle));
}

// Turn left/right.
void Entity::Turn(float da)
{
	rotate_fovi(&f, da);
}

// This routine is used mainly by computer
// controlled entities to make the naviation
// programming of them more convenient.
// Return true if this routine caused a cell
// propagation to occur.  The move speed and
// turn speed are in units of units per second.
bool Entity::Navigate(float target_x, float target_y, float move_speed, float turn_speed)
{
	// Notice that the entity should never
	// get stuck behind walls when trying to
	// traverse the scene graph because every
	// cell is a convex polygon, and the entity
	// moves towards portals, or other surfaces.
	float dx = target_x - x;
	float dy = target_y - y;
	float target_heading = quadrant_angle(dx, dy);
	float ds = move_speed / FPS;
	make_within_pi(&f.heading, &target_heading);
	float da = (float)SIGN(target_heading - f.heading) * (turn_speed / FPS);
	Turn(da);
	return MoveForward(ds);
}

// Act on the nearest peice of the environment
// directly in front of this entity.  But the
// peice must be in this cell.  So we wouldn't
// want to place any actable object too close
// to a portal or there would be some confusion
// when the item wont respond, though the user
// appears to be close enough.  This problem can
// be fixed if I reprogram this routine to check
// adjacent cells.  But for now, let's keep it simple.
bool Entity::Act(void)
{
	int k;
	float dist;
	static const float da = (float)DEGS_TO_RADS(35);
	float angle_dist, angle0, angle1;
	float dx, dy;

	// Find the nearest surface.	(fix this proximity test)
	Surface *nearest_s = 0;
	float nearest_s_dist = 1000.f;
	Surface *s;
	for(k = 0; k < SURFACES_PER_CELL; k++)
	{
		s = c->s[k];
		if(s)
		{
			// Are we roughly facing this surface?
			angle0 = s->l.angle + PI;
			angle1 = f.heading;
			make_within_pi(&angle0, &angle1);
			angle_dist = ABS(angle0 - angle1);
			if(-da <= angle_dist && angle_dist <= da)
			{
				// Are we closer to this surface and
				// are we close enough to this surface?
				dist = distance_to_line_midpt(&s->l, x, y);
				if(dist < 5.f * radius && dist < nearest_s_dist)
				{
					nearest_s_dist = dist;
					nearest_s = s;
				}
			}
		}
	}

	// Find the nearest artifact.
	Artifact *nearest_a = 0;
	float nearest_a_dist = 1000.f;
	Artifact *a;
	for(k = 0; k < ARTIFACTS_PER_CELL; k++)
	{
		a = c->a[k];
		if(a)
		{
			dx = a->x - x;
			dy = a->y - y;
			angle0 = quadrant_angle(dx, dy);
			angle1 = f.heading;
			make_within_pi(&angle0, &angle1);
			angle_dist = ABS(angle0 - angle1);
			if(-da <= angle_dist && angle_dist <= da)
			{
				dist = sqrt(dx * dx + dy * dy);
				if(dist < 5.f * radius && dist < nearest_a_dist)
				{
					nearest_a_dist = dist;
					nearest_a = a;
				}
			}
		}
	}

	// Find the nearest entity.
	//...
	Entity *nearest_e = 0;
	float nearest_e_dist = 1000.f;

	// Determine which found peices of
	// the environment are closest and
	// choose one of them to act upon.
	float nearest_dist = MIN(nearest_s_dist, MIN(nearest_a_dist, nearest_e_dist));
	if(nearest_s && nearest_dist == nearest_s_dist)
	{
		return nearest_s->HandleAction(this);
	}
	else if(nearest_a && nearest_dist == nearest_a_dist)
	{
		return nearest_a->HandleAction(this);
	}
	else if(nearest_e && nearest_dist == nearest_e_dist)
	{
		//...
		return false;
	}

	return false;
}

// Put the given artifact in this
// entity's inventory buffer.
bool Entity::TakeArtifact(Artifact *a)
{
	// If the artifact is already taken, we can't take it.
	if(a->e)
		return false;

	if(InsertIntoBuffer((void **)inventory, ARTIFACTS_PER_ENTITY, a))
	{
		a->e = this;
		return true;
	}

	return false;
}

bool Entity::DiscardArtifact(Artifact *a)
{
	// We won't be able to discard what isn't ours.
	if(a->e != this)
		return false;

	if(RemoveFromBuffer((void **)inventory, ARTIFACTS_PER_ENTITY, a))
	{
		a->e = 0;
		return true;
	}

	return false;
}

// This buffer holds the active
// entities.  We don't need any-
// thing complicated here.  A
// simple array will do fine.
// Notice that we don't manager entity
// memory here, just the entities.
#define ENTITIES		20
static Entity *EntityBuffer[ENTITIES];

// Zero the entity buffer.
void InitEntityBuffer(void)
{
	WipeBuffer((void **)EntityBuffer, ENTITIES);
}

// Put the given entity into the
// entity buffer and put it into
// the given cell.
bool SpawnEntity(Entity *e, Cell *c)
{
	if(!c->EnterEntity(e))
		return false;

	if(!InsertIntoBuffer((void **)EntityBuffer, ENTITIES, e))
	{
		c->ExitEntity(e);
		return false;
	}

	return true;
}

// Remove the given entity from the
// buffer if it is found there.
bool KillEntity(Entity *e)
{
	if(e->c && !e->c->ExitEntity(e))
		return false;

	return RemoveFromBuffer((void **)EntityBuffer, ENTITIES, e);
}

// Drive all the entities.  The
// driver routine for each entity
// is the brain of that entity.
void DriveEntities(void)
{
	int k;
	Entity *e;
	for(k = 0; k < ENTITIES; k++)
	{
		e = EntityBuffer[k];
		if(e)
			e->Drive();
	}
}

// endof entity.cpp
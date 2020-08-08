// door.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "door.h"
#include "wall.h"
#include "main.h"
#include "artifact.h"
#include "entity.h"
#include "camera.h"
#include "job.h"
#include "macros.h"
#include <stdio.h>

Door::Door()
{
	state = DOOR_STATE_CLOSED;
	draw_flag = 0;
	t = 1.f;
	t_left_door = 0;
	t_right_door = 0;
}

Door::~Door()
{
}

// If the door is open, then we can
// say we're a portal, otherwise we're
// as solid as a wall.
bool Door::IsPortal(void)
{
	if(state == DOOR_STATE_CLOSED)
		return false;
	return true;
}

// If the door is open, determine if the
// given entity is visible within the
// given field of vision.
bool Door::IsEntityVisible(fovi *f, Entity *e)
{
	if(state == DOOR_STATE_CLOSED)
		return false;
	return Portal::IsEntityVisible(f, e);
}

// Act as a surface if the door is closed.
// If the door is open, or half-open, then
// act like a portal.
bool Door::HandleCollision(Entity *e)
{
	switch(state)
	{
		case DOOR_STATE_CLOSED:
		{
			return Surface::HandleCollision(e);
		}
		case DOOR_STATE_OPEN:
		case DOOR_STATE_OPENNING:
		case DOOR_STATE_CLOSING:
		{
			return Portal::HandleCollision(e);
		}
	}

	return false;
}

// Act as a surface if the door is closed.
// If the door is open, or half-open, then
// act like a portal.
bool Door::HandlePropagation(Entity *e, line *m)
{
	switch(state)
	{
		case DOOR_STATE_CLOSED:
		{
			return Surface::HandlePropagation(e, m);
		}
		case DOOR_STATE_OPEN:
		case DOOR_STATE_OPENNING:
		case DOOR_STATE_CLOSING:
		{
			return Portal::HandlePropagation(e, m);
		}
	}

	return false;
}

// When an entity acts on a door, the
// entity tries to open the door if it's closed,
// or close the door if it's open.
bool Door::HandleAction(class Entity *e)
{
	switch(state)
	{
		case DOOR_STATE_CLOSED:
		case DOOR_STATE_CLOSING:
		{
			return OpenDoor(e);
		}
		case DOOR_STATE_OPEN:
		case DOOR_STATE_OPENNING:
		{
			return CloseDoor(e);
		}
	}

	return false;
}

bool AnimateDoor(void *data)
{
	Door *d = (Door *)data;
	static float dt = 0.3;		// Units per second.

	if(d->state == DOOR_STATE_OPENNING)
	{
		d->t -= dt / FPS;
		if(d->t <= 0.f)
		{
			d->ChangeState(DOOR_STATE_OPEN);
			return true;
		}
	}
	else if(d->state == DOOR_STATE_CLOSING)
	{
		d->t += dt / FPS;
		if(d->t >= 1.f)
		{
			d->ChangeState(DOOR_STATE_CLOSED);
			return true;
		}
	}

	d->d->t = d->t;
	return false;
}

void Door::Draw(fovi *f)
{
	// Don't draw the same logical door
	// in the same place twice.
	if(d->draw_flag)
		return;

	if(cam->mode == CAM_MODE_TOPOLOGICAL)
	{
		if(state == DOOR_STATE_CLOSED && !draw_all)
		{
			glBegin(GL_LINES);
			glColor3f(1.f, 1.f, 1.f);
			draw_line(&l);
			glEnd();
		}
		else
		{
			draw_flag = 1;
			Portal::Draw(f);
			draw_flag = 0;
		}
		return;
	}

	// If the door is open in any way,
	// then we need to draw what is behind it.
	switch(state)
	{
		case DOOR_STATE_OPEN:
		case DOOR_STATE_OPENNING:
		case DOOR_STATE_CLOSING:
		{
			draw_flag = 1;
			Portal::Draw(f);
			draw_flag = 0;
			break;
		}
	}

	// If the door is not completely open,
	// then there is something for us to draw here.
	if(state != DOOR_STATE_OPEN)
	{
		float mx, my;
		mx = (l.x0 + l.x1) / 2.f;
		my = (l.y0 + l.y1) / 2.f;

		float x0 = l.x0 + (mx - l.x0) * t;
		float y0 = l.y0 + (my - l.y0) * t;
		float x1 = l.x1 + (mx - l.x1) * t;
		float y1 = l.y1 + (my - l.y1) * t;

		if(t_left_door && t_right_door)
		{
			BeginTextureMapping(t_left_door);
			glBegin(GL_QUADS);

			glTexCoord2f(1.f - t, 0.f);
			glVertex3f(l.x0, l.y0, -WALL_HEIGHT / 2.f);

			glTexCoord2f(1.f, 0.f);
			glVertex3f(x0, y0, -WALL_HEIGHT / 2.f);

			glTexCoord2f(1.f, 1.f);
			glVertex3f(x0, y0, WALL_HEIGHT / 2.f);

			glTexCoord2f(1.f - t, 1.f);
			glVertex3f(l.x0, l.y0, WALL_HEIGHT / 2.f);

			glEnd();
			EndTextureMapping(t_left_door);

			BeginTextureMapping(t_right_door);
			glBegin(GL_QUADS);

			glTexCoord2f(0.f, 0.f);
			glVertex3f(x1, y1, -WALL_HEIGHT / 2.f);

			glTexCoord2f(t, 0.f);
			glVertex3f(l.x1, l.y1, -WALL_HEIGHT / 2.f);

			glTexCoord2f(t, 1.f);
			glVertex3f(l.x1, l.y1, WALL_HEIGHT / 2.f);

			glTexCoord2f(0.f, 1.f);
			glVertex3f(x1, y1, WALL_HEIGHT / 2.f);

			glEnd();
			EndTextureMapping(t_right_door);
		}
		else
		{
			glBegin(GL_QUADS);

			glColor3f(0.5f, 0.5f, 0.5f);
			glVertex3f(l.x0, l.y0, -WALL_HEIGHT / 2.f);
			glVertex3f(x0, y0, -WALL_HEIGHT / 2.f);
			glVertex3f(x0, y0, WALL_HEIGHT / 2.f);
			glVertex3f(l.x0, l.y0, WALL_HEIGHT / 2.f);

			glColor3f(0.4f, 0.4f, 0.4f);
			glVertex3f(x1, y1, -WALL_HEIGHT / 2.f);
			glVertex3f(l.x1, l.y1, -WALL_HEIGHT / 2.f);
			glVertex3f(l.x1, l.y1, WALL_HEIGHT / 2.f);
			glVertex3f(x1, y1, WALL_HEIGHT / 2.f);

			glEnd();
		}

		if(is_locked && state == DOOR_STATE_CLOSED)
		{
			// Draw a key-hole thing to indicate
			// that this door is locked.
			glPushMatrix();
			glTranslatef(mx + 0.01 * cos(l.angle), my + 0.01 * sin(l.angle), 0.f);
			glRotatef(RADS_TO_DEGS(l.angle - PI / 2.f), 0.f, 0.f, 1.f);
			glScalef(0.2f, 0.2f, 0.2f);
			glColor3f(0.f, 0.f, 0.f);
			glBegin(GL_TRIANGLES);
			glVertex3f(-2.f, 0.f, -3.f);
			glVertex3f(2.f, 0.f, -3.f);
			glVertex3f(0.f, 0.f, 0.f);
			glEnd();
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(0.f, 0.f, 0.f);
			float angle;
			int k;
			static const int num_sides = 10;
			for(k = 0; k <= num_sides; k++)
			{
				angle = float(k) / float(num_sides) * 2.f * PI;
				glVertex3f(2.f * cos(angle), 0.f, 2.f * sin(angle));
			}
			glEnd();
			glPopMatrix();
		}
	}
}

// Let the given entity try to open this door.
bool Door::OpenDoor(Entity *e)
{
	// If the door is locked, try using everything
	// in the given entity's inventory on this door
	// until the door is unlocked or we run out of
	// inventory items to try.
	if(is_locked)
	{
		if(cheat_mode)
			UnlockDoor();
		else
		{
			int k;
			Artifact *a;
			for(k = 0; k < ARTIFACTS_PER_ENTITY; k++)
			{
				a = e->inventory[k];
				if(a)
				{
					a->UseOn(this);
					if(!is_locked)
						break;
				}
			}
		}
	}

	// If the door isn't locked, we can act on it.
	if(!is_locked)
	{
		if(state == DOOR_STATE_CLOSED ||
			state == DOOR_STATE_CLOSING)
		{
			ChangeState(DOOR_STATE_OPENNING);
			return true;
		}
	}

	return false;
}

// Let the given entity close this door.
bool Door::CloseDoor(Entity *e)
{
	if(state == DOOR_STATE_OPEN ||
		state == DOOR_STATE_OPENNING)
	{
		ChangeState(DOOR_STATE_CLOSING);
		return true;
	}

	return false;
}

// Assuming that the door can be unlocked, do it.
void Door::UnlockDoor(void)
{
	is_locked = false;
	d->is_locked = false;
}

// Assuming that the door can be locked, do it.
void Door::LockDoor(void)
{
	if(state == DOOR_STATE_OPEN ||
		state == DOOR_STATE_OPENNING)
	{
		ChangeState(DOOR_STATE_CLOSING);
	}

	is_locked = true;
	d->is_locked = true;
}

// Handle state changes of the door.
// Also, mirror state changes to
// the door associated with this
// one in the adjacent cell.  This is
// silly and redundant, but given the
// design I've chosen, I can't see a
// cleaner way to impliment this.
// It isn't too bad.
void Door::ChangeState(DoorState state)
{
	switch(state)
	{
		case DOOR_STATE_OPEN:
		{
			t = 0.f;
			break;
		}
		case DOOR_STATE_CLOSED:
		{
			t = 1.f;
			break;
		}
		case DOOR_STATE_OPENNING:
		case DOOR_STATE_CLOSING:
		{
			if(this->state == DOOR_STATE_OPEN ||
				this->state == DOOR_STATE_CLOSED)
			{
				(void)StartJob(AnimateDoor, (void *)this);
			}
			break;
		}
	}
	
	this->state = state;
	d->state = state;
	d->t = t;
}

// endof door.cpp
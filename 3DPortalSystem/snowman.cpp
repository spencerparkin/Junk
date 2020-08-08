// snowman.cpp

// Programmed by Spencer T. Parkin

#define _CRT_TERMINATE_DEFINED

#include <glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "macros.h"
#include "snowman.h"
#include "camera.h"
#include "main.h"
#include "cell.h"
#include "portal.h"
#include "random.h"
#include "wall.h"

SnowMan::SnowMan()
{
	target_p = 0;
	recent_c = 0;
	spy_flag = false;
	t = 0;
	t_happy = t_mad = 0;
	t_backside = 0;
	target_e = 0;
}

SnowMan::~SnowMan()
{
}

void SnowMan::Draw(fovi *f)
{
	if(cam->mode == CAM_MODE_TOPOLOGICAL)
	{
		glBegin(GL_LINE_LOOP);
		if(spy_flag)
			glColor3f(0.f, 1.f, 0.f);
		else
			glColor3f(1.f, 0.f, 0.f);
		glVertex2f(x - 0.1f, y - 0.1f);
		glVertex2f(x + 0.1f, y - 0.1f);
		glVertex2f(x + 0.1f, y + 0.1f);
		glVertex2f(x - 0.1f, y + 0.1f);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(x, y);
		glVertex2f(x + radius * cos(this->f.heading),
					y + radius * sin(this->f.heading));
		glColor3f(0.9f, 0.9f, 0.9f);
		glVertex2f(x, y);
		glVertex2f(x + radius * cos(this->f.heading - this->f.angle),
					y + radius * sin(this->f.heading - this->f.angle));
		glVertex2f(x, y);
		glVertex2f(x + radius * cos(this->f.heading + this->f.angle),
					y + radius * sin(this->f.heading + this->f.angle));
		glEnd();
	}
	else
	{
		// Try to give some indication as to whether
		// the snow man is facing the camera or not.
		// I might be able to give more information by
		// reversing the texture coordinates based upon
		// the quadrant the angle is in.
		float angle0 = this->f.heading + c->angle;
		float angle1 = cam->f.heading;
		make_within_pi(&angle0, &angle1);
		int q = which_quadrant(angle1 - angle0);
		if(q == 1 || q == 4)
			t = t_backside;
		else
		{
			if(spy_flag)
				t = t_mad;
			else
				t = t_happy;
		}
		if(q == 1 || q == 2)
		{
			t0 = 0.f;
			t1 = 1.f;
		}
		else
		{
			t0 = 1.f;
			t1 = 0.f;
		}		

		// Draw the sprite.
		Entity::Draw(f);
	}
}

// This routine is the brain of
// the snow-man.  The snow-man will
// wander the scene graph until it
// can see the main entity.  Then it
// track the main entity for as long
// as it can see it, or until it can
// touch the main entity, killing it,
// and the game is over.
void SnowMan::Drive(void)
{
	// Check out target, if we have one.
	// (It might be interesting to pit
	// snowmen against snowmen.  A sort of
	// snowman civil war!  Yeah!!)
	if(target_e)
		spy_flag = CanSee(target_e);
	else
		spy_flag = false;

#if 0	// Used to test to see if it's working.
	static bool flag = false;
	if(flag != spy_flag)
	{
		flag = spy_flag;
		if(flag)
			printf("snowman can see you!\n");
		else
			printf("snowman can't see you!\n");
	}
#endif

	// Inflict damage on the target entity
	// if we are in close enough proximity.
	if(cheat_mode == false && target_e && target_e->c == c)
	{
		float dx = target_e->x - x;
		float dy = target_e->y - y;
		float r = target_e->radius + radius;
		if(dx * dx + dy * dy <= r * r)
		{
			static const float dh = 20.f;	// In units of health percentage per second.
			target_e->health -= (dh / 100.f) / FPS;
		}
	}

	// If we can see it, set course to
	// close in on the target.
	if(spy_flag)
	{
		if(spy_portal)
			target_p = spy_portal;
		else
		{
			target_x = target_e->x;
			target_y = target_e->y;
		}
	}
	// Otherwise, find a target portal.
	else if(!target_p)
	{
		int i, j;

		// Make a list of all the portals.
		Portal *p[SURFACES_PER_CELL];
		for(i = j = 0; i < SURFACES_PER_CELL; i++)
			if(c->s[i] && c->s[i]->IsPortal() && !((Portal *)c->s[i])->no_pass)
				p[j++] = (Portal *)c->s[i];

		// In this case, this location of the map is
		// not yet accessable to the player.  Just
		// be dormant in that case.
		if(j == 0)
			return;

		// If there's only one choice, choose it.
		if(j == 1)
			target_p = p[0];
		else
		{
			// Choose one of the portals at random.
			// Try not to choose the portal we came
			// through by rejecting those that go to
			// the recent cell.  Note howver, that
			// different portals may go to the same cell.
			// Only a small number of rejections are made.
			int k = 0;
			do
			{
				i = RandomNumber(0, j - 1);
				target_p = p[i];
			}
			while(target_p->adj_c == recent_c && k++ < 5);
		}

		// Use this to indicate where we came from
		// so that we can figure out where to go later.
		recent_c = c;
	}

	// Calculate our target coordinates.
	if(target_p)
	{
		// If at any point our target portal
		// becomes unpassiable, abort the target.
		if(target_p->no_pass == true)
		{
			target_p = 0;
			target_x = 0.f;
			target_y = 0.f;
		}
		else
		{
			line *l = &target_p->l;
			target_x = (l->x0 + l->x1) / 2.f;
			target_y = (l->y0 + l->y1) / 2.f;
			static float boundry_cross_dist = 0.1f;
			target_x -= boundry_cross_dist * cos(l->angle);
			target_y -= boundry_cross_dist * sin(l->angle);
		}
	}

	// Indicate that we need a new target
	// portal if our move generated a cell propagation.
	if(Navigate(target_x, target_y, 0.7f, PI / 2.f))
		target_p = 0;
}

// endof snowman.cpp
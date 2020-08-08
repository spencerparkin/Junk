// wall.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "wall.h"
#include "camera.h"
#include "cell.h"
#include "macros.h"

Wall::Wall()
{
	t = 0;

	// Override map colors for now...
	r = 1.f;
	g = 1.f;
	b = 1.f;

	clip = false;
	stretch = false;
}

Wall::~Wall()
{
}

void Wall::Draw(fovi *f)
{
	line l = this->l;
	float t0, t1;

	if(clip)
	{
		c->LtoG(&l.x0, &l.y0, this->l.x0, this->l.y0);
		c->LtoG(&l.x1, &l.y1, this->l.x1, this->l.y1);
		clip_line(f, &l, &t0, &t1);
		c->GtoL(&l.x0, &l.y0, l.x0, l.y0);
		c->GtoL(&l.x1, &l.y1, l.x1, l.y1);
	}
	else
	{
		t0 = 0.f;
		t1 = stretch ? 1.f : (length / WALL_HEIGHT);
	}

	if(cam->mode == CAM_MODE_TOPOLOGICAL)
	{
		// In an over-head view of the map,
		// just draw walls as lines in the plane.
		glBegin(GL_LINES);
		glColor3f(r, g, b);
		draw_line(&l);
		glEnd();
	}
	else
	{
		if(t)
		{
			// Draw a textured wall-quad CCW.
			BeginTextureMapping(t);
			glBegin(GL_QUADS);

			glTexCoord2f(t0, 0.f);
			glVertex3f(l.x0, l.y0, -WALL_HEIGHT / 2.f);

			glTexCoord2f(t1, 0.f);
			glVertex3f(l.x1, l.y1, -WALL_HEIGHT / 2.f);

			glTexCoord2f(t1, 1.f);
			glVertex3f(l.x1, l.y1, WALL_HEIGHT / 2.f);

			glTexCoord2f(t0, 1.f);
			glVertex3f(l.x0, l.y0, WALL_HEIGHT / 2.f);

			glEnd();
			EndTextureMapping(t);
		}
		else
		{
			// Draw the wall-quad CCW.
			glBegin(GL_QUADS);
			glColor3f(r, g, b);
			glVertex3f(l.x0, l.y0, -WALL_HEIGHT / 2.f);
			glVertex3f(l.x1, l.y1, -WALL_HEIGHT / 2.f);
			glVertex3f(l.x1, l.y1, WALL_HEIGHT / 2.f);
			glVertex3f(l.x0, l.y0, WALL_HEIGHT / 2.f);
			glEnd();
		}
	}
}

// Walls don't do anything when you act upon them.
// It might be interesting to program some walls
// to be secret passages like on Wolfenstein 3D.
bool Wall::HandleAction(class Entity *e)
{
	return false;
}

// endof wall.cpp
// artifact.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "artifact.h"
#include "main.h"
#include "cell.h"
#include "macros.h"
#include "camera.h"

Artifact::Artifact()
{
	angle = 0.f;
	dpy_list = 0;
	c = 0;
	e = 0;
}

Artifact::~Artifact()
{
}

void Artifact::Draw(fovi *f)
{
	if(cam->mode == CAM_MODE_TOPOLOGICAL)
	{
		const int sides = 10;
		float angle;
		int k;
		glBegin(GL_LINE_LOOP);
//		glColor3f(r, g, b);
		glColor3f(1.f, 1.f, 1.f);
		for(k = 0; k < sides; k++)
		{
			angle = float(k) / float(sides) * 2.f * PI;
			glVertex2f(x + radius * cos(angle),
						y + radius * sin(angle));
		}
		glEnd();
	}
}

bool Artifact::IsVisible(fovi *f)
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

void Artifact::Rotate(void)
{
	static float rads_per_sec = PI / 16.f;
	angle += rads_per_sec / FPS;
}

// The default action taken by an entity
// on an artifact is to put it in the
// entity's inventory.  This action may
// be overridden by a derived class of artifact.
// For example, a health-pack would be used
// on the spot, and not taken by the entity.
bool Artifact::HandleAction(Entity *e)
{
	if(c && c->RemoveArtifact(this))
	{
		e->TakeArtifact(this);
		return true;
	}

	return false;
}

// endof artifact.cpp
// healthpack.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "healthpack.h"
#include "entity.h"
#include "cell.h"
#include "job.h"
#include "macros.h"
#include "main.h"

static bool RotateHealthPack(void *data)
{
	HealthPack *hp = (HealthPack *)data;
	hp->angle += PI / 5.f / FPS;
	return false;	// Stay alive.
}

HealthPack::HealthPack()
{
	// The default...
	type = LARGE_HEALTH_PACK;
	angle = 0.f;
	hp_job = StartJob(RotateHealthPack, this);
}

HealthPack::~HealthPack()
{
}

// Draw the cross parallel to the XY-plane.
static void draw_white_cross(float side_len)
{
	glColor3f(1.f, 1.f, 1.f);
	glBegin(GL_QUADS);

	glVertex3f(-side_len / 2.f + 0.1f, -side_len / 8.f, side_len / 2.f + 0.02f);
	glVertex3f(side_len / 2.f - 0.1f, -side_len / 8.f, side_len / 2.f + 0.02f);
	glVertex3f(side_len / 2.f - 0.1f, side_len / 8.f, side_len / 2.f + 0.02f);
	glVertex3f(-side_len / 2.f + 0.1f, side_len / 8.f, side_len / 2.f + 0.02f);

	glVertex3f(-side_len / 8.f, -side_len / 2.f + 0.1f, side_len / 2.f + 0.02f);
	glVertex3f(side_len / 8.f, -side_len / 2.f + 0.1f, side_len / 2.f + 0.02f);
	glVertex3f(side_len / 8.f, -side_len / 8.f, side_len / 2.f + 0.02f);
	glVertex3f(-side_len / 8.f, -side_len / 8.f, side_len / 2.f + 0.02f);

	glVertex3f(-side_len / 8.f, side_len / 8.f, side_len / 2.f + 0.02f);
	glVertex3f(side_len / 8.f, side_len / 8.f, side_len / 2.f + 0.02f);
	glVertex3f(side_len / 8.f, side_len / 2.f - 0.1f, side_len / 2.f + 0.02f);
	glVertex3f(-side_len / 8.f, side_len / 2.f - 0.1f, side_len / 2.f + 0.02f);

	glEnd();
}

GLfloat HealthPack::SideLength(void)
{
	GLfloat side_len = 0.2f;
	switch(type)
	{
		case MEGA_HEALTH_PACK:
		{
			side_len += 0.2f;
			// Fall through...
		}
		case LARGE_HEALTH_PACK:
		{
			side_len += 0.2f;
			// Fall through...
		}
		case SMALL_HEALTH_PACK:
		{
			side_len += 0.2f;
			break;
		}
	}
	return side_len;
}

void HealthPack::Draw(fovi *f)
{
	GLfloat side_len = SideLength();	

	if(side_len > 0.f)
	{
		glPushMatrix();
		glTranslatef(x, y, 0.f);
		glRotatef(RADS_TO_DEGS(angle), 0.f, 0.f, 1.f);

		glColor3f(1.f, 0.f, 0.f);
		glutSolidCube(side_len);

		glRotatef(90.f, 1.f, 0.f, 0.f);	// Z-axis -> Y-axis.
		draw_white_cross(side_len);
		glRotatef(90.f, 0.f, 1.f, 0.f);
		draw_white_cross(side_len);
		glRotatef(90.f, 0.f, 1.f, 0.f);
		draw_white_cross(side_len);
		glRotatef(90.f, 0.f, 1.f, 0.f);
		draw_white_cross(side_len);

		glPopMatrix();
	}
}

// This routine is only called once during
// the map load, so I don't feel bad about
// calling the square root operation.
void HealthPack::CalcRadius(void)
{
	float side_len = SideLength();
	radius = sqrt(2.f) * side_len;
}

bool HealthPack::HandleAction(Entity *e)
{
	// Don't use up the health pack unless we really need it.
	if(e->health < 1.f)
	{
		UseOn(e);
		c->RemoveArtifact(this);
		StopJob(hp_job);
		return true;
	}
	return false;
}

bool HealthPack::UseOn(Entity *e)
{
	switch(type)
	{
		case LARGE_HEALTH_PACK:
		{
			e->health += 0.25f;
			// Fall through...
		}
		case SMALL_HEALTH_PACK:
		{
			e->health += 0.25f;
			if(e->health > 1.f)
				e->health = 1.f;
			break;
		}
		case MEGA_HEALTH_PACK:
		{
			e->health = 1.f;
			break;
		}
	}
	return true;
}

bool HealthPack::UseOn(Door *d)
{
	return false;
}

bool HealthPack::UseOn(Artifact *a)
{
	return false;
}

// endof healthpack.cpp
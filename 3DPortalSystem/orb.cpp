// orb.cpp

// Programmed by Spencer T. Parkin

#include <math.h>
#include "orb.h"
#include "main.h"
#include "macros.h"
#include "job.h"

// It doesn't hurt to call this routine
// even if the orb is not visible.
bool AnimateOrb(void *data)
{
	Orb *o = (Orb *)data;
	float da = (PI / 8.f) / FPS;	// Radians per frame.
	o->rot_x = MOD_2PI(o->rot_x + da);
	o->rot_y = MOD_2PI(o->rot_y + 2.f * da);
	o->rot_z = MOD_2PI(o->rot_z + 1.5f * da);
	return false;
}

Orb::Orb()
{
	radius = 0.8f;
	rot_x = rot_y = rot_z = 0.f;
	(void)StartJob(AnimateOrb, this);
}

Orb::~Orb()
{
}

// Here we override the default action of
// taking possetion of the orb and use this
// routine to indicate a completion of the level.
bool Orb::HandleAction(class Entity *e)
{
	level_complete = true;
	return true;
}

void Orb::Draw(fovi *f)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	GLfloat mat_amb_diff[4] = {0.f, 0.f, 0.f, 1.f};
	static GLfloat mat_spec[] = {0.8f, 0.8f, 0.8f, 1.f};
	static GLfloat mat_shin[] = {50.f};
	static GLfloat light_pos[] = {1.f, 1.f, 1.f, 0.f};
	static GLfloat light_amb[] = {0.4f, 0.4f, 0.4f, 1.f};
	static GLfloat light_diff[] = {1.f, 1.f, 1.f, 1.f};
	static GLfloat light_spec[] = {1.f, 1.f, 1.f, 1.f};
	
	switch(orb)
	{
		case ORB_OF_MOCHRIE:
		{
			mat_amb_diff[0] = 1.f;
			break;
		}
		case ORB_OF_STILES:
		{
			mat_amb_diff[1] = 1.f;
			break;
		}
		case ORB_OF_BRADY:
		{
			mat_amb_diff[2] = 1.f;
			break;
		}
		case ORB_OF_PROOPS:
		{
			mat_amb_diff[2] = 1.f;
			mat_amb_diff[1] = 1.f;
			break;
		}
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shin);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);

	glPushMatrix();
	glTranslatef(x, y, 0.f);
	glRotatef(RADS_TO_DEGS(rot_x), 1.f, 0.f, 0.f);
	glRotatef(RADS_TO_DEGS(rot_y), 0.f, 1.f, 0.f);
	glRotatef(RADS_TO_DEGS(rot_z), 0.f, 0.f, 1.f);

	switch(orb)
	{
		case ORB_OF_MOCHRIE:
		{
			glutSolidSphere(radius / 2.f, 10, 10);
			break;
		}
		case ORB_OF_STILES:
		{
			glutSolidTeapot(radius / 2.f);
			break;
		}
		case ORB_OF_PROOPS:
		{
			glutSolidTorus(radius / 5.f, radius / 2.f, 10, 10);
			break;
		}
		case ORB_OF_BRADY:
		{
			glutSolidIcosahedron();
			break;
		}
	}

	glPopMatrix();

	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
}

// endof orb.cpp
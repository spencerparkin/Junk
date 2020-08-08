// blob.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "blob.h"
#include "macros.h"
#include "cell.h"
#include "main.h"
#include "tether.h"

Blob::Blob()
{
	teth_to_target = 0;
	target_e = 0;
	spy_flag = false;
	sucking_life = false;
}

Blob::~Blob()
{
}

void Blob::Draw(fovi *f)
{
	glPushMatrix();
	glTranslatef(x, y, 0.f);
	glScalef(0.5f, 0.5f, 0.5f);

	glColor3f(0.f, 0.f, 0.f);
	glutSolidDodecahedron();

	float angle = this->f.heading;
	glTranslatef(1.5f * cos(angle), 1.5f * sin(angle), 0.f);
	glRotatef(RADS_TO_DEGS(angle), 0.f, 0.f, 1.f);
	glRotatef(90.f, 0.f, 1.f, 0.f);
	if(spy_flag)
	{
		if(sucking_life)
			glColor3f(0.f, 1.f, 0.f);
		else
			glColor3f(1.f, 0.f, 0.f);
	}
	else
		glColor3f(1.f, 1.f, 1.f);
	glutSolidTorus(0.2f, 0.5f, 5, 5);

	glPopMatrix();
}

void Blob::Drive(void)
{
	if(teth_to_target)
	{
		Portal *p = ReturnClosingPortal(teth_to_target);
		float target_x, target_y;
		if(p)
		{
			if(p->no_pass)
			{
				target_x = 0.f;
				target_y = 0.f;
			}
			else
			{
				target_x = (p->l.x0 + p->l.x1) / 2.f - 0.1f * cos(p->l.angle);
				target_y = (p->l.y0 + p->l.y1) / 2.f - 0.1f * sin(p->l.angle);
			}
		}
		else
		{
			target_x = target_e->x;
			target_y = target_e->y;
		}

		// If the blob is within close enough proximity
		// to the entity, then let it incur damage.
		sucking_life = false;
		if(cheat_mode == false && target_e->c == c)
		{
			float dx = target_e->x - x;
			float dy = target_e->y - y;
			float r = target_e->radius + radius;
			if(dx * dx + dy * dy <= r * r)
			{
				static const float dh = 30.f;	// In units of health percentage per second.
				target_e->health -= (dh / 100.f) / FPS;
				sucking_life = true;
			}
		}

		if(!sucking_life)
			(void)Navigate(target_x, target_y, 1.f, PI);
	}
	else
	{
		if(target_e)
			spy_flag = CanSee(target_e);

		if(spy_flag)
		{
			EstablishTether(&teth_to_target, this, target_e);
		}
		else
		{
			f.heading += PI / 12.f / FPS;
		}
	}
}

// If our tether gets deleted by the
// tether system, then we want to know
// about it so that we don't hang around
// with a pointer to freed memory.
void Blob::TetherBreakNotify(tether *teth)
{
	if(teth == teth_to_target)
	{
		teth_to_target = 0;
	}
}

// endof blob.cpp
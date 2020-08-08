// teleport.cpp

// Programmed by Spencer T. Parkin

#include "macros.h"
#include "wall.h"
#include "teleport.h"
#include "camera.h"
#include "tether.h"

Teleport::Teleport()
{
	c = 0;
	x = y = 0.f;
	angle = 0.f;
	target_c = 0;
}

Teleport::~Teleport()
{
}

bool Teleport::HandleAction(class Entity *e)
{
	if(!has_power || !target_c)
		return false;

	// Save the current camera mode
	// in case it's the 3rd person mode,
	// in which case the camera tether
	// will get broken.
	int cam_mode = cam->mode;

	// Break any tethers this entity is
	// connected to before transport.
	int k;
	for(k = 0; k < TETHERS_PER_ENTITY; k++)
		if(e->teth[k])
			DiscardTether(e->teth[k]);

	// Use molecular transport!
	e->c->ExitEntity(e);
	target_c->EnterEntity(e);
	e->x = target_x;
	e->y = target_y;
	e->f.heading = target_heading;

	// Restore the camera mode.
	cam->SwitchMode(cam_mode);

	return true;
}

// Draw the tele-pad as a lame circle on the floor.
void Teleport::Draw(fovi *f)
{
	glPushMatrix();
	glTranslatef(x, y, -WALL_HEIGHT / 2.f + 0.05f);
	glBegin(GL_TRIANGLE_FAN);
	if(has_power)
		glColor3f(0.f, 1.f, 0.f);
	else
		glColor3f(1.f, 0.f, 0.f);
	glVertex2f(0.f, 0.f);
	int k;
	const int num_tris = 20;
	float angle;
	for(k = 0; k <= num_tris; k++)
	{
		angle = float(k) / float(num_tris) * 2.f * PI;
		glVertex2f(radius * cos(angle), radius * sin(angle));
	}
	glEnd();
	glPopMatrix();
}

// Teleports are never inventoried.
void Teleport::DrawInventoryItem(void)
{
}

// endof teleport.cpp
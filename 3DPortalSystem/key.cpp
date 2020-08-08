// key.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "key.h"
#include "cell.h"
#include "door.h"
#include "macros.h"
#include "camera.h"

Key::Key()
{
	radius = 1.f;
	dpy_list = 0;
}

Key::~Key()
{
}

// I could have used the tesselation library
// here, but this will do fine.  Draw the
// key in the XZ-plane.
static void DrawKey(void)
{
	glBegin(GL_QUADS);

	glVertex3f(0.f, 0.f, -2.f);
	glVertex3f(1.f, 0.f, -2.f);
	glVertex3f(1.f, 0.f, 2.f);
	glVertex3f(0.f, 0.f, 2.f);

	glVertex3f(-2.f, 0.f, -3.f);
	glVertex3f(1.f, 0.f, -3.f);
	glVertex3f(1.f, 0.f, -2.f);
	glVertex3f(-2.f, 0.f, -2.f);

	glVertex3f(-2.f, 0.f, -1.f);
	glVertex3f(0.f, 0.f, -1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(-2.f, 0.f, 0.f);

	glVertex3f(-1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 3.f);
	glVertex3f(-1.f, 0.f, 3.f);

	glVertex3f(1.f, 0.f, 1.f);
	glVertex3f(2.f, 0.f, 1.f);
	glVertex3f(2.f, 0.f, 3.f);
	glVertex3f(1.f, 0.f, 3.f);

	glVertex3f(-1.f, 0.f, 3.f);
	glVertex3f(2.f, 0.f, 3.f);
	glVertex3f(2.f, 0.f, 4.f);
	glVertex3f(-1.f, 0.f, 4.f);

	glEnd();
}

void Key::Draw(fovi *f)
{
	if(cam->mode == CAM_MODE_TOPOLOGICAL)
	{
		Artifact::Draw(f);
		return;
	}

	glPushMatrix();
	glTranslatef(x, y, 0.f);
	float angle = cam->f.heading + PI / 2.f + c->angle;
	glRotatef(RADS_TO_DEGS(angle), 0.f, 0.f, 1.f);
	glScalef(0.2f, 0.2f, 0.2f);
	glColor3f(r, g, b);
	if(dpy_list)
		glCallList(dpy_list);
	else
	{
		dpy_list = glGenLists(1);
		glNewList(dpy_list, GL_COMPILE_AND_EXECUTE);
		DrawKey();
		glEndList();
	}
//	DrawKey();
	glPopMatrix();
}

// Draw this key as it would appear in the HUD.
void Key::DrawInventoryItem(void)
{
	//...
}

// Nothing happens when you use a key on an entity.
bool Key::UseOn(Entity *e)
{
	return false;
}

// Try to unlock the door if it's locked.
// No support here is provided for relocking doors.
bool Key::UseOn(Door *d)
{
	// Does this key open the given door?
	if(d->is_locked && (this->d == d || this->d == d->d))
	{
		d->UnlockDoor();
		return true;
	}

	return false;
}

// Nothing happens when you use a key on an artifact.
// The object here is too generic anyway.  If there
// will be artifacts used on artifacts, then I'll need
// to make the argument more specific.
bool Key::UseOn(Artifact *a)
{
	return false;
}

// endof key.cpp
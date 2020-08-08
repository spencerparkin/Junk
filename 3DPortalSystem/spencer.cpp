// spencer.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "spencer.h"
#include "cell.h"
#include "macros.h"
#include "camera.h"
#include "main.h"
#include "wall.h"

Spencer *spencer = 0;

Spencer::Spencer()
{
	// These should get loaded in the map.
	t_walk[0] = 0;
	t_walk[1] = 0;
	t_walk[2] = 0;
	t_walk[3] = 0;
	t_stand = 0;
	prev_x = prev_y = 0.f;
	anim_frame = 0.f;
}

Spencer::~Spencer()
{
}

// I'm drawn only if the camera is
// not in 1st person mode.
void Spencer::Draw(fovi *f)
{
	glLineWidth(2.f);
	glBegin(GL_LINES);
	glColor3f(0.f, 0.f, 0.f);
	glVertex3f(x, y, -WALL_HEIGHT / 2.f + 0.1f);
	glVertex3f(x + cos(this->f.heading), y + sin(this->f.heading), -WALL_HEIGHT / 2.f + 0.1f);
	glEnd();
	glLineWidth(1.f);

	if(cam->mode != CAM_MODE_1ST_PERSON)
		Entity::Draw(f);
}

// Normally the user-input would be read
// and dealt with from this routine, but
// since we're using an event-driven callback
// model thingy using GLUT, all the code to
// control this entity is in main.cpp.  So
// here I'm putting the animation code.
void Spencer::Drive(void)
{
	// This code is pretty cheesy, but it works okay.
	if(prev_x == x && prev_y == y && anim_frame == 0.f)
		t = t_stand;
	else
	{		
		int index = int(anim_frame) % 4;
		t = t_walk[index];
		static const float walk_fps = 3.f;	// Go at 3 walk-animation-frames per second.
		anim_frame += walk_fps / FPS;
		if(anim_frame >= 4.f)
			anim_frame = 0.f;
		prev_x = x;
		prev_y = y;
	}
}

// endof spencer.cpp
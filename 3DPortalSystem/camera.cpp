// camera.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include "macros.h"
#include "camera.h"
#include "cell.h"
#include "map.h"
#include "main.h"
#include "portal.h"
#include "tether.h"

Camera *cam = 0;
float aspect_ratio = 1.f;

Camera::Camera()
{
	cam_teth = 0;
}

Camera::~Camera()
{
}

// This routine will draw the scene
// from the camera's perspective.
// It's useful to define the camera as a type of
// entity, because it inherits all of the
// nice collision-detection and visible-
// surfice-determination support which the
// camera will need in a 1st or 3rd person mode.
void Camera::Draw(fovi *f)
{
	// Ignore the given fovi and use our own.
	f = &this->f;

	if(cell_mode == DYNAMIC_CELL_MODE)
	{
		c->x = 0.f;
		c->y = 0.f;
		c->angle = 0.f;
	}

	if(mode == CAM_MODE_TOPOLOGICAL)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		c->LtoG(&f->x, &f->y, x, y);
		GLfloat x_center = f->x;
		GLfloat y_center = f->y;
		GLfloat dx = 20.f;
		GLfloat dy = 20.f;

		if(aspect_ratio > 1.f)
			dx *= aspect_ratio;
		else
			dy /= aspect_ratio;

		gluOrtho2D(x_center - dx, x_center + dx,
					y_center - dy, y_center + dy);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();	

		glColor3f(1.f, 1.f, 1.f);
		draw_fovi(f, radius, -0.1f);
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
//		gluPerspective(RADS_TO_DEGS(2.f * f->angle), aspect_ratio, f->radius / 2.f, 1000.f);
		gluPerspective(60.f, aspect_ratio, 0.1f, 1000.f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		c->LtoG(&f->x, &f->y, x, y);
		gluLookAt(f->x, f->y, 0.f,
					f->x + cos(f->heading),
					f->y + sin(f->heading), 0.f,
					0.f, 0.f, 1.f);
	}

	// Draw the scene!
	c->Draw(f);
}

void Camera::SwitchMode(int mode)
{
	// No reason to switch to the
	// mode we're already in.
	if(this->mode == mode)
		return;

	// Deinitialize the old mode.
	switch(this->mode)
	{
		case CAM_MODE_TOPOLOGICAL:
		case CAM_MODE_1ST_PERSON:
		{
			break;
		}
		case CAM_MODE_3RD_PERSON:
		{
			if(cam_teth)
				DiscardTether(cam_teth);
			break;
		}
	}

	// Change modes.
	this->mode = mode;

	// For any mode, make sure that the
	// camera is in the same cell as its
	// subject entity.
	if(c != e->c)
	{
		c->ExitEntity(this);
		e->c->EnterEntity(this);
		x = e->x;
		y = e->y;
		f.heading = e->f.heading;
	}

	// Initialize the new mode.
	switch(mode)
	{
		case CAM_MODE_TOPOLOGICAL:
		case CAM_MODE_1ST_PERSON:
		{
			break;
		}
		case CAM_MODE_3RD_PERSON:
		{
			// Establish a tether between the camera
			// and the subject entity.  Note that the
			// camera here is the source, and the
			// subject entity is the destination.
			if(!EstablishTether(&cam_teth, this, e))
			{
				// I'm not too happy about the idea of
				// this being recursive, but oh well.
				SwitchMode(CAM_MODE_1ST_PERSON);
				return;
			}
			break;
		}
	}

	// Barf if we have no entity to look at.
	if(!e)
		fatal_error("Camera has no subject entity!");
}

// This routine is responsible for orienting
// the camera in way that avoids the camera's
// view being obscured by surfaces and other objects.
void Camera::Drive(void)
{
	switch(mode)
	{
		case CAM_MODE_TOPOLOGICAL:
		case CAM_MODE_1ST_PERSON:
		{
			// Match the subject's position and heading.
			if(c != e->c)
			{
				c->ExitEntity(this);
				e->c->EnterEntity(this);
			}
			x = e->x;
			y = e->y;
			f.heading = e->f.heading;
			break;
		}
		case CAM_MODE_3RD_PERSON:
		{
			// If something went wrong and we
			// don't have a tether, then switch
			// back to 1st person mode.
			if(!cam_teth)
			{
				SwitchMode(CAM_MODE_1ST_PERSON);
				break;
			}

			// Get the subject's position and orientation
			// with respect to the cell containing the camera.
			float subj_x, subj_y;
			float subj_heading;
			CalcTetherCoords(cam_teth, DST_WRT_SRC, &subj_x, &subj_y, &subj_heading);

			// Calculate where we want the camera to
			// be and move the camera partly there to
			// give us a smooth animation.  Smoooooth.
			float target_x = subj_x - CAM_FOLLOW_DIST * cos(subj_heading);
			float target_y = subj_y - CAM_FOLLOW_DIST * sin(subj_heading);
			float dx = target_x - x;
			float dy = target_y - y;
			Move(dx * 4.f / FPS, dy * 4.f / FPS);

			// Make sure the camera is always facing the subject.
			f.heading = MOD_2PI(quadrant_angle(subj_x - x, subj_y - y));

			break;
		}
	}

	// Update the camera's field of vision.
	f.angle = PI / 6.f * aspect_ratio;
}

// The camera is never visible.
// If it was, then we would have
// infinite recursion on our hands.
bool Camera::IsVisible(fovi *f)
{
	// I want the camera to be visible to other entities.
	// A fix for the infinite recursion was put in cell.cc.
//	return false;	
	return Entity::IsVisible(f);
}

void Camera::TetherBreakNotify(struct tether *teth)
{
	if(teth == cam_teth)
	{
		// We can null this pointer without
		// freeing the tether because the
		// notify routine is telling us that
		// someone else is freeing the tether.
		// We need to nullify it here so that
		// we don't try to delete it when we
		// switch camera modes.
		cam_teth = 0;
		SwitchMode(CAM_MODE_1ST_PERSON);
	}
}

// endof camera.cpp
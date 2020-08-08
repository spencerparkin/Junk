// hud.cpp

// Programmed by Spencer T. Parkin

#define _CRT_TERMINATE_DEFINED

#include <glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hud.h"
#include "camera.h"
#include "entity.h"
#include "main.h"

Entity *hud_e = 0;

bool InitHUD(Entity *e)
{
	// Just so you know, I tried to use
	// GLUT's overlay support, but the windows
	// version of GLUT doesn't support it yet.
//	if(!glutLayerGet(GLUT_OVERLAY_POSSIBLE))
//		return false;

	hud_e = e;
	return true;
}

// If we needed to deinitize anything,
// then we could do it here.
bool DeinitHUD(void)
{
	return true;
}

static void draw_string(float x, float y, void *font, char *string)
{
	glRasterPos2f(x, y);
	int k;
	for(k = 0; string[k] != '\0'; k++)
		glutBitmapCharacter(font, (int)string[k]);
}

void DrawHUD(void)
{
	if(!hud_e || cam->mode == CAM_MODE_TOPOLOGICAL)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat dx = 10.f;		// How wide our window is.
	GLfloat dy = 10.f;		// How tall our window is.

	// Account for the aspect ratio of the screen.
	if(aspect_ratio > 1.f)
		dx *= aspect_ratio;
	else
		dy /= aspect_ratio;

	GLfloat x_min = -dx / 2.f;
	GLfloat x_max = -x_min;
	GLfloat y_min = -dx / 2.f;
	GLfloat y_max = -y_min;

	gluOrtho2D(x_min, x_max, y_min, y_max);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	char string[128];
	sprintf(string, "FPS = %2.1f", FPS);
	glColor3f(1.f, 1.f, 1.f);
	draw_string(x_min + 0.1f, y_max - 1.f, GLUT_BITMAP_TIMES_ROMAN_24, string);

	// Draw the health bar at the bottom
	// of the user's screen.
	float hb_x0 = x_min + 0.1f;
	float hb_x1 = x_max - 0.1f;
	float hb_x = hb_x0 * (1.f - hud_e->health) + hb_x1 * hud_e->health;
	glBegin(GL_QUADS);
	if(cheat_mode)
	{
		glColor3f(0.f, 1.f, 0.f);
		hb_x = hb_x1;
	}
	else if(hud_e->health > 0.5f)
		glColor3f(1.f, 1.f, 1.f);
	else if(hud_e->health > 0.2f)
		glColor3f(1.f, 1.f, 0.f);
	else
		glColor3f(1.f, 0.f, 0.f);
	glVertex2f(hb_x0, y_min + 0.1f);
	glVertex2f(hb_x, y_min + 0.1f);
	glVertex2f(hb_x, y_min + 0.5f);
	glVertex2f(hb_x0, y_min + 0.5f);
	glEnd();
}

// endof hud.cpp
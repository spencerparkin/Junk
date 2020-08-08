// main.cpp

// Programmed by Spencer T. Parkin

#define _CRT_TERMINATE_DEFINED

#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "macros.h"
#include "main.h"
#include "map.h"
#include "texture.h"
#include "camera.h"
#include "job.h"
#include "random.h"
#include "hud.h"
#include "spencer.h"
#include "tether.h"

float FPS = 60.f;
int frame_counter = 0;
bool exit_game = false;
bool level_complete = true;
bool cheat_mode = false;		// Lets use unlock any door and do other stuff.
bool draw_all = false;		// Overrides VSD in drawing algorithm.
int cur_level = -1;

// This is a pointer to the entity controlled by the user interface.
static Entity *e = 0;

// Start here!
int main(int argc, char **argv)
{
	// Initialize GLUT.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(300, 300);
	glutCreateWindow(argv[0]);
	glutFullScreen();

	// Setup some OpenGL state.
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	// Initialize the random number generator.
	InitRandom(1);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);
	glutIdleFunc(idle);

	// Enter the event processing loop.
	glutMainLoop();

	return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// I don't think it's as nice to calculate
	// the FPS only once every second.  I would
	// like to calculate the FPS once per frame,
	// but I need a more accurate timer for that.
	static clock_t t = clock();
	clock_t elapsed_cycles = clock() - t;
	if(float(elapsed_cycles) / float(CLOCKS_PER_SEC) >= 1.f)
	{
		FPS = float(frame_counter);
		frame_counter = 0;
		t = clock();
//		printf("FPS = %f\n", FPS);
	}

	// Draw the scene.
	if(cam)
		cam->Draw(0);

	// Draw the heads-up-display.
	DrawHUD();

	// Swap double buffers and bump the frame count.
	glutSwapBuffers();
	ManageTextures();
	frame_counter++;
}

// Resize the viewport accordingly and
// calculate the window's aspect ratio.
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	if(h == 0)
		aspect_ratio = 0.f;
	else
		aspect_ratio = GLfloat(w) / GLfloat(h);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case ' ':
		{
			e->Act();
			break;
		}
		case 27:	// Esc-key.
		{
			exit_game = true;
			break;
		}
		case 'o':
		case 'O':
		{			
			if(cam->mode == CAM_MODE_TOPOLOGICAL)
				cam->SwitchMode(CAM_MODE_1ST_PERSON);
			else
				cam->SwitchMode(CAM_MODE_TOPOLOGICAL);
			break;
		}
		case 'q':
		case 'Q':	// This is mainly for debugging purposes.
		{
			if(cheat_mode)
			{
				cheat_mode = false;
				printf("cheat mode: OFF\n");
			}
			else
			{
				cheat_mode = true;
				printf("cheat mode: ON\n");
			}
			break;
		}
		case 'c':
		case 'C':
		{
			if(cam->mode == CAM_MODE_1ST_PERSON)
				cam->SwitchMode(CAM_MODE_3RD_PERSON);
			else
				cam->SwitchMode(CAM_MODE_1ST_PERSON);
			break;
		}
		case 'a':
		case 'A':
		{
			if(draw_all)
				draw_all = false;
			else
				draw_all = true;
			break;
		}
	}
}

void special(int key, int x, int y)
{
	int modifiers = glutGetModifiers();

	// In this mode the keyboard controls
	// the camera completely.
	switch(key)
	{
		case GLUT_KEY_LEFT:
		{
			if(modifiers & GLUT_ACTIVE_ALT)
				e->Strafe(-0.1f);
			else
				e->Turn(PI / 32.f);
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			if(modifiers & GLUT_ACTIVE_ALT)
				e->Strafe(0.1f);
			else
				e->Turn(-PI / 32.f);
			break;
		}
		case GLUT_KEY_UP:
		{
			e->MoveForward(0.1f);
			break;
		}
		case GLUT_KEY_DOWN:
		{
			e->MoveForward(-0.1f);
			break;
		}
		case GLUT_KEY_F1:
		{
			level_complete = true;
			break;
		}
	}
}

static int mx, my;
static int button;

void mouse(int button, int state, int x, int y)
{
	switch(button)
	{
		case GLUT_LEFT_BUTTON:
		{
			mx = x;
			my = y;
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			if(state == GLUT_DOWN)
				e->Act();
			break;
		}
	}

	::button = button;
}

void motion(int x, int y)
{
	if(button == GLUT_LEFT_BUTTON)
	{
		int dx = x - mx;
		int dy = y - my;
		e->Turn(-float(dx) / 200.f);
		e->MoveForward(-float(dy) / 40.f);	
		mx = x;
		my = y;
	}
}

void passive(int x, int y)
{
}

void idle(void)
{
	// Monitor the health of the user controlled entity.
	// If they die, then the game is over.
	if(e && e->health <= 0.f)
		exit_game = true;

	// Has the user completed the currently loaded level,
	// or does the user want to get the heck out of here?
	if(level_complete || exit_game)
	{
		// Reset this flag now so that we don't
		// forget to do it later.
		level_complete = false;

		// If there was a level loaded, clean it up.
		if(cur_level >= 0)
		{
			DeinitHUD();
			DeinitTetherSystem();
			UnloadMap(game_map[cur_level]);
			DeinitTextureManager();
			DeinitJobFacility();
			glDeleteLists(1, 100);
		}

		// At this point, exit if that's what the user wanted.
		if(exit_game)
			exit(0);

		// Bump level count.
		cur_level++;

		// Load the next level if there is one.
		if(cur_level < num_game_maps)
		{
			InitTetherSystem();
			InitJobFacility();
			InitTextureManager();
			InitEntityBuffer();
			cam = 0;
			spencer = 0;
			LoadMap(game_map[cur_level]);

			// Assign the entity that the keyboard/mouse will control.
			e = spencer;
			if(!e)
				fatal_error("No main character to play!");

			// Assign the entity that the HUD will display info about.
			InitHUD(e);

			// Setup the camera.
			if(!cam)
				fatal_error("No camera entity!");
			cam->SwitchMode(CAM_MODE_1ST_PERSON);
		}
		else
		{
			// Initiate final game sequence here.

			// For now, just exit.
			exit(0);
		}
	}
	else	// Run the level...
	{
		RunJobs();
		DriveEntities();
		glutPostRedisplay();
	}
}

// Ah, the fatal error routine.
void fatal_error(char *error_str, int exit_code)
{
	fprintf(stderr, "FATAL ERROR: %s\n", error_str);
	fprintf(stderr, "Press enter to get on with your life...\n");
	getchar();
	exit(exit_code);
}

// endof main.cpp
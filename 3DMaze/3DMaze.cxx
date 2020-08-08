// 3DMaze.cxx - Generate 3D Mazes - By Spencer T. Parkin

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Node.h"
#include "Skeleton.h"
#include "Tube.h"
#include "Field.h"

#if 0		// Don't bother to look at anything in this #if 0 block.

int w, h;
GLfloat FoviAngle = 60.0;
GLfloat LookAngle = 90.0;
Node *Container;
GLfloat Speed = 2.f;
vec3d Position = {0.f, 0.f, 0.f};	// Relative to the containing node
vec3d Velocity = {Speed * cos(LookAngle), Speed * sin(LookAngle), 0.f};
vec3d Acceleration;
vec3d Gravity = {0.f, 0.f, -0.5f};
Maze *M;

// GLUT callbacks
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle(void);

main(int argc, char **argv)
{
	// Init GLUT.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow(argv[0]);

	// Init OpenGL.
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	// Make the projection matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Make the modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Create the maze.
	M = new CorridorMaze();
	M->Generate();
	M->PatchNavigationLinks();

	// Setup player.
	Container = &((CorridorMaze *)M)->Matrix[0][0];
	vec3d_set(&Position, 0.f, 0.f, 0.f);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutIdleFunc(idle);

	// Enter message loop.
	glutMainLoop();

	return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Give us a 1st person view.

	vec3d CamPos, CamLookAt;
	vec3d CamUp = {0.f, 0.f, 1.f};
	vec3d_add(&CamPos, &Container->Pos, &Position);
	vec3d_set(&CamLookAt, cos(LookAngle), sin(LookAngle), 0.f);
	vec3d_add(&CamLookAt, &CamLookAt, &CamPos);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(FoviAngle, GLfloat(w) / GLfloat(h), 1.f, 1000.f);
	gluLookAt(CamPos.x, CamPos.y, CamPos.z,
				CamLookAt.x, CamLookAt.y, CamLookAt.z,
				CamUp.x, CamUp.y, CamUp.z);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glBegin(GL_QUADS);

#if 1	// Draw everything.

	int i, j;
	for(i = 0; i < X_NODES; i++)
		for(j = 0; j < Y_NODES; j++)
			((CorridorMaze *)M)->Matrix[i][j].DrawWalls();

#else	// Draw only what you can see.

	Field Vision;
	MakeField(&Vision, LookAngle, FoviAngle);
	Container->DrawWalls(Vision, &CamPos);

#endif

	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	// We'll need these to compensate for
	// a changine aspect ratio.
	::w = w;
	::h = h;
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case ' ':
		{
			break;
		}
		case '+':
		case '=':
		{
			Position.z += 2.f;
			break;
		}
		case '-':
		case '_':
		{
			Position.z -= 2.f;
			break;
		}
	}

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	// make sure to update container here...
	switch(key)
	{
		case GLUT_KEY_UP:
		{
			Position.x += Speed * cos(LookAngle);
			Position.y += Speed * sin(LookAngle);
			break;
		}
		case GLUT_KEY_DOWN:
		{
			Position.x -= Speed * cos(LookAngle);
			Position.y -= Speed * sin(LookAngle);
			break;
		}
		case GLUT_KEY_LEFT:
		{
			LookAngle += PI / 32.f;
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			LookAngle -= PI / 32.f;
			break;
		}
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
}

void motion(int x, int y)
{
}

void idle(void)
{
}

#else	// Okay, here is where the program begins...

GLfloat RotX = 0.f;
GLfloat RotY = 0.f;
GLfloat RotZ = 0.f;

GLfloat Zoom = -60.f;

int mouseX, mouseY;

Maze *M = 0;

int DrawTubes = 0;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0.f, 0.f, Zoom);
	glRotatef(RotX, 1.f, 0.f, 0.f);
	glRotatef(RotY, 0.f, 1.f, 0.f);
	glRotatef(RotZ, 0.f, 0.f, 1.f);

	if(M)
	{
		M->Render(DrawTubes);
	}

	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.f, GLfloat(w) / GLfloat(h), 1.f, 1000.f);
	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void motion(int x, int y)
{
	RotX += GLfloat(y - mouseY);
	RotY += GLfloat(x - mouseX);

	mouseX = x;
	mouseY = y;

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if(M)
	{
		switch(key)
		{
			case 'x':
			case 'X':
			{
				glutSetWindowTitle("3D Maze Generator - By Spencer T. Parkin");
				delete M;
				M = 0;
				break;
			}
			case ' ':
			{
				(void)ProcessNodeWithPotential();
				break;
			}
			case '`':
			{
				while(ProcessNodeWithPotential()){}
				break;
			}
			case 'l':
			case 'L':
			{
				if(DrawTubes)
				{
					glDisable(GL_LIGHTING);
					glShadeModel(GL_FLAT);
				}
				else
				{
					glEnable(GL_LIGHTING);
					glShadeModel(GL_SMOOTH);
				}

				DrawTubes = 1 - DrawTubes;
				break;
			}
			case '[':
			{
				Zoom -= 1.5f;
				break;
			}
			case ']':
			{
				Zoom += 1.5f;
				break;
			}
			case 'n':
			case 'N':
			{
				srand(unsigned(time(NULL)));
				M->PatchNavigationLinks();
				M->Navigate();
				break;
			}
			default:
			{
				return;
			}
		}
	}
	else
	{
		switch(key)
		{
			case 'c':
			{
				glutSetWindowTitle("Solid Cube Maze");
				M = new SolidCubeMaze(11, 11, 11);
				break;
			}
			case 'C':
			{
				glutSetWindowTitle("Solid Square Maze");
				M = new SolidCubeMaze(21, 21, 2);
				break;
			}
			case 'i':
			{
				glutSetWindowTitle("Tower Maze");
				M = new SolidCubeMaze(7, 30, 7);
				break;
			}
			case 't':
			{
				glutSetWindowTitle("Hollow Torus Maze (Skinny donut)");
 				M = new HollowTorusMaze(12.f, 2.f, 120, 30);
				break;
			}
			case 'T':
			{
				glutSetWindowTitle("Hollow Torus Maze (Fat donut)");
 				M = new HollowTorusMaze(6.f, 5.f, 60, 45);
				break;
			}
			case 's':
			{
				// sphere maze with longitude and latitude lines
				break;
			}
			case 'S':
			{
				// sphere maze made from an icosahedron
				break;
			}
			case 'h':
			case 'H':
			{
				glutSetWindowTitle("Flat Hexigon Maze");
				M = new HexigonMaze(50);
				break;
			}
			case 'u':
			case 'U':
			{
				// cylinder maze (cap the bottom and the top with circle mazes)
				break;
			}
			case 'q':
			case 'Q':
			{
				// 2D circle maze
				break;
			}
			case 'm':
			case 'M':
			{
				// 2D maze (feature bubble optimization is navigating algorithm?)
				glutSetWindowTitle("Flat Maze");
				M = new SolidCubeMaze(61, 61, 1);
				break;
			}
			default:
			{
				return;
			}
		}

		srand(unsigned(time(NULL)));

		// This just gets the process started.
		M->Generate();
	}

	glutPostRedisplay();
}

void print_key_map(void)
{
	printf("KEY MAP\n");
	printf("==========================\n");
	printf("c: Make cube maze\n");
	printf("C: Make square maze\n");
	printf("i: Make tower maze\n");
	printf("t: Make skinny donut maze\n");
	printf("T: Make fat donut maze\n");
	printf("h: Make hexigon maze\n");
	printf("m: Make flat maze\n");
	printf("x: Destroy maze\n");
	printf("`: Finish maze generation\n");
	printf("l: Toggle tubes\n");
	printf("[]: Zoom in/out\n");
	printf("n: Navigate maze\n");
}

void idle(void)
{
	if(M)
	{
		(void)ProcessNodeWithPotential();
		(void)ProcessNodeWithPotential();
		(void)ProcessNodeWithPotential();
		glutPostRedisplay();
	}
}

main(int argc, char **argv)
{
	print_key_map();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
//	glutCreateWindow(argv[0]);
	glutCreateWindow("3D Maze Generator - By Spencer T. Parkin");

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLineWidth(1.2f);

	GLfloat MatSpec[] = {1.f, 1.f, 1.f, 1.f};
	GLfloat MatShine[] = {50.f};
	GLfloat LightPos[] = {1.f, 1.f, 1.f, 0.f};

	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, MatShine);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	return(0);
}

#endif
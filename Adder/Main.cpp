// main.cpp

// This is a quick-and-dirty digital circuit
// similuation program that can add binary numbers.
// The program has the potential to simulate
// many other types of combinatorial circuits.
// Circuits with circular dependencies, however,
// I'm not so sure about.  That would require
// a little more work, but I'm sure it wouldn't
// be too difficult to simulate those too.

#include <GL/glut.h>
#include <stdio.h>
#include "Adder.h"

// Our window onto the XY-plane.
GLfloat x_min, x_max;
GLfloat y_min, y_max;

// Used to keep our window from going skee-wompas.
GLfloat aspect_ratio;

// Flag to run/not-run the circuit.
int run_adder_circuit = 0;

// GLUT callback prototypes:
void display(void);
void reshape(int w, int h);
void idle(void);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);

int main(int argc, char **argv)
{
	x_min = -10.f;
	x_max = -x_min;
	y_min = -10.f;
	y_max = -y_min;

	CreateAdderCircuits(4, 0x0, 0x1);

	// Initialize GLUT.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	// Setup OpenGL state.
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glShadeModel(GL_FLAT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);

	// Enter the even dispatch loop.
	glutMainLoop();

	return 0;
}

void SetupProjectionMatrix(GLenum mode, int mouse_x, int mouse_y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(mode == GL_SELECT)
	{
		GLint Viewport[4];
		glGetIntegerv(GL_VIEWPORT, Viewport);
		gluPickMatrix(GLdouble(mouse_x), GLdouble(Viewport[3] - mouse_y), 3, 3, Viewport);
	}

	// This assumes that x_min = -x_max and that y_min = -y_max.
	if(aspect_ratio > 1.f)
		gluOrtho2D(x_min * aspect_ratio, x_max * aspect_ratio, y_min, y_max);
	else
		gluOrtho2D(x_min, x_max, y_min / aspect_ratio, y_max / aspect_ratio);

	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	SetupProjectionMatrix(GL_RENDER, 0, 0);
	DrawAdderCircuits();
	glutSwapBuffers();
}

// Readjust the viewport and calculate
// the aspect ratio.
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	aspect_ratio = (h == 0) ? 0.f : (GLfloat(w) / GLfloat(h));
}

void idle(void)
{
	if(run_adder_circuit)
		IterateAdderComponents();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case ' ':
		{
			run_adder_circuit = 1 - run_adder_circuit;
			if(run_adder_circuit)
				printf("adder circuit is running...\n");
			else
				printf("adder circuit is stopped!\n");
			break;
		}
		case 'q':
		case 'Q':
		{
			WipeAdderCircuits();
			break;
		}
		case '[':
		{
			x_min -= 1.5f;
			x_max += 1.5f;
			y_min -= 1.5f;
			y_max += 1.5f;
			break;
		}
		case ']':
		{
			x_min += 1.5f;
			x_max -= 1.5f;
			y_min += 1.5f;
			y_max -= 1.5f;
			break;
		}
		case 'r':
		case 'R':
		{
			if(run_adder_circuit)
				UseResult();
			break;
		}
		case '1': CreateAdderCircuits(1, 0, 1); break;
		case '2': CreateAdderCircuits(2, 0, 1); break;
		case '3': CreateAdderCircuits(3, 0, 1); break;
		case '4': CreateAdderCircuits(4, 0, 1); break;
		case '5': CreateAdderCircuits(5, 0, 1); break;
		case '6': CreateAdderCircuits(6, 0, 1); break;
		case '7': CreateAdderCircuits(7, 0, 1); break;
		case '8': CreateAdderCircuits(8, 0, 1); break;
		case '9': CreateAdderCircuits(9, 0, 1); break;
		case 'x':
		case 'X':
		{
			DestroyAdderCircuits();
			break;
		}
	}

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
		{
			y_min += 1.5f;
			y_max += 1.5f;
			break;
		}
		case GLUT_KEY_DOWN:
		{
			y_min -= 1.5f;
			y_max -= 1.5f;
			break;
		}
		case GLUT_KEY_LEFT:
		{
			x_min -= 1.5f;
			x_max -= 1.5f;
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			x_min += 1.5f;
			x_max += 1.5f;
			break;
		}
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	switch(button)
	{
		case GLUT_LEFT_BUTTON:
		{
			if(state == GLUT_DOWN)
				TweakAdderGate(x, y);
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			break;
		}
	}

	glutPostRedisplay();
}

// Rotate the given point CCW about the origin
// by the given angle.  This is easily done
// by multilying the point by an appropriate
// rotational transformation complex number.
void RotatePoint(GLfloat *x, GLfloat *y, GLfloat angle)
{
	GLfloat re = cos(angle);
	GLfloat im = sin(angle);

	//   (a + bi)(c + di)
	// = ac + adi + bci + bdi2
	// = (ac - bd) + (ad + bc)i

	GLfloat tmp = *x;
	*x = *x * re - *y * im;
	*y = tmp * im + *y * re;
}

// endof main.cpp
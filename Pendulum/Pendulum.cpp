// pendulum.cc

// 3D motion of the pendulum can be acheived
// by working two projections of the 2D motion
// of the pendulum in two seperate and orthogonal
// planes.  These planes will be the XZ and YZ planes.
// So all of the calculations will deal with
// the 2D pendulum simulation, though our program
// will be capable of simulated 3D motion of the pendulum.

#define FOR_WIN

#ifdef FOR_PS2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/psx2mesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <3dmath.h>
#include <Controller.h>
#endif
#ifdef FOR_WIN
#include <GL/glut.h>
//#include <math.h>
#define PI			3.1415926536
#define DEGS(x)		((x) * (180.0 / PI))
#define SIGN(x)		((x) < 0 ? -1.0 : 1.0)
#endif

#ifdef FOR_PS2
PSX2MesaContext Context;
CB_Buffer cbuf;
#endif /* FOR_PS2 */

GLfloat AngleX = 0.0, AngleY = 0.0;
GLfloat DaglX = 0.0, DaglY = 0.0;
GLfloat D2aglX = 0.0, D2aglY = 0.0;
GLfloat Length = 5.0, Mass = 1.0;
GLfloat Zoom = -10.f;
GLfloat dt = 0.3f;
GLfloat XaglDrag = 1.f, YaglDrag = 1.f;
GLfloat XaglForce = 0.0, YaglForce = 0.0;

#ifdef FOR_PS2
vec Gravity = {0.f, 0.f, -9.8};
#endif
#ifdef FOR_WIN
double Gravity = -9.8;
#endif

int StickCB(float x0, float y0, float x1, float y1);
int ButtonCB(int Button, int State);
void DrawPendulum(void);
void DrivePendulum(void);

#ifdef FOR_WIN
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void display(void);
void idle(void);
#endif

main(int argc, char **argv)
{
#ifdef FOR_PS2
	int x_res, y_res;

	x_res = 640;
	y_res = 480;

	PSX2MesaInit(1, 1, PS2_GS_NOINTERLACE, PS2_GS_VESA,
						PS2_GS_FRAME, PS2_GS_640x480,
						PS2_GS_60Hz, &x_res, &y_res);

	Context = PSX2MesaCreateContext(0, NULL);
	PSX2MesaMakeCurrent(Context);
	
	glViewport(0, 0, x_res, y_res);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.f, GLfloat(x_res) / GLfloat(y_res), 1.f, 1000.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#endif
#ifdef FOR_WIN
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutCreateWindow(argv[0]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glLineWidth(2.f);
	glEnable(GL_DEPTH_TEST);

#ifdef FOR_PS2
	InitController();
	WipeCallbackBuffer(&cbuf);
	RegisterJoyStickCallback(&cbuf, 0.2f, StickCB);
	RegisterButtonCallback(&cbuf, X_BUTTON, BUTTON_ON_PRESS, ButtonCB);
	RegisterButtonCallback(&cbuf, CIRCLE_BUTTON, BUTTON_ON_PRESS, ButtonCB);
	RegisterButtonCallback(&cbuf, L1_BUTTON, BUTTON_IS_DOWN, ButtonCB);
	RegisterButtonCallback(&cbuf, R1_BUTTON, BUTTON_IS_DOWN, ButtonCB);
	RegisterButtonCallback(&cbuf, L2_BUTTON, BUTTON_IS_DOWN, ButtonCB);
	RegisterButtonCallback(&cbuf, R2_BUTTON, BUTTON_IS_DOWN, ButtonCB);

	do
	{
		PullForControllerData();
		HandleControllerCallbacks(CON_DEV_0, &cbuf);
	
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		DrawPendulum();
		DrivePendulum();
		PSX2MesaSwapBuffers();
	}
	while(ButtonState(CON_DEV_0, SELECT_BUTTON) != BUTTON_IS_DOWN);

	PSX2MesaDestroyContext(Context);

	DeinitController();
#endif
#ifdef FOR_WIN
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();
#endif

	return 0;
}

#ifdef FOR_WIN
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.f, GLfloat(w) / GLfloat(h), 1.f, 1000.f);

	glMatrixMode(GL_MODELVIEW);	
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'x': AngleX = PI / 4.f; break;
		case 'X': XaglForce = 1.9f; break;
		case 'y': AngleY = PI / 4.f; break;
		case 'Y': YaglForce = 1.9f; break;
	}	
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	DrawPendulum();
	glutSwapBuffers();
}

void idle(void)
{
	DrivePendulum();
	glutPostRedisplay();
}
#endif

void DrawPendulum(void)
{
	glPushMatrix();
	gluLookAt(0.f, Zoom, -Length / 2.f,
				0.f, 0.f, -Length / 2.f,
				0.f, 0.f, 1.f);

	glRotatef(DEGS(AngleX), 1.f, 0.f, 0.f);
	glRotatef(DEGS(AngleY), 0.f, 1.f, 0.f);

	glBegin(GL_LINES);
	 glColor3f(1.f, 1.f, 1.f);
	 glVertex3f(0.f, 0.f, 0.f);
	 glVertex3f(0.f, 0.f, -Length + Mass);
	glEnd();

	glTranslatef(0.f, 0.f, -Length);
	glColor3f(1.f, 0.f, 0.f);
	glutWireSphere(Mass, 7, 7);

	glPopMatrix();
}

// Note that dt should be calculated as 1 / FPS.
void DrivePendulum(void)
{
	if(SIGN(DaglX) == SIGN(AngleX))
		D2aglX = -AngleX * 0.5f;
	else
		D2aglX = -AngleX * 0.3f;
	D2aglX += XaglForce;
	DaglX += D2aglX * dt;
	AngleX += DaglX * dt;

	if(SIGN(DaglY) == SIGN(AngleY))
		D2aglY = -AngleY * 0.5f;
	else
		D2aglY = -AngleY * 0.3f;
	D2aglY += YaglForce;
	DaglY += D2aglY * dt;
	AngleY += DaglY * dt;

	XaglForce = 0.0;
	YaglForce = 0.0;
}

int StickCB(float x0, float y0, float x1, float y1)
{
	AngleY += x0 * 0.1f;
	AngleX += y0 * 0.1f;

	return 0;
}

#ifdef FOR_PS2
int ButtonCB(int Button, int State)
{
	switch(Button)
	{
		case X_BUTTON:
		{
			AngleX = PI / 4.f;
			break;
		}
		case CIRCLE_BUTTON:
		{
			AngleY = PI / 4.f;
			break;
		}
		case L1_BUTTON:
		{
			Zoom += 0.1f;
			break;
		}
		case R1_BUTTON:
		{
			Zoom -= 0.1f;
			break;
		}
		case L2_BUTTON:
		{
			XaglForce = 0.4f;
			break;
		}
		case R2_BUTTON:
		{
			YaglForce = 0.4f;
			break;
		}
	}

	return 0;
}
#endif

// endof Pendulum.cc

// Main.cpp

#include <GL/glut.h>
#include <assert.h>
#include "Spring.h"

// TODO: add static friction force?
// TODO: add texture mapping facility?  yeah.
// TODO: come up with BSP tree facility for VSD and CD.

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboard_up(unsigned char key, int x, int y);
void special(int key, int x, int y);
void special_up(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle(void);

Lattice *lattice0 = 0, *lattice1 = 0;
Lattice *ctrl_lattice = 0;
Vector rot = {0.f, 0.f, 0.f};
int mouse_x, mouse_y;
int mouse_dx, mouse_dy;
Vector gravity_force = {0.f, -0.5f, 0.f};
Vector throw_force = {0.f, 1.3f, 0.f};
int do_physics = 0;
int draw_springs = 0;
int draw_norms = 0;
int follow_cam = 0;
Plane ground_plane;
int draw_mode = PGON_DRAW_LIT | PGON_DRAW_SMOOTH;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowPosition(400, 300);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);
	glutIgnoreKeyRepeat(1);

	// Setup the ground plane.
	Vector vec = {0.f, 1.f, 0.f, 0.f};
	Vector pt = {0.f, -4.f, 0.f, 1.f};
	MakePlane(&ground_plane, &pt, &vec);

	// Create the lattices.
	Matrix m;
//	MatrixScale(&m, 0.5f, 0.5f, 0.5f);
	MatrixIdent(&m);
//	lattice0 = CreateRegularLattice(3, 3, 3, 90.f, 90.f, 90.f, &m, SN_FLG_EXT_FORCES | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION | SN_FLG_GRAVITY );
//	lattice0 = CreateTetrahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_EXT_FORCES | SN_FLG_FRICTION, 90.f);
//	lattice0 = CreateDodecahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_EXT_FORCES | SN_FLG_FRICTION | SN_FLG_COLL_PGON, 90.f, 90.f, 10.f);
	lattice0 = CreateIcosahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_EXT_FORCES | SN_FLG_FRICTION | SN_FLG_COLL_PGON, 60.f, 60.f);
//	lattice0 = CreateSoccerBallLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_EXT_FORCES | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION, 40.f, 40.f, 40.f);
//	lattice0 = CreateCarLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_EXT_FORCES | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION, 60.f);
	VectorSet(&vec, 0.f, 5.f, 0.f);
	MatrixTranslate(&m, &vec);
	lattice1 = CreateRegularLattice(2, 6, 4, 90.f, 90.f, 90.f, &m, SN_FLG_EXT_FORCES | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION | SN_FLG_GRAVITY );
//	lattice1 = CreateTetrahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE, 90.f);
	RegisterLatticeCollisionPlane(lattice0, &ground_plane);
	RegisterLatticeCollisionPlane(lattice1, &ground_plane);
	int k;
	for(k = 0; k < lattice0->num_pgons; k++)	
		RegisterLatticeCollisionPolygon(lattice1, &lattice0->pgon[k]);
	for(k = 0; k < lattice1->num_pgons; k++)
		RegisterLatticeCollisionPolygon(lattice0, &lattice1->pgon[k]);	

	ctrl_lattice = lattice0;

	glClearColor(1.f, 1.f, 1.f, 1.f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glPointSize(3.f);

	GLfloat light0_amb[4] = {0.7f, 0.7f, 0.7f, 1.f};
	GLfloat light0_dif[4] = {0.9f, 0.9f, 0.9f, 1.f};
	GLfloat light0_pos[4] = {1.f, 1.f, 1.f, 0.f};
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_dif);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialFunc(special);
	glutSpecialUpFunc(special_up);
//	glutMouseFunc(mouse);		// Disable mouse since arrow-keys depend on initial orientation.
//	glutMotionFunc(motion);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(follow_cam)
	{
		Vector center;
		CalcLatticeCenter(ctrl_lattice, &center);
		gluLookAt(0.f, 0.f, 20.f,
					center.x, center.y, center.z,
					0.f, 1.f, 0.f);
	}
	else
	{
		glTranslatef(0.f, 0.f, -20.f);
		glRotatef(rot.x, 1.f, 0.f, 0.f);
		glRotatef(rot.y, 0.f, 1.f, 0.f);
		glRotatef(rot.z, 0.f, 0.f, 1.f);
	}

	if(draw_springs)
	{
		DrawLatticeSprings(lattice0);
		DrawLatticeSprings(lattice1);
	}
	else
	{
		glEnable(GL_LIGHTING);
		DrawLatticePolygons(lattice0, draw_mode);
		DrawLatticePolygons(lattice1, draw_mode);
		glDisable(GL_LIGHTING);

		if(draw_norms)
		{
			glColor3f(0.f, 0.f, 0.f);
			DrawLatticeFaceNorms(lattice0);
			DrawLatticeFaceNorms(lattice1);
			
			glColor3f(1.f, 0.f, 0.f);
			DrawLatticeVertexNorms(lattice0);
			DrawLatticeVertexNorms(lattice1);
		}
	}

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	GLfloat aspect_ratio = (h == 0) ? 0.f : (GLfloat(w) / GLfloat(h));
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.f, aspect_ratio, 0.1f, 1000.f);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:	// ESC key.
		{
			DestroyLattice(lattice0);
			DestroyLattice(lattice1);
			exit(0);
			break;
		}
		case ' ':
		{
			do_physics = 1 - do_physics;
			break;
		}		
		case 'q':
		case 'Q':
		{
			RegisterLatticeExternalForce(ctrl_lattice, &throw_force);
			break;
		}
		case 't':
		case 'T':
		{
			VectorSet(&ctrl_lattice->torque, -2.5f, 0.f, 0.f);
			break;
		}
		case 's':
		case 'S':
		{
			draw_springs = 1 - draw_springs;
			break;
		}
		case 'c':
		case 'C':
		{
			if(ctrl_lattice == lattice0)
				ctrl_lattice = lattice1;
			else
				ctrl_lattice = lattice0;
			break;
		}
		case 'd':
		case 'D':
		{
			break;
		}
		case 'n':
		case 'N':
		{
			draw_norms = 1 - draw_norms;
			break;
		}
		case 'f':
		case 'F':
		{
			// this is only worth it if the ground plane
			// is drawn and the arrow keys are programmed
			// to move the lattice with respect to the camera orientation.
			follow_cam = 1 - follow_cam;
			break;
		}
	}

	glutPostRedisplay();
}

void keyboard_up(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'q':
		case 'Q':
		{
			UnregisterLatticeExternalForce(ctrl_lattice, &throw_force);
			break;
		}
		case 't':
		case 'T':
		{
			VectorSet(&ctrl_lattice->torque, 0.f, 0.f, 0.f);
			break;
		}		
	}
}

static Vector up_force = {0.f, 0.f, -1.3f, 0.f};
static Vector down_force = {0.f, 0.f, 1.3f, 0.f};
static Vector left_force = {-1.3f, 0.f, 0.f, 0.f};
static Vector right_force = {1.3f, 0.f, 0.f, 0.f};

void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:		RegisterLatticeExternalForce(ctrl_lattice, &up_force);		break;
		case GLUT_KEY_DOWN:		RegisterLatticeExternalForce(ctrl_lattice, &down_force);	break;
		case GLUT_KEY_LEFT:		RegisterLatticeExternalForce(ctrl_lattice, &left_force);	break;
		case GLUT_KEY_RIGHT:	RegisterLatticeExternalForce(ctrl_lattice, &right_force);	break;
		case GLUT_KEY_F1:
		case GLUT_KEY_F2:
		case GLUT_KEY_F3:
		case GLUT_KEY_F4:
		{
			int k;
			for(k = 0; k < lattice0->num_pgons; k++)	
				UnregisterLatticeCollisionPolygon(lattice1, &lattice0->pgon[k]);
			DestroyLattice(lattice0);

			switch(key)
			{
				case GLUT_KEY_F1:
				{
					Matrix m;
					MatrixIdent(&m);
					lattice0 = CreateRegularLattice(2, 2, 2, 90.f, 90.f, 90.f, &m, SN_FLG_EXT_FORCES | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION | SN_FLG_GRAVITY );
					break;
				}
				case GLUT_KEY_F2:
				{
					Matrix m;
					MatrixIdent(&m);
					lattice0 = CreateDodecahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_EXT_FORCES | SN_FLG_FRICTION | SN_FLG_COLL_PGON, 90.f, 90.f, 10.f);
					break;
				}
				case GLUT_KEY_F3:
				{
					Matrix m;
					MatrixIdent(&m);
					lattice0 = CreateIcosahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_TORQUE | SN_FLG_EXT_FORCES | SN_FLG_FRICTION | SN_FLG_COLL_PGON, 60.f, 60.f);
					break;
				}
//				case GLUT_KEY_F4:
//				{
//					Matrix m;
//					MatrixScale(&m, 0.2f, 0.2f, 0.2f);
//					lattice0 = CreateSoccerBallLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_EXT_FORCES | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION, 40.f, 40.f, 40.f);
//					break;
//				}
				case GLUT_KEY_F4:
				{
					Matrix m;
					MatrixIdent(&m);
					lattice0 = CreateTetrahedronLattice(&m, SN_FLG_GRAVITY | SN_FLG_COLL_PLANE | SN_FLG_EXT_FORCES | SN_FLG_TORQUE | SN_FLG_COLL_PGON | SN_FLG_FRICTION, 20.f);
					break;
				}
			}

			for(k = 0; k < lattice0->num_pgons; k++)	
				RegisterLatticeCollisionPolygon(lattice1, &lattice0->pgon[k]);
			for(k = 0; k < lattice1->num_pgons; k++)
				RegisterLatticeCollisionPolygon(lattice0, &lattice1->pgon[k]);
			RegisterLatticeCollisionPlane(lattice0, &ground_plane);
			break;
		}
	}

		

	glutPostRedisplay();
}

void special_up(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:		UnregisterLatticeExternalForce(ctrl_lattice, &up_force);	break;
		case GLUT_KEY_DOWN:		UnregisterLatticeExternalForce(ctrl_lattice, &down_force);	break;
		case GLUT_KEY_LEFT:		UnregisterLatticeExternalForce(ctrl_lattice, &left_force);	break;
		case GLUT_KEY_RIGHT:	UnregisterLatticeExternalForce(ctrl_lattice, &right_force);	break;
	}
}

void idle(void)
{
	if(do_physics)
	{
		CalcLatticeForces(lattice0);
		CalcLatticeForces(lattice1);

		IntegrateLatticeNodes(lattice0, 0.005f);
		IntegrateLatticeNodes(lattice1, 0.005f);

		CalcLatticeNorms(lattice0);
		CalcLatticeNorms(lattice1);

		glutPostRedisplay();
	}
}

void mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		mouse_x = x;
		mouse_y = y;
	}
}

void motion(int x, int y)
{
	mouse_dx = x - mouse_x;
	mouse_dy = y - mouse_y;
	mouse_x = x;
	mouse_y = y;

//	rot.x += GLfloat(mouse_dy);
	rot.y += GLfloat(mouse_dx);

	glutPostRedisplay();
}

// endof Main.cpp
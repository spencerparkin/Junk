// Main.cpp

// Programmed by Spencer T. Parkin

#define _CRT_TERMINATE_DEFINED
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Main.h"
#include "Vector.h"
#include "ConvexHull.h"
#include "Skeleton.h"

//#define DIAGNOSTIC_MODE

GLfloat aspect_ratio = 1.0;
Vector rot = {0.f, 0.f, 0.f};
GLfloat zoom = -10.f;
int mouse_x, mouse_y;
int mouse_dx, mouse_dy;
int do_lighting = 0;
int draw_axis = 0;
int go_optimize = 0;
int do_ortho_proj = 0;
int draw_norms = 0;
int draw_edges = 0;
GLint front_face = GL_CCW;
int arrow_key_bits = 0;
int jt_idx = 0;

int main(int argc, char **argv)
{
	// initialize program data.
#ifdef DIAGNOSTIC_MODE
	Vector rot_model = {PI / 3.0, PI / 3.0, 0.0};
	PopulatePointCloud(10, 321, 0); //&rot_model);
#else
	char filename[80];
	printf("filename: ");
	fflush(stdout);
	scanf_s("%s", filename);
	if(!MakeSkeleton(filename))
		return -1;
	Find3DConvexHull();
#endif // DIAGNOSTIC_MODE

	// initialize glut.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);

	// setup initial opengl state machine
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glPointSize(4.f);
	glLineWidth(2.f);
	glEnable(GL_CULL_FACE);
	glFrontFace(front_face);
	glCullFace(GL_BACK);

	// setup an opengl light.
	glEnable(GL_LIGHT0);
	GLfloat light_pos[4] = {1.f, 1.f, 1.f, 0.f};
	GLfloat light_amb[4] = {0.5f, 0.5f, 0.5f, 1.f};
	GLfloat light_dif[4] = {0.8f, 0.8f, 0.8f, 1.f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);	

	// register glut callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(special);
	glutSpecialUpFunc(special_up);
	glutIdleFunc(idle);

	// enter the event processing loop.
	glutMainLoop();

	return 0;
}

void SaveCloud(char *filename)
{
	FILE *fp = 0;
	fopen_s( &fp, filename, "wb");
	if(!fp)
		return;
	fwrite((void *)&num_cloud_pts, sizeof(num_cloud_pts), 1, fp);
	fwrite((void *)pt_cloud, sizeof(Vector), num_cloud_pts, fp);
	fclose(fp);
	printf("data written...\n");
}

void LoadCloud(char *filename)
{
	FILE *fp = 0;
	fopen_s( &fp, filename, "rb");
	if(!fp)
		return;
	fread((void *)&num_cloud_pts, sizeof(num_cloud_pts), 1, fp);
	fread((void *)pt_cloud, sizeof(Vector), num_cloud_pts, fp);
	fclose(fp);
	printf("data read...\n");
}

// return a random number in [0,1].
double Random(void)
{
	double r = (double)rand();
	return r / double(RAND_MAX);
}

double Random(double min, double max)
{
	return min + (max - min) * Random();
}

void PopulatePointCloud(int cloud, unsigned int seed, Vector *rot)
{
	const double phi = (1.0 + sqrt(5.0)) / 2.0;
	srand(seed);
	switch(cloud)
	{
		case 0:
		{
			// test a point cloud that
			// lives entirely in one plane.
			num_cloud_pts = MAX_POINTS;
			Vector *pt = &pt_cloud[0];
			int k;
			double angle, radius;
			for(k = 0; k < num_cloud_pts; k++)
			{
				// hopefully all of these will be unique.
				radius = Random(0.5, 5.0);
				angle = Random(0.0, 2.0 * PI);
				VectorSet(pt, radius * cos(angle), radius * sin(angle), 0.0);
				VectorRotate(pt, pt, &Xaxis, -PI / 9.0);
				VectorRotate(pt, pt, &Yaxis, PI / 1.5);
				pt++;				
			}
			break;
		}
		case 1:
		{
			// test a square array of points
			// living entirely in one plane.
			num_cloud_pts = 4*4;
			Vector *pt = &pt_cloud[0];
			double x, y;
			int i, j;
			x = -1.5;
			for(i = 0; i < 4; i++)
			{
				y = -1.5;
				for(j = 0; j < 4; j++)
				{
					VectorSet(pt, x, y, 0.0);
					VectorRotate(pt, pt, &Xaxis, -PI / 9.0);
					VectorRotate(pt, pt, &Yaxis, PI / 1.5);
					pt++;
					y += 1.0;
				}
				x += 1.0;
			}
			break;
		}
		case 2:
		{
			// test a cube of points.
			num_cloud_pts = 4*4*4;
			Vector *pt = &pt_cloud[0];
			double x, y, z;
			int i, j, k;
			x = -1.5;
			for(i = 0; i < 4; i++)
			{
				y = -1.5;
				for(j = 0; j < 4; j++)
				{
					z = -1.5;
					for(k = 0; k < 4; k++)
					{
						VectorSet(pt, x, y, z);
						VectorRotate(pt, pt, &Xaxis, PI / 4.0);
						VectorRotate(pt, pt, &Yaxis, PI / 3.0);
						pt++;
						z += 1.0;
					}
					y += 1.0;
				}
				x += 1.0;
			}
			break;
		}
		case 3:
		{
			num_cloud_pts = 50; //MAX_POINTS;
			Vector *pt = &pt_cloud[0];
			double x, y, z;
			int k;			
			for(k = 0; k < num_cloud_pts; k++)
			{
				// hopefully all of these will be unique.
				x = Random(-5.0, 5.0);
				y = Random(-5.0, 5.0);
				z = Random(-5.0, 5.0);
				VectorSet(pt++, x, y, z);
			}
			break;			
		}
		case 4:
		{			
			// this is the dodacehedron.			
			Vector *pt = &pt_cloud[0];
			VectorSet(pt++, 1.0, 1.0, 1.0);
			VectorSet(pt++, -1.0, 1.0, 1.0);
			VectorSet(pt++, 1.0, -1.0, 1.0);
			VectorSet(pt++, -1.0, -1.0, 1.0);
			VectorSet(pt++, 1.0, 1.0, -1.0);
			VectorSet(pt++, -1.0, 1.0, -1.0);
			VectorSet(pt++, 1.0, -1.0, -1.0);
			VectorSet(pt++, -1.0, -1.0, -1.0);
			VectorSet(pt++, 0.0, 1.0 / phi, phi);
			VectorSet(pt++, 0.0, -1.0 / phi, phi);
			VectorSet(pt++, 0.0, 1.0 / phi, -phi);
			VectorSet(pt++, 0.0, -1.0 / phi, -phi);
			VectorSet(pt++, 1.0 / phi, phi, 0.0);
			VectorSet(pt++, -1.0 / phi, phi, 0.0);
			VectorSet(pt++, 1.0 / phi, -phi, 0.0);
			VectorSet(pt++, -1.0 / phi, -phi, 0.0);
			VectorSet(pt++, phi, 0.0, 1.0 / phi);
			VectorSet(pt++, -phi, 0.0, 1.0 / phi);
			VectorSet(pt++, phi, 0.0, -1.0 / phi);
			VectorSet(pt++, -phi, 0.0, -1.0 / phi);
			num_cloud_pts = pt - pt_cloud;
			break;
		}
		case 5:
		{
			// test the icosahedron.
			Vector *pt = &pt_cloud[0];
			VectorSet(pt++, 0.0, 1.0, phi);
			VectorSet(pt++, 0.0, -1.0, phi);
			VectorSet(pt++, 0.0, 1.0, -phi);
			VectorSet(pt++, 0.0, -1.0, -phi);
			VectorSet(pt++, 1.0, phi, 0.0);
			VectorSet(pt++, -1.0, phi, 0.0);
			VectorSet(pt++, 1.0, -phi, 0.0);
			VectorSet(pt++, -1.0, -phi, 0.0);
			VectorSet(pt++, phi, 0.0, 1.0);
			VectorSet(pt++, -phi, 0.0, 1.0);
			VectorSet(pt++, phi, 0.0, -1.0);
			VectorSet(pt++, -phi, 0.0, -1.0);
			num_cloud_pts = pt - pt_cloud;
			int k;
			for(k = 0; k < num_cloud_pts; k++)
				VectorNormalize(&pt_cloud[k], &pt_cloud[k]);
			break;
		}
		case 6:
		{
			// icosidodecahedron.
			Vector *pt = &pt_cloud[0];
			VectorSet(pt++, 2.0, 0.0, 0.0);
			VectorSet(pt++, -2.0, 0.0, 0.0);
			VectorSet(pt++, 0.0, 2.0, 0.0);
			VectorSet(pt++, 0.0, -2.0, 0.0);
			VectorSet(pt++, 0.0, 0.0, 2.0);
			VectorSet(pt++, 0.0, 0.0, -2.0);
			VectorSet(pt++, phi, 1.0 / phi, 1.0);
			VectorSet(pt++, -phi, 1.0 / phi, 1.0);
			VectorSet(pt++, phi, -1.0 / phi, 1.0);
			VectorSet(pt++, -phi, -1.0 / phi, 1.0);
			VectorSet(pt++, phi, 1.0 / phi, -1.0);
			VectorSet(pt++, -phi, 1.0 / phi, -1.0);
			VectorSet(pt++, phi, -1.0 / phi, -1.0);
			VectorSet(pt++, -phi, -1.0 / phi, -1.0);
			VectorSet(pt++, 1.0, phi, 1.0 / phi);
			VectorSet(pt++, -1.0, phi, 1.0 / phi);
			VectorSet(pt++, 1.0, -phi, 1.0 / phi);
			VectorSet(pt++, -1.0, -phi, 1.0 / phi);
			VectorSet(pt++, 1.0, phi, -1.0 / phi);
			VectorSet(pt++, -1.0, phi, -1.0 / phi);
			VectorSet(pt++, 1.0, -phi, -1.0 / phi);
			VectorSet(pt++, -1.0, -phi, -1.0 / phi);
			VectorSet(pt++, 1.0 / phi, 1.0, phi);
			VectorSet(pt++, -1.0 / phi, 1.0, phi);
			VectorSet(pt++, 1.0 / phi, -1.0, phi);
			VectorSet(pt++, -1.0 / phi, -1.0, phi);
			VectorSet(pt++, 1.0 / phi, 1.0, -phi);
			VectorSet(pt++, -1.0 / phi, 1.0, -phi);
			VectorSet(pt++, 1.0 / phi, -1.0, -phi);
			VectorSet(pt++, -1.0 / phi, -1.0, -phi);
			num_cloud_pts = pt - pt_cloud;
			break;
		}
		case 7:
		{
			Vector *pt = &pt_cloud[0];
			const int top_sides = 15;
			const int bot_sides = 5;
			const double top_radius = 3.0;
			const double bot_radius = 2.0;
			const double length = 3.0;
			double angle;
			int k;
			for(k = 0; k < top_sides; k++)
			{
				angle = 2.0 * PI * double(k) / double(top_sides);
				VectorSet(pt++, top_radius * cos(angle), top_radius * sin(angle), -length / 2.0);
			}
			for(k = 0; k < bot_sides; k++)
			{
				angle = 2.0 * PI * double(k) / double(bot_sides);
				VectorSet(pt++, bot_radius * cos(angle), bot_radius * sin(angle), length / 2.0);
			}
			num_cloud_pts = pt - pt_cloud;
			break;
		}
		case 8:
		{
			Vector *pt = &pt_cloud[0];
			const double sphere_radius = 2.0;
			double x, y, z, r;
			const int lat_count = 10;
			const int lon_count = 16;
			int i, j;
			double lat_angle, lon_angle;
			for(i = 0; i <= lat_count; i++)
			{
				lat_angle = PI * double(i) / double(lat_count);
				z = sphere_radius * cos(lat_angle);
				r = sphere_radius * sin(lat_angle);
				for(j = 0; j < lon_count; j++)
				{					
					lon_angle = 2.0 * PI * double(j) / double(lon_count);
					x = r * cos(lon_angle);
					y = r * sin(lon_angle);
					VectorSet(pt++, x, y, z);
					if(i == 0 || i == lat_count)
						break;
				}
			}
			num_cloud_pts = pt - pt_cloud;
			break;
		}
		case 9:
		{
			Vector *pt = &pt_cloud[0];
			const double ring_radius = 3.0;
			const double tube_radius = 0.5;
			double x, y, z, r;
			double tube_angle, ring_angle;
			const int ring_sides = 20;
			const int tube_sides = 12;
			int i, j;
			for(i = 0; i < tube_sides; i++)
			{
				tube_angle = 2.0 * PI * double(i) / double(tube_sides);
				r = ring_radius + tube_radius * cos(tube_angle);
				z = tube_radius * sin(tube_angle);
				for(j = 0; j < ring_sides; j++)
				{
					ring_angle = 2.0 * PI * double(j) / double(ring_sides);
					x = r * cos(ring_angle);
					y = r * sin(ring_angle);
					VectorSet(pt++, x, y, z);
				}
			}
			num_cloud_pts = pt - pt_cloud;
			break;
		}
		case 10:
		{
			Vector *pt = &pt_cloud[0];
			double angle = 0.2 * 180.0 / PI;
			double sa = sin(angle);
			double ca = cos(angle);
			VectorSet(pt++, -2.0 * ca, 0.0, 2.0 * sa);
			VectorSet(pt++, ca, sqrt(3.0) * ca, 2.0 * sa);
			VectorSet(pt++, ca, -sqrt(3.0) * ca, 2.0 * sa);			
			VectorSet(pt++, ca * 2.0, 0.0, -1.0 - 2.0 * sa);
			VectorSet(pt++, -ca, sqrt(3.0) * ca, -1.0 - 2.0 * sa);
			VectorSet(pt++, -ca, -sqrt(3.0) * ca, -1.0 - 2.0 * sa);
			num_cloud_pts = pt - pt_cloud;
			break;
		}
		default:
		{
			num_cloud_pts = 0;
			break;
		}
	}

	if(rot)
	{
		Vector *pt;
		int k;
		for(k = 0; k < num_cloud_pts; k++)
		{
			pt = &pt_cloud[k];
			VectorRotate(pt, pt, &Xaxis, rot->x);
			VectorRotate(pt, pt, &Yaxis, rot->y);
			VectorRotate(pt, pt, &Zaxis, rot->z);
		}
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(do_ortho_proj)
	{
		if(aspect_ratio > 1.f)
			glOrtho(zoom * aspect_ratio, -zoom * aspect_ratio, zoom, -zoom, -1000.f, 1000.f);
		else
			glOrtho(zoom, -zoom, zoom / aspect_ratio, -zoom / aspect_ratio, -1000.f, 1000.f);
	}
	else
		gluPerspective(60.f, aspect_ratio, 0.1f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(!do_ortho_proj)
		glTranslatef(0.f, 0.f, zoom);
	glRotatef(rot.x, 1.f, 0.f, 0.f);
	glRotatef(rot.y, 0.f, 1.f, 0.f);
	glRotatef(rot.z, 0.f, 0.f, 1.f);

	// draw the x, y, and z-axis for reference.
	if(draw_axis)
	{
		glBegin(GL_LINES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-5.f, 0.f, 0.f);
		glVertex3f(5.f, 0.f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.f, -5.f, 0.f);
		glVertex3f(0.f, 5.f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, -5.f);
		glVertex3f(0.f, 0.f, 5.f);
		glEnd();
		glBegin(GL_POINTS);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(5.f, 0.f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.f, 5.f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 5.f);
		glEnd();
	}

#ifdef DIAGNOSTIC_MODE
	glColor3f(0.f, 0.f, 0.f);
	DrawPointCloud();
	if(draw_edges)
		DrawEdgeList();
#else
	DrawSkeleton();
#endif // DIAGNOSTIC_MODE
	DrawPolygonList();	

	glutSwapBuffers();
}

void DrawPointCloud(void)
{
	glBegin(GL_POINTS);
	Vector *pt;
	int k;
	for(k = 0; k < num_cloud_pts; k++)
	{
		pt = &pt_cloud[k];
		glVertex3f(pt->x, pt->y, pt->z);
	}
	glEnd();
}

void DrawEdgeList(void)
{
	glBegin(GL_LINES);
	glColor3f(0.f, 1.f, 0.f);
	Edge *e;
	for(e = (Edge *)EdgeList.head; e; e = e->next)
	{
		glVertex3f(e->vtx[0]->x, e->vtx[0]->y, e->vtx[0]->z);
		glVertex3f(e->vtx[1]->x, e->vtx[1]->y, e->vtx[1]->z);
	}
	glEnd();
}

void DrawPolygonList(void)
{
	if(do_lighting)
	{
		glEnable(GL_LIGHTING);
		GLfloat mat_dif[] = {0.5f, 0.5f, 0.5f, 1.f};
		GLfloat mat_amb[] = {0.9f, 0.9f, 0.9f, 1.f};
		glMaterialfv(GL_DIFFUSE, GL_FRONT_AND_BACK, mat_dif);
		glMaterialfv(GL_AMBIENT, GL_FRONT_AND_BACK, mat_amb);
	}
	Vector *pt, center;
	Polygon *poly;
	srand(0);
	int k;	
	for(poly = (Polygon *)PolygonList.head; poly; poly = poly->next)
	{
		glBegin(GL_POLYGON);
		if(!do_lighting)
			glColor3f(Random(0.0, 1.0), Random(0.0, 1.0), Random(0.0, 1.0));
		else
		{
			if(front_face == GL_CCW)
				glNormal3f(poly->norm.x, poly->norm.y, poly->norm.z);
			else
				glNormal3f(-poly->norm.x, -poly->norm.y, -poly->norm.z);
		}		
		for(k = 0; k < poly->num_vertices; k++)
		{
			pt = poly->vtx[k];
			glVertex3f(pt->x, pt->y, pt->z);			
		}
		glEnd();
	}	
	if(do_lighting)
		glDisable(GL_LIGHTING);
	if(draw_norms)
	{
		glBegin(GL_LINES);
		for(poly = (Polygon *)PolygonList.head; poly; poly = poly->next)
		{
			VectorCopy(&center, &Origin);
			for(k = 0; k < poly->num_vertices; k++)
				VectorAdd(&center, &center, poly->vtx[k]);
			VectorScale(&center, &center, 1.0 / double(poly->num_vertices));			
			glColor3f(1.f, 0.f, 1.f);
			glVertex3f(center.x, center.y, center.z);
			VectorAdd(&center, &center, &poly->norm);
			glVertex3f(center.x, center.y, center.z);			
		}
		glEnd();
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	aspect_ratio = (h == 0) ? 0.0 : (GLfloat(w) / GLfloat(h));
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
#ifdef DIAGNOSTIC_MODE
		case 's':
		case 'S':
		{
			Reset3DConvexHullFinder();
			break;
		}
		case ' ':
		{
			Iterate3DConvexHullFinder();
			break;
		}
		case 'p':
		case 'P':
		{
			FILE *fp = fopen("mesh.txt", "w");
			Print3DConvexHull(fp);
			fclose(fp);
			break;
		}
#endif // DIAGNOSTIC_MODE
		case '[':
		{
			zoom -= 0.5f;
			break;
		}
		case ']':
		{
			zoom += 0.5f;
			break;
		}
		case 'c':
		case 'C':
		{
			glGetIntegerv(GL_FRONT_FACE, &front_face);
			if(front_face == GL_CCW)
				front_face = GL_CW;
			else
				front_face = GL_CCW;
			glFrontFace(front_face);
			break;
		}		
		case 'a':
		case 'A':
		{
			draw_axis = 1 - draw_axis;
			break;
		}
		case 'b':
		case 'B':
		{
			if(glIsEnabled(GL_CULL_FACE))
				glDisable(GL_CULL_FACE);
			else
				glEnable(GL_CULL_FACE);
			break;
		}
		case 'l':
		case 'L':
		{
			do_lighting = 1 - do_lighting;
			break;
		}
		case 'j':
		{
			jt_idx--;
			if(jt_idx < 0)
				jt_idx = num_joints - 1;
			break;
		}
		case 'J':
		{
			jt_idx = (jt_idx + 1) % num_joints;
			break;
		}
		case 'n':
		case 'N':
		{
			draw_norms = 1 - draw_norms;
			break;
		}
		case 'e':
		case 'E':
		{
			draw_edges = 1 - draw_edges;
			break;
		}
		case 'v':
		case 'V':
		{
			double vol = Calc3DConvexHullVolume();
			printf("vol = %1.32f\n", vol);
			break;
		}
		case 'q':
		case 'Q':
		{
			double area = Calc3DConvexHullSurfaceArea();
			printf("area = %1.32f\n", area);
			break;
		}
		case 'o':
		case 'O':
		{
			//OptimizeJoint(&joint_list[jt_idx]);
			//OptimizeSkeleton();
			go_optimize = 1 - go_optimize;
			break;
		}
        case 'i':
        case 'I':
        {
			Bone *bone;
			int k;
			for(k = 0; k < num_bones; k++)
			{
				bone = &bone_list[k];
				printf("(%d) %f == %1.32f\n", k, bone->length, VectorDistance(bone->jt[0]->vtx, bone->jt[1]->vtx));
			}
			break;
        }
//		case 'p':
//		case 'P':
//		{
//			do_ortho_proj = 1 - do_ortho_proj;
//			break;
//		}
	}
	glutPostRedisplay();
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
	rot.y += double(mouse_dx) * 0.5;
	rot.x += double(mouse_dy) * 0.5;
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:			arrow_key_bits |= KEY_UP_BIT; break;
		case GLUT_KEY_DOWN:			arrow_key_bits |= KEY_DN_BIT; break;
		case GLUT_KEY_LEFT:			arrow_key_bits |= KEY_LF_BIT; break;
		case GLUT_KEY_RIGHT:		arrow_key_bits |= KEY_RT_BIT; break;
		case GLUT_KEY_PAGE_UP:		arrow_key_bits |= KEY_PU_BIT; break;
		case GLUT_KEY_PAGE_DOWN:	arrow_key_bits |= KEY_PD_BIT; break;
		case GLUT_KEY_F1:
		{
#ifdef DIAGNOSTIC_MODE		
			LoadCloud("debug.cld");
#else
			SaveCloud("debug.cld");
#endif // DIAGNOSTIC_MODE
			break;
		}
		case GLUT_KEY_F8:
		{
			int k;
			for(k = 0; k < num_joints; k++)
				joint_list[k].sphere_radius = 0.5;
			break;
		}
	}
	glutPostRedisplay();
}

void special_up(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:			arrow_key_bits &= ~KEY_UP_BIT; break;
		case GLUT_KEY_DOWN:			arrow_key_bits &= ~KEY_DN_BIT; break;
		case GLUT_KEY_LEFT:			arrow_key_bits &= ~KEY_LF_BIT; break;
		case GLUT_KEY_RIGHT:		arrow_key_bits &= ~KEY_RT_BIT; break;
		case GLUT_KEY_PAGE_UP:		arrow_key_bits &= ~KEY_PU_BIT; break;
		case GLUT_KEY_PAGE_DOWN:	arrow_key_bits &= ~KEY_PD_BIT; break;
	}
}

void idle(void)
{
#ifndef DIAGNOSTIC_MODE
	static Vector up = {0.0, 0.1, 0.0};
	static Vector dn = {0.0, -0.1, 0.0};
	static Vector lf = {-0.1, 0.0, 0.0};
	static Vector rt = {0.1, 0.0, 0.0};
	static Vector pu = {0.0, 0.0, 0.1};
	static Vector pd = {0.0, 0.0, -0.1};
	Vector delta;
	VectorCopy(&delta, &Origin);
	if(arrow_key_bits & KEY_UP_BIT)
		VectorAdd(&delta, &delta, &up);
	if(arrow_key_bits & KEY_DN_BIT)
		VectorAdd(&delta, &delta, &dn);
	if(arrow_key_bits & KEY_LF_BIT)
		VectorAdd(&delta, &delta, &lf);
	if(arrow_key_bits & KEY_RT_BIT)
		VectorAdd(&delta, &delta, &rt);
	if(arrow_key_bits & KEY_PU_BIT)
		VectorAdd(&delta, &delta, &pu);
	if(arrow_key_bits & KEY_PD_BIT)
		VectorAdd(&delta, &delta, &pd);
	if(!VectorEq(&delta, &Origin))
	{
		MoveJoint(&joint_list[jt_idx], &delta);
		Find3DConvexHull();
		glutPostRedisplay();
	}
	if(go_optimize)
	{
		OptimizeSkeleton();
		Find3DConvexHull();
		double vol = Calc3DConvexHullVolume();
		printf("vol = %1.32f\n", vol);
		glutPostRedisplay();
	}
#endif // not DIAGNOSTIC_MODE
}

// endof Main.cpp
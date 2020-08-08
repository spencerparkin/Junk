// Main.cpp

// This software was written by Spencer T. Parkin, Copyright (c) 2007, All Rights Reserved.

// i believe this program is highly suseptable
// to local maxima.  i know that the algorithm
// is completely suseptable to it.

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Vector.h"
#include "Volume.h"
#include "List.h"
#include "Tree.h"

//#define DEBUG_VOL_CODE

double Random(void);

Vector rot = {0.f, 0.f, 0.f};
int mouse_x, mouse_y;
int mouse_dx, mouse_dy;
GLint ff;
int draw_tets = 0;

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle(void);

int num_cloud_pts = 0;
Vector pt_cloud[MAX_CLOUD_POINTS];
void MakeCloudPoints(unsigned int seed);
void DrawPointCloud(void);
void DrawTriList(void);
void DrawCandList(void);
void DrawTetList(double scale);

void SetupTree(int tree_type, int parm);
Node *root = 0;
int update_tree_hull = 0;
double Volume = 0.0;
int do_auto_optimization = 0;
int do_lighting = 0;
int draw_axis = 1;
int center_cloud = 0;
Vector cloud_center;
int need_free = 0;
double zoom = -8.0;

void WritePointCloudData(char *filename);
void ReadPointCloudData(char *filename);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(700, 700);
	glutCreateWindow(argv[0]);

	glClearColor(1.f, 1.f, 1.f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glPointSize(3.f);
	glEnable(GL_CULL_FACE);
	ff = GL_CCW;
	glFrontFace(ff);
	glCullFace(GL_CW);	
	glEnable(GL_LIGHT0);
	GLfloat light_pos[4] = {1.f, 1.f, 1.f, 0.f};
	GLfloat light_amb[4] = {0.5f, 0.5f, 0.5f, 1.f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

#ifdef DEBUG_VOL_CODE
	num_cloud_pts = 4096;
	MakeCloudPoints(5321);	
	ResetVolumeAlgorithm(pt_cloud, num_cloud_pts);
#else
	SetupTree(4, 11);	// (3,10) vs (3.11) is interesting
	update_tree_hull = 1;
#endif // DEBUG_VOL_CODE

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

void CalcCloudCenter(void)
{
	int k;
	VectorSet(&cloud_center, 0.0, 0.0, 0.0);
	for(k = 0; k < num_cloud_pts; k++)	
		VectorAdd(&cloud_center, &cloud_center, &pt_cloud[k]);
	VectorScale(&cloud_center, &cloud_center, 1.0 / double(num_cloud_pts));
}

void idle(void)
{
	if(do_auto_optimization && tweak_angle > 0.0001)
	{
		if(!OptimizeTreeIteration(root, root))
		{
			tweak_angle *= 0.98;
			if(tweak_angle < PI / 64.0)
				num_tweak_samples = 16;				
		}
		update_tree_hull = 1;
	}
	if(update_tree_hull)
	{
		update_tree_hull = 0;
		num_cloud_pts = CalcTreeCloudPoints(root, pt_cloud);
		CalcCloudCenter();
		Volume = CalcVolume(pt_cloud, num_cloud_pts);
		printf("Volume = %1.32f\n", Volume);
	}
	glutPostRedisplay();
}

void SetupTree(int tree_type, int parm)
{
	switch(tree_type)
	{
		case 0:
		{
			need_free = 1;

			root = new Node;
			root->num_sub_nodes = 2;
			VectorSet(&root->vec, sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
			
			root->sub_node[0] = new Node;
			root->sub_node[0]->num_sub_nodes = 0;
			VectorSet(&root->sub_node[0]->vec, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0.0);
			
			root->sub_node[1] = new Node;
			root->sub_node[1]->num_sub_nodes = 0;
			VectorSet(&root->sub_node[1]->vec, 0.0, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0);

			break;
		}
		case 1:
		{
			need_free = 1;

			root = new Node;
			root->num_sub_nodes = 1;
			VectorSet(&root->vec, sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);

			root->sub_node[0] = new Node;
			root->sub_node[0]->num_sub_nodes = 1;
			VectorSet(&root->sub_node[0]->vec, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0.0);

			root->sub_node[0]->sub_node[0] = new Node;
			root->sub_node[0]->sub_node[0]->num_sub_nodes = 0;
			VectorSet(&root->sub_node[0]->sub_node[0]->vec, 0.0, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0);

			break;
		}
		case 2:
		{
			need_free = 1;

			root = new Node;
			root->num_sub_nodes = 3;
			VectorSet(&root->vec, sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
			
			root->sub_node[0] = new Node;
			root->sub_node[0]->num_sub_nodes = 0;
			VectorSet(&root->sub_node[0]->vec, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0.0);
			
			root->sub_node[1] = new Node;
			root->sub_node[1]->num_sub_nodes = 1;
			VectorSet(&root->sub_node[1]->vec, 1.0, 1.0, 1.0);
			VectorNormalize(&root->sub_node[1]->vec, &root->sub_node[1]->vec);

			root->sub_node[2] = new Node;
			root->sub_node[2]->num_sub_nodes = 0;
			VectorSet(&root->sub_node[2]->vec, 0.0, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0);

			root->sub_node[1]->sub_node[0] = new Node;
			root->sub_node[1]->sub_node[0]->num_sub_nodes = 0;
			VectorSet(&root->sub_node[1]->sub_node[0]->vec, 0.0, 1.0, 0.0);

			break;
		}
		case 3:
		{
			need_free = 1;

			if(parm > MAX_SUB_NODES)
				break;

			root = new Node;
			root->num_sub_nodes = parm;
			VectorSet(&root->vec, 0.0, 0.0, 1.0);

			int k;
			double angle;
			for(k = 0; k < parm; k++)
			{
				angle = double(k) / double(parm) * 2.0 * PI;
				root->sub_node[k] = new Node;
				root->sub_node[k]->num_sub_nodes = 0;
				VectorSet(&root->sub_node[k]->vec, cos(angle), sin(angle), 1.0);
				VectorNormalize(&root->sub_node[k]->vec, &root->sub_node[k]->vec);
			}

			break;
		}
		case 4:
		{
			need_free = 0;

			static Node n[12];
			VectorSet(&n[0].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[1].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[2].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[3].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[4].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[5].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[6].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[7].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[8].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[9].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[10].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[11].vec, 0.0, 0.0, 1.0);
			
			root = &n[0];
			n[0].num_sub_nodes = 2;
			n[0].sub_node[0] = &n[1];
			n[0].sub_node[1] = &n[2];
			n[1].num_sub_nodes = 0;
			n[2].num_sub_nodes = 3;
			n[2].sub_node[0] = &n[3];
			n[2].sub_node[1] = &n[4];
			n[2].sub_node[2] = &n[5];
			n[3].num_sub_nodes = 2;
			n[3].sub_node[0] = &n[6];
			n[3].sub_node[1] = &n[7];
			n[4].num_sub_nodes = 2;
			n[4].sub_node[0] = &n[8];
			n[4].sub_node[1] = &n[9];
			n[5].num_sub_nodes = 2;
			n[5].sub_node[0] = &n[10];
			n[5].sub_node[1] = &n[11];
			n[6].num_sub_nodes = 0;
			n[7].num_sub_nodes = 0;
			n[8].num_sub_nodes = 0;
			n[9].num_sub_nodes = 0;
			n[10].num_sub_nodes = 0;
			n[11].num_sub_nodes = 0;
			break;
		}
		case 5:
		{
			need_free = 0;

			static Node n[17];
			VectorSet(&n[0].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[1].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[2].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[3].vec, -1.0, 0.0, 0.0);
			VectorSet(&n[4].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[5].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[6].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[7].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[8].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[9].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[10].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[11].vec, -1.0, 0.0, 0.0);
			VectorSet(&n[12].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[13].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[14].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[15].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[16].vec, 0.0, 0.0, 1.0);

			root = &n[0];
			n[0].num_sub_nodes = 1;
			n[1].num_sub_nodes = 3;
			n[2].num_sub_nodes = 2;
			n[3].num_sub_nodes = 3;
			n[4].num_sub_nodes = 3;
			n[5].num_sub_nodes = 0;
			n[6].num_sub_nodes = 0;
			n[7].num_sub_nodes = 2;
			n[8].num_sub_nodes = 0;
			n[9].num_sub_nodes = 0;
			n[10].num_sub_nodes = 0;
			n[11].num_sub_nodes = 0;
			n[12].num_sub_nodes = 2;
			n[13].num_sub_nodes = 0;
			n[14].num_sub_nodes = 0;
			n[15].num_sub_nodes = 0;
			n[16].num_sub_nodes = 0;

			n[0].sub_node[0] = &n[1];

			n[1].sub_node[0] = &n[2];
			n[1].sub_node[1] = &n[3];
			n[1].sub_node[2] = &n[4];

			n[2].sub_node[0] = &n[8];
			n[2].sub_node[1] = &n[9];

			n[3].sub_node[0] = &n[10];
			n[3].sub_node[1] = &n[11];
			n[3].sub_node[2] = &n[12];

			n[4].sub_node[0] = &n[5];
			n[4].sub_node[1] = &n[6];
			n[4].sub_node[2] = &n[7];			

			n[7].sub_node[0] = &n[15];
			n[7].sub_node[1] = &n[16];

			n[12].sub_node[0] = &n[13];
			n[12].sub_node[1] = &n[14];

			break;
		}
		case 6:
		{
			need_free = 0;
			static Node n[9];
			VectorSet(&n[0].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[1].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[2].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[3].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[4].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[5].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[6].vec, -1.0, 0.0, 0.0);
			VectorSet(&n[7].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[8].vec, 0.0, 0.0, -1.0);

			root = &n[0];

			n[0].num_sub_nodes = 1;
			n[1].num_sub_nodes = 3;
			n[2].num_sub_nodes = 0;
			n[3].num_sub_nodes = 0;
			n[4].num_sub_nodes = 3;
			n[5].num_sub_nodes = 0;
			n[6].num_sub_nodes = 0;
			n[7].num_sub_nodes = 1;
			n[8].num_sub_nodes = 0;

			n[0].sub_node[0] = &n[1];

			n[1].sub_node[0] = &n[2];
			n[1].sub_node[1] = &n[3];
			n[1].sub_node[2] = &n[4];

			n[4].sub_node[0] = &n[5];
			n[4].sub_node[1] = &n[6];
			n[4].sub_node[2] = &n[7];

			n[7].sub_node[0] = &n[8];

			break;
		}
		case 7:
		{
			need_free = 0;
			static Node n[11];
			VectorSet(&n[0].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[1].vec, 0.0, -1.0, 0.0);
			VectorSet(&n[2].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[3].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[4].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[5].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[6].vec, 0.0, 0.0, -1.0);
			VectorSet(&n[7].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[8].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[9].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[10].vec, 0.0, 1.0, 0.0);
			root = &n[0];

			n[0].num_sub_nodes = 1;
			n[1].num_sub_nodes = 1;
			n[2].num_sub_nodes = 3;
			n[3].num_sub_nodes = 0;
			n[4].num_sub_nodes = 0;
			n[5].num_sub_nodes = 3;
			n[6].num_sub_nodes = 0;
			n[7].num_sub_nodes = 0;
			n[8].num_sub_nodes = 1;
			n[9].num_sub_nodes = 1;
			n[10].num_sub_nodes = 0;

			n[0].sub_node[0] = &n[1];
			n[1].sub_node[0] = &n[2];
			n[2].sub_node[0] = &n[3];
			n[2].sub_node[1] = &n[4];
			n[2].sub_node[2] = &n[5];
			n[5].sub_node[0] = &n[6];
			n[5].sub_node[1] = &n[7];
			n[5].sub_node[2] = &n[8];
			n[8].sub_node[0] = &n[9];
			n[9].sub_node[0] = &n[10];
			break;
		}
		case 8:
		{
			need_free = 0;
			static Node n[11];
			VectorSet(&n[0].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[1].vec, 0.0, 1.0, 0.0);
			VectorSet(&n[2].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[3].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[4].vec, sqrt(2.0)/2.0, sqrt(2.0)/2.0, 0.0);
			VectorSet(&n[5].vec, 0.0, 0.0, 0.0);
			VectorSet(&n[6].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[7].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[8].vec, 1.0, 0.0, 0.0);
			VectorSet(&n[9].vec, 0.0, 0.0, 1.0);
			VectorSet(&n[10].vec, 0.0, 0.0, -1.0);
			root = &n[0];

			n[0].num_sub_nodes = 2;
			n[1].num_sub_nodes = 0;
			n[2].num_sub_nodes = 1;
			n[3].num_sub_nodes = 1;
			n[4].num_sub_nodes = 1;
			n[5].num_sub_nodes = 1;
			n[6].num_sub_nodes = 1;
			n[7].num_sub_nodes = 1;
			n[8].num_sub_nodes = 2;
			n[9].num_sub_nodes = 0;
			n[10].num_sub_nodes = 0;

			n[0].sub_node[0] = &n[1];
			n[0].sub_node[1] = &n[2];
			n[2].sub_node[0] = &n[3];
			n[3].sub_node[0] = &n[4];
			n[4].sub_node[0] = &n[5];
			n[5].sub_node[0] = &n[6];
			n[6].sub_node[0] = &n[7];
			n[7].sub_node[0] = &n[8];
			n[8].sub_node[0] = &n[9];
			n[8].sub_node[1] = &n[10];

			break;
		}
		default:
		{
			break;
		}
	}
}

void MakeCloudPoints(unsigned int seed)
{
	// for reproducibility.
	srand(seed);
	switch(num_cloud_pts)
	{
		case 17:
		{
			Vector *pt = pt_cloud;
			VectorSet(pt++, 1.f, 1.f, 1.f);
			VectorSet(pt++, -1.f, 1.f, 1.f);
			VectorSet(pt++, 1.f, -1.f, 1.f);
			VectorSet(pt++, -1.f, -1.f, 1.f);
			VectorSet(pt++, 1.f, 1.f, -1.f);
			VectorSet(pt++, -1.f, 1.f, -1.f);
			VectorSet(pt++, 1.f, -1.f, -1.f);
			VectorSet(pt++, -1.f, -1.f, -1.f);
			VectorSet(pt++, 0.f, 0.f, 0.f);
			VectorSet(pt++, 2.f, 2.f, 2.f);
			VectorSet(pt++, -2.f, 2.f, 2.f);
			VectorSet(pt++, 2.f, -2.f, 2.f);
			VectorSet(pt++, -2.f, -2.f, 2.f);
			VectorSet(pt++, 2.f, 2.f, -2.f);
			VectorSet(pt++, -2.f, 2.f, -2.f);
			VectorSet(pt++, 2.f, -2.f, -2.f);
			VectorSet(pt++, -2.f, -2.f, -2.f);
			break;
		}
		case 50:
		{
			int i, j;
			int lat_count, lon_count;
			lat_count = 8;
			lon_count = 8;
			GLfloat radius = 2.f;
			Vector *pt = pt_cloud;
			GLfloat lat_angle, lon_angle;
			GLfloat tmp_radius, z;
			for(i = 0; i < lat_count; i++)
			{
				lat_angle = PI * GLfloat(i) / GLfloat(lat_count - 1);
				tmp_radius = radius * sin(lat_angle);
				z = radius * cos(lat_angle);
				for(j = 0; j < lon_count; j++)
				{
					lon_angle = 2.f * PI * GLfloat(j) / GLfloat(lon_count);
					VectorSet(pt++, tmp_radius * cos(lon_angle), tmp_radius * sin(lon_angle), z);
					if(i == 0 || i == lat_count - 1)
						break;
				}
			}
			break;
		}
		case 64:
		{
			int i, j, k;
			Vector *pt = pt_cloud;
			double x, y, z;
			x = -1.5;
			for(i = 0; i < 4; i++)
			{
				y = -1.5;
				for(j = 0; j < 4; j++)
				{
					z = -1.5;
					for(k = 0; k < 4; k++)
					{
						VectorSet(pt++, x, y, z);
						z += 1.0;
					}
					y += 1.0;
				}
				x += 1.0;
			}
			break;
		}
		case 4096:
		{
			int i, j, k;
			Vector *pt = pt_cloud;
			double x, y, z;
			x = -7.5;
			for(i = 0; i < 16; i++)
			{
				y = -7.5;
				for(j = 0; j < 16; j++)
				{
					z = -7.5;
					for(k = 0; k < 16; k++)
					{
						VectorSet(pt++, x, y, z);
						z += 1.0;
					}
					y += 1.0;
				}
				x += 1.0;
			}
			break;
		}
		case 100:
		{
			int i, j;
			GLfloat ring_angle, tube_angle;
			GLfloat radius, y;
			GLfloat ring_radius = 3.0;
			GLfloat tube_radius = 0.5;
			int tube_sides = 10;
			int ring_sides = 10;
			Vector *pt = pt_cloud;
			for(i = 0; i < tube_sides; i++)
			{
				tube_angle = 2.f * PI * GLfloat(i) / GLfloat(tube_sides);
				radius = ring_radius + tube_radius * cos(tube_angle);
				y = sin(tube_angle);
				for(j = 0; j < ring_sides; j++)
				{
					ring_angle = 2.f * PI * GLfloat(j) / GLfloat(ring_sides);
					VectorSet(pt++, radius * cos(ring_angle), y, radius * sin(ring_angle));
				}
			}
			break;
		}
		default:
		{
			Vector *pt = pt_cloud;
			int k;
			for(k = 0; k < num_cloud_pts; k++)
			{
				pt->x = Random() * 6.0 - 3.0;
				pt->y = Random() * 6.0 - 3.0;
				pt->z = Random() * 6.0 - 3.0;
				pt++;
			}
			break;
		}
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.f, 0.f, zoom);
	glRotatef(rot.x, 1.f, 0.f, 0.f);
	glRotatef(rot.y, 0.f, 1.f, 0.f);
	glRotatef(rot.z, 0.f, 0.f, 1.f);

	if(draw_axis)
	{
		// draw the xyz-axis for reference.
		glLineWidth(1.f);
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
	}

#ifdef DEBUG_VOL_CODE
	DrawCandList();	
	if(draw_tets)
		DrawTetList(0.8);
	else
		DrawTriList();
#else
	glPushMatrix();
	if(center_cloud)
		glTranslatef(-cloud_center.x, -cloud_center.y, -cloud_center.z);
	glColor3f(1.f, 0.f, 1.f);
	glLineWidth(3.f);
	DrawTree(root);
	DrawPointCloud();
	DrawTriList();
	glPopMatrix();
#endif // DEBUG_VOL_CODE

	glutSwapBuffers();
}

void DrawPointCloud(void)
{
	glColor3f(0.f, 0.f, 0.f);
	glBegin(GL_POINTS);
	Vector *pt = pt_cloud;
	int k;
	for(k = 0; k < num_cloud_pts; k++, pt++)
		glVertex3f(pt->x, pt->y, pt->z);
	glEnd();
}

void DrawCandList(void)
{
	glColor3f(0.f, 0.f, 0.f);
	glBegin(GL_POINTS);
	CandidatePoint *cpt;
	Vector *pt;
	for(cpt = (CandidatePoint *)CandPtList.head; cpt; cpt = cpt->next)
	{
		pt = cpt->pt;
		glVertex3f(pt->x, pt->y, pt->z);
	}
	glEnd();
}

void DrawTriList(void)
{
	if(do_lighting)
	{
		glEnable(GL_LIGHTING);
		GLfloat mat_amb_dif[4] = {0.4f, 0.4f, 0.4f, 1.f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_dif);
	}
	glBegin(GL_TRIANGLES);
	int k;
	Vector *v;
	Triangle *tri;
#ifdef DEBUG_VOL_CODE
	for(tri = (Triangle *)TriList.head; tri; tri = tri->next)
	{		
		for(k = 0; k < 3; k++)
		{
			if(tri == (Triangle *)TriList.head)
				glColor3f(0.f, 1.f, 0.f);
			else
			{
				switch(k)
				{
					case 0: glColor3f(0.9f, 0.2f, 0.2f);	break;
					case 1: glColor3f(0.8f, 0.3f, 0.3f);	break;
					case 2: glColor3f(0.6f, 0.1f, 0.1f);	break;
				}
			}
			v = tri->vtx[k];
			glVertex3f(v->x, v->y, v->z);
		}		
	}
#endif // DEBUG_VOL_CODE
	Vector norm, v01, v02;
	for(tri = (Triangle *)TriMeshList.head; tri; tri = tri->next)
	{
		if(do_lighting)
		{
			VectorSub(&v01, tri->vtx[1], tri->vtx[0]);
			VectorSub(&v02, tri->vtx[2], tri->vtx[0]);
			VectorCross(&norm, &v01, &v02);
			VectorNormalize(&norm, &norm);
			glNormal3f(norm.x, norm.y, norm.z);
			if(ff == GL_CW)
				VectorScale(&norm, &norm, -1.f);
		}
		for(k = 0; k < 3; k++)
		{
			switch(k)
			{
				case 0: glColor3f(0.2f, 0.2f, 0.2f);	break;
				case 1: glColor3f(1.f, 1.f, 1.f);		break;
				case 2: glColor3f(0.5f, 0.5f, 0.5f);	break;
			}
			v = tri->vtx[k];
			glVertex3f(v->x, v->y, v->z);
		}
	}
	glEnd();
	if(do_lighting)
		glDisable(GL_LIGHTING);
}

void DrawTetList(double scale)
{
	Tetrahedron *tet;
	Triangle *tri;
	Vector center, vec;
	int i, j;
	glBegin(GL_TRIANGLES);
	for(tet = (Tetrahedron *)TetList.head; tet; tet = tet->next)
	{
		VectorAdd(&center, tet->v[0], tet->v[1]);
		VectorAdd(&center, &center, tet->v[2]);
		VectorAdd(&center, &center, tet->v[3]);
		VectorScale(&center, &center, 0.25);
		for(i = 0; i < 4; i++)
		{
			tri = tet->tri[i];
			for(j = 0; j < 3; j++)
			{
				switch(j)
				{
					case 0: glColor3f(1.f, 0.f, 0.f); break;
					case 1: glColor3f(0.f, 1.f, 0.f); break;
					case 2: glColor3f(0.f, 0.f, 1.f); break;
				}
				VectorSub(&vec, tri->vtx[j], &center);
				VectorScale(&vec, &vec, scale);
				VectorAdd(&vec, &vec, &center);
				glVertex3f(vec.x, vec.y, vec.z);
			}
		}
	}
	glEnd();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect_ratio = (h == 0) ? 0.f : (GLfloat(w) / GLfloat(h));
	gluPerspective(60.f, aspect_ratio, 0.1f, 1000.f);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'c':
		case 'C':
		{			
			glGetIntegerv(GL_FRONT_FACE, &ff);
			if(ff == GL_CCW)
				ff = GL_CW;
			else
				ff = GL_CCW;
			glFrontFace(ff);
			break;
		}
		case 27:
		{
			if(need_free)
				FreeTree(root);
			exit(0);
			break;
		}
#ifdef DEBUG_VOL_CODE
		case ' ':
		{
			int k = PerformVolumeAlgorithmIteration();
			printf("tri count = %d\n", k);
			break;
		}
		case 'g':
		case 'G':
		{
			while(PerformVolumeAlgorithmIteration())
				;	// null statement.
			break;
		}
		case 'v':
		case 'V':
		{
			double vol;
			vol = CalcVolumeFromTriMeshList();
			printf("vol from tri mesh = %f\n", vol);
			vol = CalcVolumeFromTetList();
			printf("vol from tet list = %f\n", vol);
			break;
		}
		case 't':
		case 'T':
		{
			draw_tets = 1 - draw_tets;
			break;
		}
		case 'n':
		case 'N':
		{
			printf("num tris = %d\n", TriMeshList.item_count);
			printf("num tets = %d\n", TetList.item_count);
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
		case 'r':
		case 'R':
		{
			ReadPointCloudData("debug.data");
			ResetVolumeAlgorithm(pt_cloud, num_cloud_pts);
			break;
		}
#else
		case ' ':
		{
			if(!OptimizeTreeIteration(root, root))
			{
				tweak_angle *= 0.8;
				if(tweak_angle < PI / 64.0)
					num_tweak_samples = 16;
				printf("tweak angle = %f\n", tweak_angle);
				printf("num samples = %d\n", num_tweak_samples);
			}
			update_tree_hull = 1;
			break;
		}
		case 'g':
		case 'G':
		{
			do_auto_optimization = 1 - do_auto_optimization;
			break;
		}
		case 'w':
		case 'W':
		{
			WritePointCloudData("debug.data");
			break;
		}
		case 'm':
		case 'M':
		{
			center_cloud = 1 - center_cloud;
			break;
		}
		case 'a':
		case 'A':
		{
			draw_axis = 1 - draw_axis;
			break;
		}
#endif // DEBUG_VOL_CODE
		case 'l':
		case 'L':
		{
			do_lighting = 1 - do_lighting;
			break;
		}
		case '[':
		{
			zoom -= 0.1;
			break;
		}
		case ']':
		{
			zoom += 0.1;
			break;
		}
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
	rot.x += GLfloat(mouse_dy);
	rot.y += GLfloat(mouse_dx);
	glutPostRedisplay();
}

double Random(void)
{
	return double(rand()) / double(RAND_MAX);
}

void WritePointCloudData(char *filename)
{
	FILE *fp = fopen(filename, "wb");
	fwrite((void *)&num_cloud_pts, sizeof(num_cloud_pts), 1, fp);
	fwrite((void *)pt_cloud, sizeof(Vector), num_cloud_pts, fp);
	fclose(fp);
	printf("data written...\n");
}

void ReadPointCloudData(char *filename)
{
	FILE *fp = fopen(filename, "rb");
	fread((void *)&num_cloud_pts, sizeof(num_cloud_pts), 1, fp);
	fread((void *)pt_cloud, sizeof(Vector), num_cloud_pts, fp);
	fclose(fp);
	printf("data read...\n");
}

// endof Main.cpp
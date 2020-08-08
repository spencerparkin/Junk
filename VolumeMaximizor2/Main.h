// Main.h

#ifndef __Main_h__
#define __Main_h__

#include "Vector.h"

#define KEY_UP_BIT		1
#define KEY_DN_BIT		2
#define KEY_LF_BIT		4
#define KEY_RT_BIT		8
#define KEY_PU_BIT		16
#define KEY_PD_BIT		32

extern GLfloat aspect_ratio;
extern int jt_idx;

// glut callback prototypes:
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void special(int key, int x, int y);
void special_up(int key, int x, int y);
void idle(void);

// misc. prototypes:
void PopulatePointCloud(int cloud, unsigned int seed, Vector *rot);
void DrawPointCloud(void);
void DrawPolygonList(void);
void DrawEdgeList(void);
double Random(void);
double Random(double min, double max);
void SaveCloud(char *filename);
void LoadCloud(char *filename);

#endif //__Main_h__

// endof Main.h
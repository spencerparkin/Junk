// Vector.h

#ifndef __Vector_h__
#define __Vector_h__

#include <GL/glut.h>

#define PI		3.1415926536
#define EPS		1e-6

#define MIN(a,b)		((a) < (b) ? (a) : (b))
#define MAX(a,b)		((a) > (b) ? (a) : (b))
#define SIGN(x)			((x) < 0.f ? -1.f : 1.f)
#define ABS(x)			((x) < 0.f ? -(x) : (x))
#define SWAP(t,a,b)		do { t tmp = (a); (a) = (b); (b) = tmp; } while(0)
#define _SWAP(a,b)		do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while(0)

struct Vector
{
	GLfloat x, y, z, w;
};

extern Vector Xaxis, Yaxis, Zaxis;
extern Vector Origin;
extern Vector Gravity;

// |a x b| = |a||b|sin t
// |a * b| = |a||b|cos t

void VectorSet(Vector *v, GLfloat x, GLfloat y, GLfloat z, GLfloat w = 1.f);
void VectorGet(Vector *v, GLfloat *x, GLfloat *y, GLfloat *z, GLfloat *w = 0);
void VectorCopy(Vector *v, Vector *v0);
int VectorEq(Vector *v0, Vector *v1);
void VectorAdd(Vector *v, Vector *v0, Vector *v1);
void VectorSub(Vector *v, Vector *v0, Vector *v1);
void VectorScale(Vector *v, Vector *v0, GLfloat s);
void VectorRescale(Vector *v, Vector *v0, GLfloat len);
void VectorNormalize(Vector *v, Vector *v0);
GLfloat VectorLength(Vector *v);
GLfloat VectorDistance(Vector *v0, Vector *v1);
GLfloat VectorDot(Vector *v0, Vector *v1);
void VectorCross(Vector *v, Vector *v0, Vector *v1);
void VectorRotate(Vector *v, Vector *v0, Vector *axis, GLfloat angle);
void VectorLerp(Vector *v, Vector *v0, Vector *v1, GLfloat t);
GLfloat VectorAngle(Vector *v0, Vector *v1);

#endif //__Vector_h__

// endof Vector.h
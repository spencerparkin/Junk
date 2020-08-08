// Vector.h

#ifndef __Vector_h__
#define __Vector_h__

#define PI		3.1415926536
#define EPS		1e-14 //1e-15 //1e-12 //1e-8 //1e-5

struct Vector
{
	double x, y, z, w;
};

extern Vector Xaxis, Yaxis, Zaxis;
extern Vector Origin;

// |a x b| = |a||b|sin t
// |a * b| = |a||b|cos t

void VectorSet(Vector *v, double x, double y, double z);
void VectorGet(Vector *v, double *x, double *y, double *z);
void VectorCopy(Vector *v, Vector *v0);
int VectorEq(Vector *v0, Vector *v1);
void VectorAdd(Vector *v, Vector *v0, Vector *v1);
void VectorSub(Vector *v, Vector *v0, Vector *v1);
void VectorScale(Vector *v, Vector *v0, double s);
void VectorNormalize(Vector *v, Vector *v0);
double VectorLength(Vector *v);
double VectorDistance(Vector *v0, Vector *v1);
double VectorDot(Vector *v0, Vector *v1);
void VectorCross(Vector *v, Vector *v0, Vector *v1);
void VectorRotate(Vector *v, Vector *v0, Vector *axis, double angle);
void VectorLerp(Vector *v, Vector *v0, Vector *v1, double t);
double VectorAngle(Vector *v0, Vector *v1);

#endif //__Vector_h__

// endof Vector.h
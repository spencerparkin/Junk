// Vector.cpp

#include "Vector.h"
#include "Quaternion.h"
#include <math.h>

// The standard orthogonal basis vectors for R^3.
Vector Xaxis = {1.f, 0.f, 0.f, 0.f};
Vector Yaxis = {0.f, 1.f, 0.f, 0.f};
Vector Zaxis = {0.f, 0.f, 1.f, 0.f};
Vector Origin = {0.f, 0.f, 0.f, 1.f};
Vector Gravity = {0.f, -1.f, 0.f, 0.f};		// Yeah, it's a bit lighter than earth gravity.

// Set elements of the given vector.
void VectorSet(Vector *v, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}

// Get element of the given vector.
void VectorGet(Vector *v, GLfloat *x, GLfloat *y, GLfloat *z, GLfloat *w)
{
	if(x)	*x = v->x;
	if(y)	*y = v->y;
	if(z)	*z = v->z;
	if(w)	*w = v->w;
}

// Copy vector data.
void VectorCopy(Vector *v, Vector *v0)
{
	v->x = v0->x;
	v->y = v0->y;
	v->z = v0->z;
	v->w = v0->w;
}

int VectorEq(Vector *v0, Vector *v1)
{
	return fabs(v0->x - v1->x) < EPS &&
			fabs(v0->y - v1->y) < EPS &&
			fabs(v0->z - v1->z) < EPS;
}

// Add two vectors,
void VectorAdd(Vector *v, Vector *v0, Vector *v1)
{
	v->x = v0->x + v1->x;
	v->y = v0->y + v1->y;
	v->z = v0->z + v1->z;
}

// Subtract two vectors.
void VectorSub(Vector *v, Vector *v0, Vector *v1)
{
	v->x = v0->x - v1->x;
	v->y = v0->y - v1->y;
	v->z = v0->z - v1->z;
}

// Scale a vector.
void VectorScale(Vector *v, Vector *v0, GLfloat s)
{
	v->x = v0->x * s;
	v->y = v0->y * s;
	v->z = v0->z * s;
}

// Rescale a vector to the desired length.
void VectorRescale(Vector *v, Vector *v0, GLfloat len)
{
	GLfloat mag = VectorLength(v0);
	VectorScale(v, v0, len / mag);
}

// Rescale a vector to be of unit length.
void VectorNormalize(Vector *v, Vector *v0)
{
	GLfloat length = VectorLength(v0);
	if(length != 0.f)
		VectorScale(v, v0, 1.f / length);
}

// Return the length of the given vector.
GLfloat VectorLength(Vector *v)
{
	return sqrt(VectorDot(v, v));
}

// Return the distance between the two given points.
GLfloat VectorDistance(Vector *v0, Vector *v1)
{
	Vector vec;
	VectorSub(&vec, v1, v0);
	return VectorLength(&vec);
}

// Compute the dot-product of two vectors.
GLfloat VectorDot(Vector *v0, Vector *v1)
{
	GLfloat dot =
			v0->x * v1->x +
			v0->y * v1->y +
			v0->z * v1->z;
	return dot;
}

// Compute the cross-product of two vectors.
// This operation is not commutative.
void VectorCross(Vector *v, Vector *v0, Vector *v1)
{
	static Vector tmp0, tmp1;

	if(v0 == v)
	{
		VectorCopy(&tmp0, v0);
		v0 = &tmp0;
	}

	if(v1 == v)
	{
		VectorCopy(&tmp1, v1);
		v1 = &tmp1;
	}

	v->x = v0->y * v1->z - v0->z * v1->y;
	v->y = v0->z * v1->x - v0->x * v1->z;
	v->z = v0->x * v1->y - v0->y * v1->x;
}

// Rotate the given vector about the given axis
// by the given angle measured in radians.
void VectorRotate(Vector *v, Vector *v0, Vector *axis, GLfloat angle)
{
	Quat q;
	QuatSetup(&q, axis, angle);	
	QuatTransform(&q, v, v0, 1);
}

// Perform a linear interpolation between the two
// given points by the given index.
void VectorLerp(Vector *v, Vector *v0, Vector *v1, GLfloat t)
{
	v->x = v0->x + (v1->x - v0->x) * t;
	v->y = v0->y + (v1->y - v0->y) * t;
	v->z = v0->z + (v1->z - v0->z) * t;
}

// Compute and return the angle between the
// two given vectors.  This will be in [0,pi].
GLfloat VectorAngle(Vector *v0, Vector *v1)
{
	Vector n0, n1;
	VectorNormalize(&n0, v0);
	VectorNormalize(&n1, v1);
	return acos(VectorDot(&n0, &n1));
}

// endof Vector.cpp
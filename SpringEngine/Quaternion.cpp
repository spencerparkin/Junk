// Quaternion.cpp

#include "Quaternion.h"
#include <math.h>

void QuatCopy(Quat *q, Quat *q0)
{
	q->w = q0->w;
	q->x = q0->x;
	q->y = q0->y;
	q->z = q0->z;
}

// Setup a rotational transformation
// quaternion that can rotate points
// about the given axis by the given angle.
void QuatSetup(Quat *q, Vector *axis, GLfloat angle)
{
	Vector vec;
	VectorNormalize(&vec, axis);
	angle *= 0.5;
	GLfloat ca = cos(angle);
	GLfloat sa = sin(angle);
	q->w = ca;
	q->x = vec.x * sa;
	q->y = vec.y * sa;
	q->z = vec.z * sa;
}

// Transform the given set of points
// through the given quaternion.
void QuatTransform(Quat *q, Vector *v, Vector *v0, int num_vecs)
{
	Vector uv, uuv;
	int k;
	for(k = 0; k < num_vecs; k++)
	{
		// This is a fast conjugation of q*v*q^-1 using
		// a vector form of quaternion multiplication,
		// knowing that the real parts cancel, and realizing
		// that the inverse of a normalized quaternion is
		// its conjugate.
		VectorCross(&uv, q, v);
		VectorCross(&uuv, q, &uv);
		VectorScale(&uv, &uv, 2.0 * q->w);
		VectorScale(&uuv, &uuv, 2.0);
		VectorAdd(v, v0, &uv);
		VectorAdd(v, v, &uuv);
		v++;
		v0++;
	}
}

// Perform a sphereical-linear interpolation
// between the two given quaternions by the
// given index.
void QuatSlerp(Quat *q, Quat *q0, Quat *q1, GLfloat t)
{
	// Not implemented yet...
}

// endof Quaternion.cpp
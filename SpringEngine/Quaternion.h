// Quaternion.h

#ifndef __Quaternion_h__
#define __Quaternion_h__

#include "Vector.h"

// Points and vectors just use the
// vector structure, but here I'll
// bother to typedef it.  But know
// that you can cast between the two
// at will.
typedef Vector Quat;

void QuatCopy(Quat *q, Quat *q0);
void QuatSetup(Quat *q, Vector *axis, GLfloat angle);
void QuatTransform(Quat *q, Vector *v, Vector *v0, int num_vecs);
void QuatSlerp(Quat *q, Quat *q0, Quat *q1, GLfloat t);

#endif //__Quaternion_h__

// endof Quaternion.h
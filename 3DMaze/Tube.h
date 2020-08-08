// Tube.h

#ifndef __Tube_h__
#define __Tube_h__

#define PI			3.14159263f
#define TWO_PI		(2.f * PI)

#define ABS(x)		((x) < 0 ? -(x) : (x))

struct Quat
{
	GLfloat W, X, Y, Z;
};

Quat QuatMult(Quat& Left, Quat& Right);

void RenderTube(GLfloat x0, GLfloat y0, GLfloat z0,
				GLfloat x1, GLfloat y1, GLfloat z1,
				int Density, GLfloat Radius);

#endif __Tube_h__

// endof Tube.h
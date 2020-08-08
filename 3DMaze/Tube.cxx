// Tube.cxx -- By Spencer T. Parkin

#include <GL/glut.h>
#include "Tube.h"
#include <math.h>

// This is the WORST example of how to
// draw a tube from one point to another.
void RenderTube(GLfloat x0, GLfloat y0, GLfloat z0,
				GLfloat x1, GLfloat y1, GLfloat z1,
				int Density, GLfloat Radius)
{
	// Calculate the tube axis and normalize it.
	Quat Axis =
	{
		1.f,
		x1 - x0,
		y1 - y0,
		z1 - z0,
	};
	GLfloat Scalar = 1.f / sqrt(Axis.X * Axis.X +
								Axis.Y * Axis.Y +
								Axis.Z * Axis.Z);
	Axis.X *= Scalar;
	Axis.Y *= Scalar;
	Axis.Z *= Scalar;

	// Cook up a quaternion we can
	// use to calculate the tube vertices.
	GLfloat HalfAngle = ((1.f / GLfloat(Density)) * TWO_PI) / 2.f;
	Quat Q =
	{
		Axis.W * cos(HalfAngle),
		Axis.X * sin(HalfAngle),
		Axis.Y * sin(HalfAngle),
		Axis.Z * sin(HalfAngle),
	};

	// The inverse of a normalized quaternion
	// is its conjugate.
	Quat InvQ =
	{
		Q.W,
		-Q.X,
		-Q.Y,
		-Q.Z
	};

	// Begin w/a vector mutually perpendicular
	// to the tube axis and the X axis or begin
	// w/the Z axis.
	Quat V;
#define EPSILON		0.001f
	if(ABS(Axis.X) - 1.f < EPSILON &&
		ABS(Axis.Y) < EPSILON &&
		ABS(Axis.Z) < EPSILON)
	{
		// Use Z-axis.
		V.W = 1.f;
		V.X = 0.f;
		V.Y = 0.f;
		V.Z = 1.f;
	}
	else
	{
		// Use cross-product of tube axis and X-axis.
		V.W = 1.f;
		V.X = 0.f;
		V.Y = Axis.Z;
		V.Z = -Axis.Y;
	}

	// Resize the vector to be the length of our tube radius.
	Scalar = Radius / sqrt(V.X * V.X + V.Y * V.Y + V.Z * V.Z);
	V.X *= Scalar;
	V.Y *= Scalar;
	V.Z *= Scalar;

	// Go draw the quads!!
	glBegin(GL_QUAD_STRIP);
//	glColor3f(1.f, 1.f, 1.f);

	int i;
	Quat N;

	for(i = 0; i <= Density; i++)
	{
		N = V;
		Scalar = 1.f / sqrt(N.X * N.X + N.Y * N.Y + N.Z * N.Z);
		N.X *= Scalar;
		N.Y *= Scalar;
		N.Z *= Scalar;
		//normalize(&N.X);

		glNormal3f(N.X, N.Y, N.Z);

		glVertex3f(x0 + V.X, y0 + V.Y, z0 + V.Z);
		glVertex3f(x1 + V.X, y1 + V.Y, z1 + V.Z);

		if(i + 1 <= Density)
		{
			// p' = qpq^-1	(Their is a much faster way to do this.)
			V = QuatMult(Q, V);
			V = QuatMult(V, InvQ);
			// This chained calculation may cause round-off
			// error that would cause the tube to have a
			// seem or overlay a little bit.
		}
	}

	glEnd();
}

Quat QuatMult(Quat& Left, Quat& Right)
{
	Quat Q =
	{
		// W =
		Left.W * Right.W -
		Left.X * Right.X -
		Left.Y * Right.Y -
		Left.Z * Right.Z,

		// X =
		Left.W * Right.X +
		Left.X * Right.W +
		Left.Y * Right.Z -
		Left.Z * Right.Y,

		// Y =
		Left.W * Right.Y -
		Left.X * Right.Z +
		Left.Y * Right.W +
		Left.Z * Right.X,

		// Z =
		Left.W * Right.Z +
		Left.X * Right.Y -
		Left.Y * Right.X +
		Left.Z * Right.W
	};

	return(Q);
}

// endof Tube.cxx
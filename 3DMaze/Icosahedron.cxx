// Icosahedron.cxx - Got this from my OpenGL book.

// TODO: I should use this code to create a node network
//       at the vertices of a subdivided icosahedron.  This
//       would make an interesting maze!

#include <GL/glut.h>
#include <math.h>
#include "Icosahedron.h"

#define X	.525731112119133606
#define Z	.850650808352039932

GLfloat IcosVertices[ICOS_VERTICES][3] =
{
	{-X, 0.f, Z}, {X, 0.f, Z}, {-X, 0.f, -Z}, {X, 0.f, -Z},
	{0.f, Z, X}, {0.f, Z, -X}, {0.f, -Z, X}, {0.f, -Z, -X},
	{Z, X, 0.f}, {-Z, X, 0.f}, {Z, -X, 0.f}, {-Z, -X, 0.f},
};

int IcosTriangles[ICOS_FACES][3] =
{
	{0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
	{8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
	{7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
	{6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11},
};

void normalize(GLfloat *vec)
{
	GLfloat scalar = 1.f / sqrt(
				vec[0] * vec[0] +
				vec[1] * vec[1] +
				vec[2] * vec[2]);

	vec[0] *= scalar;
	vec[1] *= scalar;
	vec[2] *= scalar;
}

static void RenderIcosTriangle(GLfloat *v0, GLfloat *v1, GLfloat *v2, int SubDivisionDepth, GLfloat Radius)
{
	if(SubDivisionDepth > 0)
	{
		GLfloat v01[3], v12[3], v20[3];
		int i;

		for(i = 0; i < 3; i++)
		{
			v01[i] = v0[i] + v1[i];
			v12[i] = v1[i] + v2[i];
			v20[i] = v2[i] + v0[i];
		}

		normalize(v01);
		normalize(v12);
		normalize(v20);

		RenderIcosTriangle(v0, v01, v20, SubDivisionDepth - 1, Radius);
		RenderIcosTriangle(v1, v12, v01, SubDivisionDepth - 1, Radius);
		RenderIcosTriangle(v2, v20, v12, SubDivisionDepth - 1, Radius);
		RenderIcosTriangle(v01, v12, v20, SubDivisionDepth - 1, Radius);
	}
	else
	{
		glNormal3fv(v0);
		glVertex3f(
				v0[0] * Radius,
				v0[1] * Radius,
				v0[2] * Radius);

		glNormal3fv(v1);
		glVertex3f(
				v1[0] * Radius,
				v1[1] * Radius,
				v1[2] * Radius);

		glNormal3fv(v2);
		glVertex3f(
				v2[0] * Radius,
				v2[1] * Radius,
				v2[2] * Radius);
	}
}

void RenderIcosahedron(GLfloat x, GLfloat y, GLfloat z, int SubDivisionDepth, GLfloat Radius)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_TRIANGLES);

	int i;

	for(i = 0; i < ICOS_FACES; i++)
	{
		RenderIcosTriangle(
						IcosVertices[IcosTriangles[i][0]],
						IcosVertices[IcosTriangles[i][1]],
						IcosVertices[IcosTriangles[i][2]],
						SubDivisionDepth, Radius);
	}

	glEnd();
	glPopMatrix();
}

// endof Icosahedron.cxx
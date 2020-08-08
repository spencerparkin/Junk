// Matrix.cpp

#include "Matrix.h"
#include <string.h>
#include <math.h>

void MatrixIdent(Matrix *m)
{
	memset((void *)m, 0, sizeof(Matrix));
	m->ele[0][0] = 1.f;
	m->ele[1][1] = 1.f;
	m->ele[2][2] = 1.f;
	m->ele[3][3] = 1.f;
}

void MatrixCopy(Matrix *m, Matrix *m0)
{
	memcpy((void *)m, (void *)m0, sizeof(Matrix));
}

void MatrixMult(Matrix *m, Matrix *m0, Matrix *m1)
{
	static Matrix tmp_m0, tmp_m1;

	if(m == m0)
	{
		MatrixCopy(&tmp_m0, m0);
		m0 = &tmp_m0;
	}

	if(m == m1)
	{
		MatrixCopy(&tmp_m1, m1);
		m1 = &tmp_m1;
	}

	int i, j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			m->ele[i][j] =
							m0->ele[i][0] * m1->ele[0][j] +
							m0->ele[i][1] * m1->ele[1][j] +
							m0->ele[i][2] * m1->ele[2][j] +
							m0->ele[i][3] * m1->ele[3][j];
		}
	}
}

void MatrixScale(Matrix *m, GLfloat sx, GLfloat sy, GLfloat sz)
{
	MatrixIdent(m);

	m->ele[0][0] = sx;
	m->ele[1][1] = sy;
	m->ele[2][2] = sz;
}

void MatrixRotXaxis(Matrix *m, GLfloat angle)
{
	GLfloat ca = cos(angle);
	GLfloat sa = sin(angle);

	MatrixIdent(m);

	m->ele[1][1] = ca;
	m->ele[1][2] = -sa;
	m->ele[2][1] = sa;
	m->ele[2][2] = ca;
}

void MatrixRotYaxis(Matrix *m, GLfloat angle)
{
	GLfloat ca = cos(angle);
	GLfloat sa = sin(angle);

	MatrixIdent(m);

	m->ele[0][0] = ca;
	m->ele[0][2] = sa;
	m->ele[2][0] = -sa;
	m->ele[2][2] = ca;
}

void MatrixRotZaxis(Matrix *m, GLfloat angle)
{
	GLfloat ca = cos(angle);
	GLfloat sa = sin(angle);

	MatrixIdent(m);

	m->ele[0][0] = ca;
	m->ele[0][1] = -sa;
	m->ele[1][0] = sa;
	m->ele[1][1] = ca;
}

void MatrixRotAxisAngle(Matrix *m, Vector *axis, GLfloat angle)
{
	GLfloat ca = cos(angle);
	GLfloat sa = sin(angle);
	GLfloat om_ca = 1.f - ca;

	MatrixIdent(m);

	GLfloat xx = axis->x * axis->x;
	GLfloat xy = axis->x * axis->y;
	GLfloat xz = axis->x * axis->z;
	GLfloat yy = axis->y * axis->y;
	GLfloat yz = axis->y * axis->z;
	GLfloat zz = axis->z * axis->z;	

	m->ele[0][0] = ca + om_ca * xx;
	m->ele[0][1] = om_ca * xy - sa * axis->z;
	m->ele[0][2] = om_ca * xz + sa * axis->y;
	m->ele[1][0] = om_ca * xy + sa * axis->z;
	m->ele[1][1] = ca + om_ca * yy;
	m->ele[1][2] = om_ca * yz - sa * axis->x;
	m->ele[2][0] = om_ca * xz - sa * axis->y;
	m->ele[2][1] = om_ca * yz + sa * axis->x;
	m->ele[2][2] = ca + om_ca * zz;
}

void MatrixTranslate(Matrix *m, Vector *vec)
{
	MatrixIdent(m);
	m->ele[0][3] = vec->x;
	m->ele[1][3] = vec->y;
	m->ele[2][3] = vec->z;
}

void MatrixTransform(Matrix *m, Vector *vec, int num_vecs)
{
	Vector tmp;

	while(num_vecs-- > 0)
	{
		VectorCopy(&tmp, vec);

		vec->x = m->ele[0][0] * tmp.x +
				 m->ele[0][1] * tmp.y +
				 m->ele[0][2] * tmp.z +
				 m->ele[0][3] * tmp.w;

		vec->y = m->ele[1][0] * tmp.x +
				 m->ele[1][1] * tmp.y +
				 m->ele[1][2] * tmp.z +
				 m->ele[1][3] * tmp.w;

		vec->z = m->ele[2][0] * tmp.x +
				 m->ele[2][1] * tmp.y +
				 m->ele[2][2] * tmp.z +
				 m->ele[2][3] * tmp.w;

		vec->w = m->ele[3][0] * tmp.x +
				 m->ele[3][1] * tmp.y +
				 m->ele[3][2] * tmp.z +
				 m->ele[3][3] * tmp.w;

		vec++;
	}
}

// endof Matrix.cpp
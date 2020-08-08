// Matrix.h

#ifndef __Matrix_h__
#define __Matrix_h__

#include <GL/glut.h>
#include "Vector.h"

struct Matrix
{
	GLfloat ele[4][4];
};

void MatrixIdent(Matrix *m);
void MatrixCopy(Matrix *m, Matrix *m0);
void MatrixMult(Matrix *m, Matrix *m0, Matrix *m1);	// UNTESTED
void MatrixScale(Matrix *m, GLfloat sx, GLfloat sy, GLfloat sz);
void MatrixRotXaxis(Matrix *m, GLfloat angle);
void MatrixRotYaxis(Matrix *m, GLfloat angle);
void MatrixRotZaxis(Matrix *m, GLfloat angle);
void MatrixRotAxisAngle(Matrix *m, Vector *axis, GLfloat angle);	// UNTESTED
void MatrixTranslate(Matrix *m, Vector *vec);
void MatrixTransform(Matrix *m, Vector *vec, int num_vecs = 1);

#endif //__Matrix_h__

// endof Matrix.h
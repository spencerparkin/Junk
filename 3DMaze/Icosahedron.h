// Icosahedron.h

#ifndef __Icosahedron_h__
#define __Icosahedron_h__

#define ICOS_FACES		20
#define ICOS_VERTICES	12

extern GLfloat IcosVertices[ICOS_VERTICES][3];
extern int IcosTriangles[ICOS_FACES][3];

void RenderIcosahedron(GLfloat x, GLfloat y, GLfloat z, int SubDivisionDepth, GLfloat Radius);

#endif __Icosahedron_h__

// endof Icosahedron.h
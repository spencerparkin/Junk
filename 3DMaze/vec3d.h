// vec3d.h

#ifndef __vec3d_h__
#define __vec3d_h__

#include <GL/glut.h>

struct vec3d
{
//	GLfloat w, x, y, z;
	GLfloat x, y, z;
};

void		vec3d_set		(vec3d *dest, GLfloat x, GLfloat y, GLfloat z, GLfloat w = 1.0);
void		vec3d_get		(vec3d *src, GLfloat *x, GLfloat *y, GLfloat *z, GLfloat *w = 0);
void		vec3d_copy		(vec3d *dest, vec3d *src);
void		vec3d_scale		(vec3d *dest, vec3d *src, GLfloat scale);
void		vec3d_add		(vec3d *dest, vec3d *src_left, vec3d *src_right);
void		vec3d_sub		(vec3d *dest, vec3d *src_left, vec3d *src_right);
GLfloat		vec3d_dot		(vec3d *src_left, vec3d *src_right);
void		vec3d_neg		(vec3d *dest, vec3d *src);
void		vec3d_cross		(vec3d *dest, vec3d *src_left, vec3d *src_right);
GLfloat		vec3d_mag2		(vec3d *src);
void		vec3d_norm		(vec3d *dest, vec3d *src);
void		vec3d_homog		(vec3d *dest, vec3d *src);
GLfloat		vec3d_angle		(vec3d *src0, vec3d *src1);

#define ABS(x)		((x) < 0 ? -(x) : (x))
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))

#endif __vec3d_h__

// endof vec3d.h
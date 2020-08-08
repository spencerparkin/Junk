// vec3d.cpp -- By Spencer T. Parkin

#include <math.h>
#include "vec3d.h"

void vec3d_set(vec3d *dest, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
//	dest->w = w;
	dest->x = x;
	dest->y = y;
	dest->z = z;
}

void vec3d_get(vec3d *src, GLfloat *x, GLfloat *y, GLfloat *z, GLfloat *w)
{
//	if(w)	*w = src->w;
	if(x)	*x = src->x;
	if(y)	*y = src->y;
	if(z)	*z = src->z;
}

void vec3d_copy(vec3d *dest, vec3d *src)
{
	*dest = *src;
}

void vec3d_scale(vec3d *dest, vec3d *src, GLfloat scale)
{
	dest->x = scale * src->x;
	dest->y = scale * src->y;
	dest->z = scale * src->z;
}

void vec3d_add(vec3d *dest, vec3d *src_left, vec3d *src_right)
{
	dest->x = src_left->x + src_right->x;
	dest->y = src_left->y + src_right->y;
	dest->z = src_left->z + src_right->z;
}

void vec3d_sub(vec3d *dest, vec3d *src_left, vec3d *src_right)
{
	dest->x = src_left->x - src_right->x;
	dest->y = src_left->y - src_right->y;
	dest->z = src_left->z - src_right->z;
}

GLfloat vec3d_dot(vec3d *src_left, vec3d *src_right)
{
	return(src_left->x * src_right->x +
		   src_left->y * src_right->y +
		   src_left->z * src_right->z);
}

void vec3d_neg(vec3d *dest, vec3d *src)
{
	vec3d_scale(dest, src, -1.f);
}

void vec3d_cross(vec3d *dest, vec3d *src_left, vec3d *src_right)
{
	vec3d u, v;

	if(dest == src_left) { u = *src_left; src_left = &u; }
	if(dest == src_right) { v = *src_right; src_right = &v; }

	dest->x = src_left->y * src_right->z - src_left->z * src_right->y;
	dest->y = src_left->z * src_right->x - src_left->x * src_right->z;
	dest->z = src_left->x * src_right->y - src_left->y * src_right->x;
}

GLfloat vec3d_mag2(vec3d *src)
{
	return(vec3d_dot(src, src));
}

void vec3d_norm(vec3d *dest, vec3d *src)
{
	vec3d_scale(dest, src, 1.0f / sqrt(vec3d_mag2(src)));
}

/*
void vec3d_homog(vec3d *dest, vec3d *src)
{
	vec3d_scale(dest, src, 1.f / src->w);
}
*/

GLfloat vec3d_angle(vec3d *src0, vec3d *src1)
{
	GLfloat angle;
	vec3d norm0, norm1;

	vec3d_norm(&norm0, src0);
	vec3d_norm(&norm1, src1);

	angle = acos(vec3d_dot(&norm0, &norm1));

	return angle;
}
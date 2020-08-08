// Field.h

#ifndef __Field_h__
#define __Field_h__

struct Field
{
	GLfloat ga, la;
};

void MakeField(Field *f, vec3d *eye, vec3d *v0, vec3d *v1);
void MakeField(Field *f, GLfloat look_angle, GLfloat fovi_angle);
int FieldOverlap(Field *f0, Field *f1);
void FieldIntersection(Field *fdest, Field *fsrc0, Field *fsrc1);

#endif __Field_h__

// endof Field.h
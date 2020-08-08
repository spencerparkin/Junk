// artifact.h

// Programmed by Spencer T. Parkin

#ifndef __artifact_h__
#define __artifact_h__

#include <glut.h>
#include "fovi.h"

// An artifact is an object that lives in a
// cell and can potentially be taken possesion
// of by an entity and used on the environment.
class Artifact
{
	public:

	Artifact();
	~Artifact();

	bool IsVisible(fovi *f);
	virtual bool HandleAction(class Entity *e);
	virtual void Draw(fovi *f);
	virtual void DrawInventoryItem(void) = 0;
	virtual bool UseOn(class Entity *e) = 0;	// Use this artifact on the given entity.
	virtual bool UseOn(class Door *d) = 0;		// Use this artifact on the given door.
	virtual bool UseOn(class Artifact *a) = 0;	// Use this artifact on the given artifact.
	void Rotate(void);

	float x, y;
	float radius;
	float angle;
	GLuint dpy_list;
	class Cell *c;
	class Entity *e;
	float r, g, b;
};

#endif __artifact_h__

// endof artifact.h
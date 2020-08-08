// wall.h

// Programmed by Spencer T. Parkin

#ifndef __wall_h__
#define __wall_h__

#include "surface.h"
#include "texture.h"

class Wall : public Surface
{
	public:

	Wall();
	~Wall();

	virtual void Draw(fovi *f);
	virtual bool HandleAction(class Entity *e);

	float r, g, b;
	texture *t;
	bool stretch;
};

#define WALL_HEIGHT		2.f

#endif __wall_h__

// endof wall.h
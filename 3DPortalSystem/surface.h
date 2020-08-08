// surface.h

// Programmed by Spencer T. Parkin

#ifndef __surface_h__
#define __surface_h__

#include "line.h"
#include "fovi.h"

// Surfaces are objects subject to
// visible-surface-determination and
// collision-detection.  This is an
// abstract base class.
class Surface
{
	public:

	Surface();
	~Surface();

	bool IsVisible(fovi *f);
	virtual bool IsEntityVisible(fovi *f, class Entity *e);
	virtual bool IsPortal(void) { return false; }
	virtual bool HandleCollision(class Entity *e);
	virtual bool HandlePropagation(class Entity *e, line *m);
	virtual bool HandleAction(class Entity *e) = 0;
	virtual void Draw(fovi *f) = 0;

	line l;			// Coordinates local to a cell.
	class Cell *c;	// The cell this surface belongs to.
	line t;			// Texture coordinates for the floor/ceiling.
	float length;	// The length of this surface from a topological perspective.
	bool clip;		// Draw only the visible parts of this surface if set.
};

#endif __surface_h__

// endof surface.h
// portal.h

// Programmed by Spencer T. Parkin

#ifndef __portal_h__
#define __portal_h__

#include "surface.h"

class Portal : public Surface
{
public:

	Portal();
	~Portal();

	virtual bool IsEntityVisible(fovi *f, class Entity *e);
	virtual bool IsPortal(void) { return true; }
	virtual bool HandleCollision(Entity *e);
	virtual bool HandlePropagation(Entity *e, line *m);
	virtual bool HandleAction(class Entity *e);
	virtual void Draw(fovi *f);

	class Cell *adj_c;		// The adjacent cell.
	float dx, dy;			// The location of the adjacent cell WRT the cell containing this portal.
	float da;				// The orientation of the adjacent cell WRT the cell containing this portal.
	bool no_pass;	// Set if computer controlled entities are restricted from passing through this portal.
};

extern Portal *spy_portal;	// Used by the entity-visibility-determination algorithm.

#endif __portal_h__

// endof portal.h
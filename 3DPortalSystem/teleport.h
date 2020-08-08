// teleport.h

// Programmed by Spencer T. Parkin

#ifndef __teleport_h__
#define __teleport_h__

#include "artifact.h"

class Teleport : public Artifact
{
	public:

	Teleport();
	~Teleport();

	virtual bool HandleAction(class Entity *e);
	virtual void Draw(fovi *f);
	virtual void DrawInventoryItem(void);

	// Teleports can't be used on anything, but
	// they can be used to teleport entities.
	virtual bool UseOn(class Entity *e) { return false; }
	virtual bool UseOn(class Door *d) { return false; }
	virtual bool UseOn(class Artifact *a) { return false; }

	bool has_power;	// True if this teleport is activated.
	Cell *target_c;		// The cell we are teleported to.
	float target_x, target_y;
	float target_heading;
};

#endif __teleport_h__

// endof teleport.h
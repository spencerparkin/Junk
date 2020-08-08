// orb.h

// Programmed by Spencer T. Parkin

#ifndef __orb_h__
#define __orb_h__

#include "artifact.h"

enum OrbType
{
	ORB_OF_MOCHRIE,
	ORB_OF_STILES,
	ORB_OF_PROOPS,
	ORB_OF_BRADY,
	NUM_OF_ORBS,
};

class Orb : public Artifact
{
	public:

	Orb();
	~Orb();

	virtual bool HandleAction(class Entity *e);
	virtual void Draw(fovi *f);
	virtual void DrawInventoryItem(void) {}	// Orbs are not kept.

	// For now, orbs aren't used, just collected at
	// the end of level.
	virtual bool UseOn(class Entity *e) { return false; };
	virtual bool UseOn(class Door *d) { return false; };
	virtual bool UseOn(class Artifact *a) { return false; };

	OrbType orb;
	float rot_x, rot_y, rot_z;
};

#endif __orb_h__

// endof orb.h
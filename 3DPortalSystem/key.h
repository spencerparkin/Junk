// key.h

// Programmed by Spencer T. Parkin

#ifndef __key_h__
#define __key_h__

#include "artifact.h"

class Key : public Artifact
{
public:

	Key();
	~Key();

	virtual void Draw(fovi *f);
	virtual void DrawInventoryItem(void);
	virtual bool UseOn(Entity *e);
	virtual bool UseOn(Door *d);
	virtual bool UseOn(Artifact *a);

	// A pointer to the door this key unlocks.
	// Remember that any one of two correspdoning
	// doors represents a logical door.  We we
	// only need a pointer to one of the doors here.
	Door *d;
};

#endif __key_h__

// endof key.h
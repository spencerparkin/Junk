// blob.h

// Programmed by Spencer T. Parkin

#ifndef __blob_h__
#define __blob_h__

#include "entity.h"

// Blobs are wonderful entities.  They hang
// around in one place and look in all
// directions.  If they see you, they attach
// a tether to you, and then relentlessly
// track you until they kill you or the
// tether is broken somehow.  The tether may
// be broken by using a teleport.  Also, a
// good statagy for dealing with blobs is
// finding a room to confine them in.  Blobs
// aren't programmed to try and open doors,
// although they easily could be.
class Blob : public Entity
{
	public:

	Blob();
	~Blob();

	virtual void Draw(fovi *f);		// Draw the blob!
	virtual void Drive(void);		// Drive the blob!
	void TetherBreakNotify(tether *teth);

	tether *teth_to_target;
	Entity *target_e;
	bool spy_flag, sucking_life;
};

#endif __blob_h__

// endof blob.h
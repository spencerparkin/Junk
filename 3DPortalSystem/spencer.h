// spencer.h

// Programmed by Spencer T. Parkin

#ifndef __spencer_h__
#define __spencer_h__

#include "entity.h"

// This is the game's main character.
class Spencer : public Entity
{
	public:

	Spencer();
	~Spencer();

	virtual void Draw(fovi *f);
	virtual void Drive(void);

	// The walk animation sequence of textures.
	texture *t_walk[4];

	// The standing texture.
	texture *t_stand;

	float prev_x, prev_y;
	float anim_frame;
};

extern Spencer *spencer;

#endif __spencer_h__

// endof spencer.h
// snowman.h

// Programmed by Spencer T. Parkin

#ifndef __snowman_h__
#define __snowman_h__

#include "entity.h"

// The snow man romes the scene graph
// aimlessly until it spots you in its
// field of vision.  Then it comes after
// you and tries to kill you.  When it's
// not doing that it's meandering around
// and if it sees a switch or a door it
// tries to mess with it.  He may also
// try to take posession of things the
// user needs.
class SnowMan : public Entity
{
	public:

	SnowMan();
	~SnowMan();

	virtual void Draw(fovi *f);
	virtual void Drive(void);

	class Portal *target_p;
	class Cell *recent_c;
	float target_x, target_y;
	float target_heading;
	Entity *target_e;
	bool spy_flag;
	texture *t_happy, *t_mad;	// Snowmen are happy, or they are MAD!!
	texture *t_backside;	// You guessed it.
};

#endif __snowman_h__

// endof snowman.h
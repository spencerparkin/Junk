// switch.h

// Programmed by Spencer T. Parkin

#ifndef __switch_h__
#define __switch_h__

#include "wall.h"
#include "texture.h"

enum SwitchState
{
	SWITCH_STATE_ON,
	SWITCH_STATE_OFF,
};

class Switch : public Wall
{
	public:

	Switch();
	~Switch();

	virtual bool HandleAction(class Entity *e);

	texture *t_on, *t_off;		// On/Off textures.
	SwitchState state;			// On/Off state.
	void (* switch_CB)(SwitchState state, void *data[2]);	// Manipulates the 2 objects.
	void *data[2];			// 2 unkown objects this switch operates upon.
};

#endif __switch_h__

// endof switch.h
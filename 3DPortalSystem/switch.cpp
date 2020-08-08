// switch.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "switch.h"

Switch::Switch()
{
	t_on = 0;
	t_off = 0;
	state = SWITCH_STATE_OFF;
	switch_CB = 0;
	stretch = true;
	clip = false;
}

Switch::~Switch()
{
}

// Toggle between the on/off states.
// Make sure that the wall's texture
// reflect the state of the switch.
// Call the switch call-back routine
// if one was provided by in the map.
bool Switch::HandleAction(Entity *e)
{
	switch(state)
	{
		case SWITCH_STATE_ON:
		{
			state = SWITCH_STATE_OFF;
			t = t_off;
			break;
		}
		case SWITCH_STATE_OFF:
		{
			state = SWITCH_STATE_ON;
			t = t_on;
			break;
		}
	}

	if(switch_CB)
		switch_CB(state, data);

	return false;
}

// endof switch.cpp
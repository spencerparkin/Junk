// door.h

// Programmed by Spencer T. Parkin

#ifndef __door_h__
#define __door_h__

#include "portal.h"
#include "texture.h"
#include <time.h>

// This is the amount of time in seconds
// that it takes for the door to open or closed.
#define DOOR_OPEN_CLOSE_TIME	3.f

enum DoorState
{
	DOOR_STATE_CLOSED,
	DOOR_STATE_OPENNING,
	DOOR_STATE_OPEN,
	DOOR_STATE_CLOSING,
};

class Door : public Portal
{
	public:

	Door();
	~Door();

	virtual bool IsEntityVisible(fovi *f, class Entity *e);
	virtual bool IsPortal(void);
	virtual bool HandleCollision(Entity *e);
	virtual bool HandlePropagation(Entity *e, line *m);
	virtual bool HandleAction(class Entity *e);
	virtual void Draw(fovi *f);
	bool OpenDoor(Entity *e);
	bool CloseDoor(Entity *e);
	void UnlockDoor(void);
	void LockDoor(void);
	void ChangeState(DoorState state);

	// This is a pointer to the door associated
	// with this door in the adjacent cell.  The
	// reason we have two door objects for every
	// logical door is because edges are not shared
	// between cells in the representation of the scene graph.
	Door *d;

	// This is the current state of the door.
	// Is it open?  Closed?  Openning?  Closing?
	DoorState state;

	// This determines whether or not we are
	// allowed to open the door if it is closed.
	bool is_locked;

	// A parameter used in animating the openning
	// and closing of the door.
	float t;

	// Used to determine which door gets drawn.
	int draw_flag;

	// Double door textures.
	texture *t_left_door, *t_right_door;
};

#endif __door_h__

// endof door.h
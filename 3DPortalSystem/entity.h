// entity.h

// Programmed by Spencer T. Parkin

#ifndef __entity_h__
#define __entity_h__

#include "fovi.h"
#include "texture.h"

// This is the maximum number of artifacts
// an entity can carry around at any given time.
#define ARTIFACTS_PER_ENTITY	20

// This is the maximum number of tethers
// an entity may be attached to.
#define TETHERS_PER_ENTITY		10

// Entities are agents capable of
// traversing the scene graph.  Note
// that computer controlled entities
// could be programmed to try and
// act on their environment.  Currently
// this is not the case.
class Entity
{
	public:

	Entity();
	~Entity();

	virtual bool IsVisible(fovi *f);	// Is this entity in the given fovi?
	virtual void Draw(fovi *f);	// Draw this entity.
	virtual void Drive(void) = 0;	// Make the entity do stuff.
	bool Move(float dx, float dy);	// Used to traverse the scene graph.
	bool Strafe(float ds);		// Strafe left/right.
	bool MoveForward(float ds);		// Move forward/backward.
	void Turn(float da);	// Turn left/right.
	bool Propagate(class Portal *p);	// Go to an adjacent cell.
	virtual bool Act(void);		// Act on the environment.
	bool CanSee(Entity *e);		// Can this entity see the given entity?
	bool TakeArtifact(class Artifact *a);
	bool DiscardArtifact(class Artifact *a);
	bool Navigate(float target_x, float target_y, float move_speed, float turn_speed);
	virtual void TetherMadeNotify(struct tether *teth) {}	// Called when tether attached to this entity.
	virtual void TetherBreakNotify(struct tether *teth) {}	// Called when tether detached from this entity.

	float x, y;		// The local position (relative to the cell.)
	fovi f;			// The global position/field-of-vision.
	float radius;	// The enclosing collision object.  (A cylinder.)
	class Cell *c;	// The occupied cell.
	texture *t;		// A sprite texture for this entity, instead of some geometry.
	float t0, t1;	// Texture coordinates.
	GLuint dpy_list;	// Used to display the entity's geometry.
	class Artifact *inventory[ARTIFACTS_PER_ENTITY];	// Buffer to hold inventory items.
	float health;	// Health percentage in the range [0,1].
	struct tether *teth[TETHERS_PER_ENTITY];	// Each is an end-point to a tether.
};

// The entity system routines:
void InitEntityBuffer(void);
bool SpawnEntity(Entity *e, Cell *c);
bool KillEntity(Entity *e);
void DriveEntities(void);

#endif __entity_h__

// endof entity.h
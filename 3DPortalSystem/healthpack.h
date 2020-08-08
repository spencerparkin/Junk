// healthpack.h

// Programmed by Spencer T. Parkin

#ifndef __healthpack_h__
#define __healthpack_h__

#include "artifact.h"

enum HealthPackType
{
	SMALL_HEALTH_PACK,		// 25% health restoration.
	LARGE_HEALTH_PACK,		// 50% health restoration.
	MEGA_HEALTH_PACK,		// Full health restoration.
};

class HealthPack : public Artifact
{
	public:

	HealthPack();
	~HealthPack();

	virtual bool HandleAction(Entity *e);
	virtual void Draw(fovi *f);
	virtual void DrawInventoryItem(void) {}	// Health-packs are not kept.
	virtual bool UseOn(Entity *e);
	virtual bool UseOn(Door *d);
	virtual bool UseOn(Artifact *a);
	void CalcRadius(void);
	GLfloat SideLength(void);

	HealthPackType type;
	struct job *hp_job;
};

#endif __healthpack_h__

// endof healthpack.h
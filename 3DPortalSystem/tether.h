// tether.h

// Programmed by Spencer T. Parkin

#ifndef __tether_h__
#define __tether_h__

#include "cell.h"
#include "portal.h"
#include "entity.h"
#include "LL.h"

// This is a path described by
// portals through the scene graph
// connecting a source entity to
// a destination entity.
struct tether
{
	// This is an ordered list of adjacent cells.
	LL path;
	
	// These are the entities said to be
	// attached by this tether object.
	Entity *src, *dst;
};

// Call these before and after each level
// to do initialization and clean-up of
// the memory used by the tethering system.
void InitTetherSystem(void);
void DeinitTetherSystem(void);

// This routine does a breadth-first search to
// find the shortest path between the two given entities.
// It returns this path in the form of a tether object.
// Notice that this module manages all the tether memory.
bool EstablishTether(tether **teth, Entity *src, Entity *dst);

// This routine is called from the propagation routine
// to maintain the integrity of the given tether.  It also
// perform path optimizations as nessisary.
bool MaintainTether(tether *teth, Entity *e, Portal *p);

// This frees the memory allocated by the tether
// establishment routine.
bool DiscardTether(tether *teth);

// Return the portal that the source entity would
// go through in order to close in on the target entity.
Portal *ReturnClosingPortal(tether *teth);

//...
#define SRC_WRT_DST		0
#define DST_WRT_SRC		1
void CalcTetherCoords(tether *teth, int coord_wrt, float *x, float *y, float *angle);

#endif __tether_h__

// endof tether.h
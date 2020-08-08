// tether.cpp

// Programmed by Spencer T. Parkin

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tether.h"
#include "buffer.h"
#include "main.h"
#include "cell.h"
#include "map.h"

// This module will maintain a list of
// the established tethers and be
// responsible for freeing the memory
// it allocates for each one.
static LL tether_list;

// Wipe the tether list.
void InitTetherSystem(void)
{
	memset((void *)&tether_list, 0, sizeof(LL));
}

// Note that this must be called before
// entity memory is dealocated.
void DeinitTetherSystem(void)
{
	LL_node *n;
	while(tether_list.num_items > 0)
	{
		n = tether_list.head;
		DiscardTether((tether *)n->data);
	}
}

// Establish a shortest-path tether between the
// two given entities.  Note that tethers are bilateral.
// It doesn't really matter which entity is the source,
// and which is the destination.  The terms are used
// mostly to differentiate between the two entities
// that are attached by the tether.  False is returned if
// something went wrong here.  The given tether pointer is
// initialized for the callers use.
bool EstablishTether(tether **teth, Entity *src, Entity *dst)
{
	LL_node *n;
	*teth = new tether;
	memset((void *)&(*teth)->path, 0, sizeof(LL));
	(*teth)->src = (*teth)->dst = 0;
	n = new LL_node(*teth);
	LL_insert(&tether_list, n);

	LL frontier, explored;
	memset((void *)&frontier, 0, sizeof(LL));
	memset((void *)&explored, 0, sizeof(LL));

	src->c->flags |= TETH_FLAG;
	src->c->parent = 0;
	LL_insert(&frontier, new LL_node(src->c));

	// Perform a breadth-first-search of the scene graph
	// for the destination entity rooted at the source entity.
	Cell *c;
	int k;
	while(frontier.num_items > 0)
	{
		// Remove the head of the frontier list and
		// insert it into the explored list.
		n = frontier.head;
		LL_remove(&frontier, n);
		LL_insert(&explored, n);

		// Explore this node.  Look for the destination entity.
		c = (Cell *)n->data;
		for(k = 0; k < ENTITIES_PER_CELL; k++)
			if(c->e[k] == dst)
				break;
		
		// If we found the destination entity,
		// then build up the shortest path to it
		// from the source.  Notice that in a
		// graph with cycles, the shortest path
		// may not be unique, but I just care about
		// finding at least one of them, so it
		// doesn't matter which one.
		if(k < ENTITIES_PER_CELL)
		{
			while(c)
			{
				LL_insert(&(*teth)->path, new LL_node(c));
				c = c->parent;
			}
			break;
		}

		// Append to the tail of the frontier list
		// all unexplored adjacent cells.  Build
		// up a tree that is gradually spanning the
		// graph in a breadth-first fasion.  Notice
		// that we're using the frontier list here
		// as a queue.  This ensures that we search
		// nearer cells before those further away.
		Portal *p;
		for(k = 0; k < SURFACES_PER_CELL; k++)
		{
			if(c->s[k] && c->s[k]->IsPortal())
			{
				p = (Portal *)c->s[k];
				if(!(p->adj_c->flags & TETH_FLAG))
				{
					p->adj_c->flags |= TETH_FLAG;
					p->adj_c->parent = c;
					LL_append(&frontier, new LL_node(p->adj_c));
				}
			}
		}
	}

	// Free all the explored nodes.
	while(explored.num_items > 0)
	{
		n = explored.head;
		((Cell *)n->data)->flags &= ~TETH_FLAG;
		LL_remove(&explored, n);
		delete n;
	}

	// Free all the unexplored frontier
	// nodes that we didn't have to explore.
	while(frontier.num_items > 0)
	{
		n = frontier.head;
		((Cell *)n->data)->flags &= ~TETH_FLAG;
		LL_remove(&frontier, n);
		delete n;
	}

	// If we couldn't find the destination
	// entity from the source entity's position,
	// then we failed to create the tether.
	// Abort the making of this tether.
	if((*teth)->path.num_items == 0)
	{
		DiscardTether(*teth);
		*teth = 0;
		return false;
	}

	// Finally, indicate the entities involved
	// in this holy union.  Amen and amen.
	InsertIntoBuffer((void **)src->teth, TETHERS_PER_ENTITY, *teth);
	InsertIntoBuffer((void **)dst->teth, TETHERS_PER_ENTITY, *teth);
	(*teth)->src = src;
	(*teth)->dst = dst;
	src->TetherMadeNotify(*teth);
	dst->TetherMadeNotify(*teth);
	return true;
}

// Update the given tether to reflect the propagation
// of the given entity through the given portal.
// The entity is assumed to be a member of this tether.
// With all the allocation/deallocation done in this
// routine, there is a bit of a concern here about thrashing
// the free-store and maybe memory fragmentation.
bool MaintainTether(tether *teth, Entity *e, Portal *p)
{
	LL_node *n;
	LL_node *new_n = 0;
	if(e == teth->src)
	{
		if(teth->path.num_items > 1 &&
				p->adj_c == (Cell *)teth->path.head->next->data)
		{
			n = teth->path.head;
			LL_remove(&teth->path, n);
			delete n;
		}
		// If there is only one cell in the path list
		// or we're not moving to the next cell in
		// the path list from the head cell, then we
		// need to add a cell to the path.
		else
		{
			new_n = new LL_node(p->adj_c);
			LL_insert(&teth->path, new_n);
		}
	}
	else if(e == teth->dst)
	{
		if(teth->path.num_items > 1 &&
				p->adj_c == (Cell *)teth->path.tail->prev->data)
		{
			n = teth->path.tail;
			LL_remove(&teth->path, n);
			delete n;
		}
		else
		{
			new_n = new LL_node(p->adj_c);
			LL_append(&teth->path, new_n);
		}
	}
	else
	{
		return false;
	}

	// Check for cycles in the tether's path.
	// If a cycle is found, remove it.  Notice
	// that this cycle optimization does not
	// garentee that we always maintain the shortest
	// path between the source and destination,
	// but it will be of help.  If we always wanted
	// the shortest path, then we'd have to (or at
	// least, I wouldn't know how to do it any other
	// way than to) do the BFS every frame, and
	// that's just not a good idea for our frame rate.
	// A cycle is potentially created in the path
	// only when a new cell was added to the path.
	if(new_n)
	{
		int count = 0;
		for(n = teth->path.head; n; n = n->next)
		{
			count += (n->data == new_n->data);
			if(count > 1)
				break;
		}

		// If we detected a cycle, remove it.
		if(count > 1)
		{
			LL_node *prev_n;
			for(;;)
			{
				count -= (n->data == new_n->data);
				if(count == 0)
					break;
				prev_n = n->prev;
				LL_remove(&teth->path, n);
				n = prev_n;
			}
			printf("Cycle removed!\n");
		}
	}

	return true;
}

// Free the memory taken up by the given
// tether and release the entities tied
// together by this tether.
bool DiscardTether(tether *teth)
{
	if(!teth)
		return false;

	// Remove this tether from the main tether list.
	// To do this, we need to go find it in the list.
	// The list shouldn't be that big.
	LL_node *n;
	for(n = tether_list.head; n; n = n->next)
	{
		if(n->data == teth)
		{
			LL_remove(&tether_list, n);
			break;
		}
	}
	if(!n)
		fatal_error("Unable to remove tether from main tether list!");

	// Now remove all the tether data.
	while(teth->path.num_items > 0)
	{
		n = teth->path.head;
		LL_remove(&teth->path, n);
		delete n;
	}

	// Indicate that these entities are no longer
	// tied together by the given tether.  These
	// pointers may be null if the making of the
	// tether went wrong and needed to be aborted.
	if(teth->src)
	{
		teth->src->TetherBreakNotify(teth);
		RemoveFromBuffer((void **)teth->src->teth, TETHERS_PER_ENTITY, teth);
	}
	if(teth->dst)
	{
		teth->dst->TetherBreakNotify(teth);
		RemoveFromBuffer((void **)teth->dst->teth, TETHERS_PER_ENTITY, teth);
	}

	// Finally, delete the tether object itself.
	delete teth;
	return true;
}

// Return the portal that the source entity would
// go through in order to close in on the target entity.
// If null is returned, then the source and destination
// entities must be in the same cell.
Portal *ReturnClosingPortal(tether *teth)
{
	// Do a sanity check.
	if((Cell *)teth->path.head->data != teth->src->c ||
		(Cell *)teth->path.tail->data != teth->dst->c)
	{
		fatal_error("Tether doesn't work!  Path end-points incorrect.");
	}

	if(teth->src->c == teth->dst->c)
	{
		// Do another sanity check.
		if(teth->path.num_items != 1)
			fatal_error("Tether doesn't work!  Singleton path expected.");
		return(0);
	}

	// Do anoter sanity check.
	if(teth->path.num_items < 2)
		fatal_error("Tether doesn't work!  Multi-cell path expected.");

	// Return the portal exiting the source entity's
	// occupying cell and entering the next cell in
	// the path that should be adjacent to it.
	Cell *adj_c = (Cell *)teth->path.head->next->data;
	Portal *p = teth->src->c->ReturnPortal(adj_c);

#if 0	// Actually, this isn't an error (if no
		// portal is returned) because we might
		// have trapped the entity behind a door!
		// In fact, this is a good stratagy for
		// dealing with the blobs.
	// Do yet another sanity check.
	if(!p)
		fatal_error("The tether doesn't work!  Adjacent cell wasn't really adjacent.");
#endif

	return(p);
}

// Calculate the coordinates of one entity of
// the given tether with respect to the cell
// occupied by the other entity.
void CalcTetherCoords(tether *teth, int coord_wrt, float *x, float *y, float *angle)
{
	if(coord_wrt == DST_WRT_SRC)
	{
		// In this mode we need to first calculate
		// the coordinates and orientations of each
		// cell in the path relative to one another.
		// Yes, this is a bit rediculous.
		if(cell_mode == DYNAMIC_CELL_MODE)
		{
			Cell *c = (Cell *)teth->path.head->data;
			c->x = 0.f;
			c->y = 0.f;
			c->angle = 0.f;
			LL_node *n;
			Cell *adj_c;
			Portal *p;
			for(n = teth->path.head->next; n; n = n->next)
			{
				adj_c = (Cell *)n->data;
				p = c->ReturnPortal(adj_c);
				adj_c->CalcPosAngle(p);
				c = adj_c;
			}
		}

		// Now convert the destination entity's position and orientation
		// from what it is in its current cell's coordinate system
		// to that WRT the coordinate system of the source entity's cell.
		teth->dst->c->LtoG(x, y, teth->dst->x, teth->dst->y);
		teth->src->c->GtoL(x, y, *x, *y);
		*angle = teth->dst->f.heading + teth->dst->c->angle;
	}
	else if(coord_wrt == SRC_WRT_DST)
	{
		if(cell_mode == DYNAMIC_CELL_MODE)
		{
			Cell *c = (Cell *)teth->path.tail->data;
			c->x = 0.f;
			c->y = 0.f;
			c->angle = 0.f;
			LL_node *n;
			Cell *adj_c;
			Portal *p;
			for(n = teth->path.tail->prev; n; n = n->prev)
			{
				adj_c = (Cell *)n->data;
				p = c->ReturnPortal(adj_c);
				adj_c->CalcPosAngle(p);
				c = adj_c;
			}
		}

		teth->src->c->LtoG(x, y, teth->src->x, teth->src->y);
		teth->dst->c->GtoL(x, y, *x, *y);
		*angle = teth->src->c->angle + teth->src->f.heading;
	}
}

// endof tether.cpp
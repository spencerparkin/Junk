// Node.h

#ifndef __Node_h__
#define __Node_h__

#include "vec3d.h"
#include "Field.h"

#define LINKS	10

#define NAV_STACKED		0x00000001
#define NAV_TOUCHED		0x00000002
#define NAV_TRACE		0x00000004
#define NAV_BUBBLE		0x00000008

#define BUBBLE_THRESHOLD	30

class Node
{
	public:

	Node();
	~Node();

	void DeterminePotentialLinks(void);
	void DeterminePotentialLinksOfNeighbors(void);
	void LinkPathway(void);
	void AddToList(void);
	void SubFromList(void);
	void Render(int Tube);
	void PatchNavigationLinks(void);
	int Navigate(void);
	int TraceBubble(Node *N = 0);

	// These routines are designed specificaly
	// for the CorridorMaze class.
	void DrawWalls(Field *Vision, vec3d *Position);
	void DrawWalls(void);
	void DrawWall(int Wall);
	
	Node *Skeleton[LINKS];	// neighboring nodes
	Node *Link[LINKS];		// potential pathways
	Node *Pathway;			// final pathway
	int NumLinks;			// # of potential pathways
	Node *Next, *Prev;		// list pointers
	vec3d Pos;				// node coordinates
	Node *NavLink[LINKS];	// navigation links
	int NavFlags;			// navigation flags
	int NavCount;			// navigation count
	Node *Caller;			// used in depth-first search algorithm
};

extern Node *NavTarget;

int ProcessNodeWithPotential(void);
void GenerateMaze(Node *N, int Now);

#endif __Node_h__

// endof Node.h
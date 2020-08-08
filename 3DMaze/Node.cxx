// Node.cxx - Generate Mazes with network skeletons - By Spencer T. Parkin

#include <GL/glut.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "vec3d.h"
#include "Node.h"
#include "Skeleton.h"
#include "Tube.h"
#include "Icosahedron.h"

static Node *NodeList = 0;
static int NodeListCount = 0;

Node::Node()
{
	memset(this, 0, sizeof(Node));
}

Node::~Node()
{
}

// Update potential pathway links for this node.
void Node::DeterminePotentialLinks(void)
{
	NumLinks = 0;

	// If no final pathway has been chosen yet.
	if(!Pathway)
	{
		int i;

		for(i = 0; i < LINKS; i++)
		{
			// Potential pathways are neighboring
			// nodes with final pathways chosen.
			if(Skeleton[i] && Skeleton[i]->Pathway)
			{
				Link[NumLinks++] = Skeleton[i];
			}
		}

		if(NumLinks > 0)
		{
			// Make sure we're in the list of
			// nodes to be processed.
			AddToList();
		}
	}
}

// Update potential pathway links for this nodes neighboring nodes.
void Node::DeterminePotentialLinksOfNeighbors(void)
{
	int i;

	for(i = 0; i < LINKS; i++)
	{
		if(Skeleton[i])
		{
			Skeleton[i]->DeterminePotentialLinks();
		}
	}
}

// Finalize this nodes pathway.
void Node::LinkPathway(void)
{
	// We better have links or we're in trouble.
	assert(NumLinks > 0);
	if(NumLinks > 0)
	{
		// Choose a final pathway at random.
		Pathway = Link[rand() % NumLinks];
		NumLinks = 0;

		// A final pathway has been chosen so
		// we need to update the potential pathways
		// for all neigboring nodes.
		DeterminePotentialLinksOfNeighbors();
	}

	// Take us out of the list of
	// nodes to be processed.
	SubFromList();
}

// Add this node to the nodes-w/potential list.
void Node::AddToList(void)
{
	// If we're not already in the list.
	if(!(Next || Prev))
	{
		// Become the new list-pointer.
		Next = NodeList;
		Prev = NULL;

		if(Next)
		{
			Next->Prev = this;
		}

		NodeList = this;

		// Bump up the list-item count.
		NodeListCount++;
	}
}

// Subtract this node from the nodes-w/potential list.
void Node::SubFromList(void)
{
	// If we're in the list.
	if(Next || Prev || this == NodeList)
	{
		// Decouple from the list.
		if(Next) Next->Prev = Prev;
		if(Prev) Prev->Next = Next;

		// Adjust list-pointer if nessisary.
		if(this == NodeList)
		{
			NodeList = Next;
		}

		// Bump down the list-item count.
		NodeListCount--;
	}
}

// Draw this node linking to its pathway or
// show its potential links if unfinalized.
void Node::Render(int Tube)
{
	GLfloat X, Y, Z;

	vec3d_get(&Pos, &X, &Y, &Z);

#if 1
	if(Pathway)
	{
		if(Tube)
		{
			// These will be joints for the tubes.
			RenderIcosahedron(X, Y, Z, 1, 0.12f);

			// Draw the pathways as tubes.
			RenderTube(X, Y, Z, Pathway->Pos.x, Pathway->Pos.y, Pathway->Pos.z, 6, 0.12f);
		}
		else
		{
			if(NavFlags & NAV_BUBBLE)
			{
				glColor3f(1.f, 0.f, 0.f);

				glVertex3f(X - 0.2f, Y - 0.2f, Z);
				glVertex3f(X - 0.2f, Y + 0.2f, Z);

				glVertex3f(X - 0.2f, Y + 0.2f, Z);
				glVertex3f(X + 0.2f, Y + 0.2f, Z);

				glVertex3f(X + 0.2f, Y + 0.2f, Z);
				glVertex3f(X + 0.2f, Y - 0.2f, Z);

				glVertex3f(X + 0.2f, Y - 0.2f, Z);
				glVertex3f(X - 0.2f, Y - 0.2f, Z);
			}

			if((NavFlags & NAV_STACKED) && (Pathway->NavFlags & NAV_STACKED))
			{
				glColor3f(0.f, 0.f, 1.f);
			}
			else if(NavFlags & NAV_TOUCHED)			
			{
				glColor3f(1.f, 0.f, 0.f);
			}
			else
			{
				glColor3f(1.f, 1.f, 1.f);
			}

			glVertex3f(X, Y, Z);
			glVertex3f(Pathway->Pos.x, Pathway->Pos.y, Pathway->Pos.z);
		}
	}
	else if(NumLinks > 0 && !Tube)
	{
		int i;
		
		glColor3f(1.f, 0.f, 0.f);

		for(i = 0; i < NumLinks; i++)
		{
			glVertex3f(X, Y, Z);
			glVertex3f(Link[i]->Pos.x, Link[i]->Pos.y, Link[i]->Pos.z);
		}
	}
#else
	int i;

	for(i = 0; i < LINKS; i++)
	{
		if(Skeleton[i])
		{
			glVertex3f(X, Y, Z);
			glVertex3f(Skeleton[i]->Pos.x, Skeleton[i]->Pos.y, Skeleton[i]->Pos.z);
		}
	}
#endif
}

// Patch all pathways of this node.
// This should be called after the maze has been generated.
void Node::PatchNavigationLinks(void)
{
	int i;

	for(i = 0; i < LINKS; i++)
	{
		if(Skeleton[i])
		{
			if(Pathway == Skeleton[i] || Skeleton[i]->Pathway == this)
			{
				NavLink[i] = Skeleton[i];
			}
		}
	}
}

// Randomly choose a node with potential pathways
// and have it randomly choose its final pathway.
int ProcessNodeWithPotential(void)
{
	// If there are any more nodes to process.
	if(NodeList)
	{
		int i;

//		i = rand() % NodeListCount;
		i = rand() % (NodeListCount < 10 ? NodeListCount : 10);

		Node *N = NodeList;

		while(i--)
		{
			N = N->Next;
		}

		N->LinkPathway();
	}

	return(NodeList != 0);
}

// Randomly generate a maze with only one possible
// way to travel between any two different nodes.
// It is assumed that node network skeleton has
// already been constructed.
void GenerateMaze(Node *N, int Now)
{
	NodeList = 0;
	NodeListCount = 0;

	// Seed the random number by what time it is.
	srand(unsigned(time(NULL)));

	// This code ensures that the maze
	// will only have one solution.
	int k = 0;
	while(!N->Skeleton[k])
	{
		k++;
		assert(k < LINKS);
		if(k >= LINKS)return;
	}
	N->Link[0] = N->Skeleton[k];
	N->NumLinks = 1;
	N->LinkPathway();
	N->Link[0]->LinkPathway();

	// We may want to animate the maze generation process.
	if(Now)
	{
		// Link all other nodes at random.
		while(ProcessNodeWithPotential()){}
	}
}

// To fill in the bubble we need to find
// the closed circuit.

// This idea will not work!
#if 0
int Node::TraceBubble(Node *N)
{
	// Have we come full circle?
	if(this == N)
	{
		return(1);
	}

	if(!N)
	{
		N = this;
	}

	int i;

	for(i = 0; i < LINKS; i++)
	{
		if(Skeleton[i] && (Skeleton[i]->NavFlags & NAV_TRACE))
		{
			if(Skeleton[i]->TraceBubble(N))
			{
				NavFlags |= NAV_BUBBLE;
				return(1);
			}
		}
	}

	return(0);
}
#endif

Node *NavTarget = 0;

int Node::Navigate(void)
{
	NavFlags |= NAV_STACKED;

	if(this == NavTarget)
	{
		return(1);
	}

	int i, j;

#if 1

	if(Caller)
	{
		NavCount = Caller->NavCount + 1;
	}

	// This only works for hallow (2D) mazes like the torus or any flat maze.
	for(i = 0; i < LINKS; i++)
	{
		if(Skeleton[i]) // && Skeleton[i] != Caller)
		{
			if(Skeleton[i]->NavFlags & NAV_STACKED)
			{
//				Skeleton[i]->NavFlags |= NAV_TOUCHED;

				// Potential bubble condition:
				if(NavCount - Skeleton[i]->NavCount > BUBBLE_THRESHOLD)
				{
					// Trace the bubble...

					Node *N = this;

					while(N != Skeleton[i])
					{
						for(j = 0; j < LINKS; j++)
						{
							if(N->Skeleton[j] && !(N->Skeleton[j]->NavFlags & NAV_STACKED))
							{
								N->Skeleton[j]->NavFlags |= NAV_BUBBLE; //NAV_TRACE;
							}
						}

						N = N->Caller;
					}

					/*
					for(j = 0; j < LINKS; j++)
					{
						if(Skeleton[i] && (Skeleton[i]->NavFlags & NAV_TRACE))
						{
							Skeleton[i]->TraceBubble();
						}
					}*/

					//tmp...
					return(1);

					break;
				}
			}
		}
	}

	// A better way might be to weight the directions closer
	// to the exit greater than those leading away from the exit.
	// The probability would be in favor of the directions
	// going towards the exit.

	// Traverse the maze systematicaly in a random fasion...
	
	int GoFlag[LINKS], k = 0;

	for(i = 0; i < LINKS; i++)
	{
		GoFlag[i] = (NavLink[i] && !(NavLink[i]->NavFlags & NAV_STACKED));
		k += GoFlag[i];
	}

	while(k > 0)
	{
		if(k > 1)
		{
			do
			{
				j = rand() % LINKS;
			}
			while(!GoFlag[j]);
		}
		else
		{
			j = 0;

			while(!GoFlag[j])
			{
				j++;
			}
		}

		if(!(NavLink[j]->NavFlags & NAV_BUBBLE))
		{
			NavLink[j]->Caller = this;
			if(NavLink[j]->Navigate())
			{
				return(1);
			}
		}

		GoFlag[j] = 0;

		k--;
	}

	Caller = 0;

#else
	// Traverse the maze systematicaly in an orderly fasion...

	for(i = 0; i < LINKS; i++)
	{
		if(NavLink[i] && !(NavLink[i]->NavFlags & NAV_STACKED))
		{
			if(NavLink[i]->Navigate())
			{
				return(1);
			}
		}
	}
#endif

	NavFlags &= ~NAV_STACKED;

	return(0);
}

void Node::DrawWalls(Field *Vision, vec3d *Position)
{
	vec3d box[4];

	static GLfloat dx = BOX_WIDTH / 2.f;
	static GLfloat dy = BOX_LENGTH / 2.f;

	// Note that this must correlate with the DrawWall routine
	// and so with the way we linked the skeletal matrix.  Dumb.
	vec3d_set(&box[0], Position->x - dx, Position->y - dy, 0.f);
	vec3d_set(&box[1], Position->x - dx, Position->y + dy, 0.f);
	vec3d_set(&box[2], Position->x + dx, Position->y + dy, 0.f);
	vec3d_set(&box[3], Position->x + dx, Position->y - dy, 0.f);

	int k;
	Field LineOfSite;

	for(k = 0; k < 4; k++)
	{
		MakeField(&LineOfSite, Position, &box[k], &box[(k + 1) % 4]);

		if(FieldOverlap(Vision, &LineOfSite))
		{
			if(NavLink[k])
			{
				Field PartialView;
				FieldIntersection(&PartialView, Vision, &LineOfSite);
				NavLink[k]->DrawWalls(&PartialView, Position);
			}
			else
				DrawWall(k);
		}
	}
}

void Node::DrawWalls(void)
{
	// This is a test routine.  We're going to just
	// draw all the walls regardless of visibility.

	int i;
	for(i = 0; i < 4; i++)
		if(!NavLink[i])
			DrawWall(i);
}

void Node::DrawWall(int Wall)
{
	// We assume that we're in a GL_QUADS block

	GLfloat dx, dy, dz;
	
	dx = BOX_WIDTH / 2.f;
	dy = BOX_LENGTH / 2.f;
	dz = BOX_DEPTH / 2.f;

	// Just for fun, color the wall according to position.
	vec3d Color;
	vec3d_norm(&Color, &Pos);
	glColor3f(ABS(Color.x), ABS(Color.y), ABS(Color.z));

	// Notice that each case must be in correlation
	// with the way in which we linked the skeletal matrix.
	switch(Wall)
	{
		case 0: // -x
		{
			glVertex3f(Pos.x - dx, Pos.y - dy, Pos.z - dz);
			glVertex3f(Pos.x - dx, Pos.y + dy, Pos.z - dz);
			glVertex3f(Pos.x - dx, Pos.y + dy, Pos.z + dz);
			glVertex3f(Pos.x - dx, Pos.y - dy, Pos.z + dz);
			break;
		}
		case 1: // +y
		{
			glVertex3f(Pos.x - dx, Pos.y + dy, Pos.z - dz);
			glVertex3f(Pos.x + dx, Pos.y + dy, Pos.z - dz);
			glVertex3f(Pos.x + dx, Pos.y + dy, Pos.z + dz);
			glVertex3f(Pos.x - dx, Pos.y + dy, Pos.z + dz);
			break;
		}
		case 2: // +x
		{
			glVertex3f(Pos.x + dx, Pos.y - dy, Pos.z - dz);
			glVertex3f(Pos.x + dx, Pos.y + dy, Pos.z - dz);
			glVertex3f(Pos.x + dx, Pos.y + dy, Pos.z + dz);
			glVertex3f(Pos.x + dx, Pos.y - dy, Pos.z + dz);
			break;
		}				
		case 3: // -y
		{
			glVertex3f(Pos.x - dx, Pos.y - dy, Pos.z - dz);
			glVertex3f(Pos.x + dx, Pos.y - dy, Pos.z - dz);
			glVertex3f(Pos.x + dx, Pos.y - dy, Pos.z + dz);
			glVertex3f(Pos.x - dx, Pos.y - dy, Pos.z + dz);
			break;
		}		
	}
}

// endof Node.cxx
// BSP_Tree.h

#ifndef __BSP_Tree_h__
#define __BSP_Tree_h__

#include "Vector.h"
#include "Polygon.h"
#include "List.h"
#include "Spring.h"

// A node of the tree represents one of
// the subspaces and has pointers to two
// subspaces of this space, each divided
// by the partitioning plane.  If this
// node is a leaf, then position of the
// polygons in this space is arbitrary.
// If it is a branch, then they must be
// coplanar to the partitioning plane.
struct BSP_Node
{	
	Plane plane;							// The partitioning plane.
	List pgon_list;							// List of polygons stored in this space.
	BSP_Node *front, *back;					// Front and back subspace pointers.
};

// The type of BSP tree supported here
// is one that implements an oct-tree.
BSP_Node *BSP_Create(List *pgon_list, MemPool *vtx_mp, int max_pgons_per_leaf);
void BSP_Destroy(BSP_Node *root);
void BSP_DrawDebug(BSP_Node *root);
void BSP_Render(BSP_Node *root, int frustum);
void BSP_CollideSphere(BSP_Node *root, Vector *center, GLfloat radius);
void BSP_CollideCylinder(BSP_Node *root, Vector *v0, Vector *v1, GLfloat radius);
void BSP_CollideLattice(BSP_Node *root, Lattice *lattice);

#endif //__BSP_Tree_h__

// endof BSP_Tree.h
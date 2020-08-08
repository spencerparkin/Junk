// BSP_Tree.cpp

#include "BSP_Tree.h"
#include <math.h>
#include <string.h>

static void ChooseBestPartitioningPlane(Plane *plane, List *pgon_list)
{
	Vector center;
	VectorSet(&center, 0.f, 0.f, 0.f);
	Vector min, max;
	VectorSet(&max, -9999.f, -9999.f, -9999.f);
	VectorSet(&min, 9999.f, 9999.f, 9999.f);

	Polygon *pgon;
	int num_vertices = 0, k;
	for(pgon = (Polygon *)pgon_list->head; pgon; pgon = pgon->next)
	{
		for(k = 0; k < pgon->num_vertices; k++)
		{
			min.x = MIN(min.x, pgon->vtx[k]->x);
			min.y = MIN(min.y, pgon->vtx[k]->y);
			min.z = MIN(min.z, pgon->vtx[k]->z);

			max.x = MAX(max.x, pgon->vtx[k]->x);
			max.y = MAX(max.y, pgon->vtx[k]->y);
			max.z = MAX(max.z, pgon->vtx[k]->z);

			VectorAdd(&center, &center, pgon->vtx[k]);
			num_vertices++;
		}
	}

	VectorScale(&center, &center, 1.f / GLfloat(num_vertices));
	Vector extent;
	VectorSub(&extent, &max, &min);

	GLfloat biggest_dim = MAX(MAX(fabs(extent.x), fabs(extent.y)), fabs(extent.z));

	if(biggest_dim == fabs(extent.x))
		MakePlane(plane, &center, &Xaxis);
	else if(biggest_dim == fabs(extent.y))
		MakePlane(plane, &center, &Yaxis);
	else
		MakePlane(plane, &center, &Zaxis);
}

// The given list of polygons is assumed to be
// allocated by the free-store.  Here, polygons
// in the list may be deleted and new polygons allocated.
// The given list is emptied.
BSP_Node *BSP_Create(List *pgon_list, MemPool *vtx_mp, int max_pgons_per_leaf)
{
	BSP_Node *node = new BSP_Node;
	memset((void *)node, 0, sizeof(BSP_Node));

	if(pgon_list->item_count <= max_pgons_per_leaf)
		memcpy((void *)&node->pgon_list, (void *)pgon_list, sizeof(List));	
	else
	{
		List front_list, back_list;
		WipeList(&front_list);
		WipeList(&back_list);		

		ChooseBestPartitioningPlane(&node->plane, pgon_list);

		Polygon *pgon, pgon_front, pgon_back;
		while(pgon_list->item_count > 0)
		{
			pgon = (Polygon *)pgon_list->head;
			ListRemove((ListItem *)pgon);

			switch(SplitPolygon(&node->plane, pgon, &pgon_front, &pgon_back, vtx_mp))
			{
				default:
				case PGON_IN_PLANE:
				{
					ListInsert(&node->pgon_list, (ListItem *)pgon);
					break;
				}
				case PGON_ON_PLANE_FRONT:
				{
					ListInsert(&front_list, (ListItem *)pgon);
					break;
				}
				case PGON_ON_PLANE_BACK:
				{
					ListInsert(&back_list, (ListItem *)pgon);
					break;
				}
				case PGON_STRADDLES_PLANE:
				{
					delete pgon;
					Polygon *new_pgon = new Polygon;
					memcpy((void *)new_pgon, (void *)&pgon_front, sizeof(Polygon));
					ListInsert(&front_list, (ListItem *)new_pgon);
					new_pgon = new Polygon;
					memcpy((void *)new_pgon, (void *)&pgon_back, sizeof(Polygon));
					ListInsert(&back_list, (ListItem *)new_pgon);
					break;
				}
			}
		}

		if(front_list.item_count > 0)
			node->front = BSP_Create(&front_list, vtx_mp, max_pgons_per_leaf);
		if(back_list.item_count > 0)
			node->back = BSP_Create(&back_list, vtx_mp, max_pgons_per_leaf);
	}

	return node;
}

// This will not only delete the nodes of
// the given BSP tree, but it will also
// delete the polygons in the tree.
void BSP_Destroy(BSP_Node *root)
{
	if(root->front)
		BSP_Destroy(root->front);
	if(root->back)
		BSP_Destroy(root->back);
	ListItem *item;
	while(root->pgon_list.item_count > 0)
	{
		item = root->pgon_list.head;
		ListRemove(item);
		delete item;
	}
	delete [] root;
}

void BSP_DrawDebug(BSP_Node *root)
{
	
}

void BSP_Render(BSP_Node *root, int frustum)
{
}

void BSP_CollideSphere(BSP_Node *root, Vector *center, GLfloat radius)
{
}

void BSP_CollideCylinder(BSP_Node *root, Vector *v0, Vector *v1, GLfloat radius)
{
}

// Here we assume that the lattice mesh forms
// a convex polyhedron.  In its equillibrium
// position this is surely the case, however,
// it wobbles around so it may not nessisarly
// be convex all the time.  It could have some
// concavity.  This routine will not do anything
// to resolve or even detect collisions for the
// given lattice.  What it does is register a
// subset of the set of all collidable polygons
// with the lattice.  These are used in the
// lattice integration subroutine.
void BSP_CollideLattice(BSP_Node *root, Lattice *lattice)
{
}

// endof BSP_Tree.cpp
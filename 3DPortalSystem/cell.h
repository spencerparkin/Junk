// cell.h

// Programmed by Spencer T. Parkin

#ifndef __cell_h__
#define __cell_h__

#include "fovi.h"
#include "surface.h"
#include "entity.h"
#include "artifact.h"

// This is the maximum number of
// entities that can concurrently
// occupy a single cell.
#define ENTITIES_PER_CELL	10

// This is the maximum number of
// surfaces a cell may have.
#define SURFACES_PER_CELL	20

// This is the maximum number of
// artifacts the cell may have.
#define ARTIFACTS_PER_CELL	10

// A cell is a node of the scene graph.
// It is a collection of surfaces forming
// a convex polygon.  Portal surfaces are
// the edges of the scene graph.  Entities
// may occupy, enter, and leave the cells.
class Cell
{
	public:

	Cell();
	~Cell();

	void Draw(fovi *f);				// Draw the contents of this cell.
	bool IsEntityVisible(fovi *f, Entity *e);	// Determine entity visibility.
	bool HandleCollisions(Entity *e);	// Handle collisions.
	bool HandlePropagations(Entity *e, line *m);	// Handle propagations.
	bool EnterEntity(Entity *e);	// Insert the given entity in this cell.
	bool ExitEntity(Entity *e);		// Remove the given entity from this cell.
	bool InsertSurface(Surface *s);
	bool RemoveSurface(Surface *s);
	bool InsertArtifact(Artifact *a);
	bool RemoveArtifact(Artifact *a);
	void CalcFloorCeilTexCoords(void);
	void GtoL(float *x_dst, float *y_dst, float x_src, float y_src);
	void LtoG(float *x_dst, float *y_dst, float x_src, float y_src);
	void CalcPosAngle(class Portal *p);
	class Portal *ReturnPortal(Cell *adj_c);
	void DrawFloorCeiling(fovi *f);

	Surface *s[SURFACES_PER_CELL];	// The surfaces of this cell.
	Entity *e[ENTITIES_PER_CELL];	// The entities in this cell.
	Artifact *a[ARTIFACTS_PER_CELL];	// The artifacts in this cell.
	float x, y;						// The position of this cell.
	float angle;					// The orientation of this cell.
	float tx, ty;					// Texture coordinates for the floor/ceiling.
	bool calc_tex_coords;			// Flag to indicate if we need to calculate tex-cords.
	bool stretch, clip;		// Stretch/clip the floor if true, respectively.
	float x_min, x_max;
	float y_min, y_max;
	int flags;			// Used in depth-first and breadth-first searches.
	struct texture *t_floor, *t_ceil;
	GLuint dpy_list;
	Cell *parent;
};

#define DRAW_FLAG		1
#define COLL_FLAG		2
#define VIS_FLAG		4
#define TETH_FLAG		8

#endif __cell_h__

// endof cell.h
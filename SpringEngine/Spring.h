// Spring.h

#ifndef __Spring_h__
#define __Spring_h__

#include "Vector.h"
#include "Plane.h"
#include "Matrix.h"
#include "Polygon.h"
#include "ConvexHull.h"

#define SN_FLG_STATIONARY		1
#define SN_FLG_EXT_FORCES		2
#define SN_FLG_COLL_PLANE		4
#define SN_FLG_COLL_PGON		8
#define SN_FLG_FRICTION			16
#define SN_FLG_TORQUE			32
#define SN_FLG_CONTACT			64		// Not set by lattice user.  Flag used internally.
#define SN_FLG_GRAVITY			128

// For rigid body dynamics, you might try
// implementing an SPR_FLG_LOCKED flag.  But
// this is non-trivial.  My guess is that
// all nodes strongly connected by locked
// spring edges would have to be recognized
// as a single unit.  After net forces for
// each node are calculated, the components
// of those net forces that are parallel to
// the locked springs would need to be cropped
// out.  Maybe that's not true.  Maybe the
// component of the net force parallel to the
// vector pointing to the COM would need to
// be cropped out.  I'm not sure how integration
// would procede.  The result would have to
// maintain the integrity of the lengths of all
// springs.  One idea is to integrate as normal,
// then try to get all springs within epsilon
// of their desired lengths, but this would be
// costly and may not preserve realism.  You
// might be able to approximate locked springs
// with very-very strong springs, but this hasn't
// worked yet.

struct SpringNode
{
	// Store velocity and acceleration here too?
	Vector *pos, lpos;		// The current and last positions of this node.
	Vector net_force;		// The sum of the forces exherted on this node.	
	Vector *norm;			// Pointer to the vertex lighting normal.
	GLfloat mass;			// The mass of this node.
	GLfloat D;				// A damping factor for this node.
	GLfloat u, v;			// Texture coordinates.
	Plane *friction_plane;	// The plane this node may be sliding on.
	GLfloat u_r;			// Rolling friction coeficient.
	int flags;				// Control flags.
#define MAX_ADJ_PGONS		16
	struct Polygon *pgon[MAX_ADJ_PGONS];	// List of polygons in the mesh sharing this node.
};

// So far there is only one spring control flag
// and it is not implemented.  It probably never
// will be because the idea isn't sound.
#define SPR_FLG_LOCKED		1

struct Spring
{
	GLfloat equib_len;			// The natural length of this spring.
	GLfloat K;					// This spring's spring constant.	
	GLfloat min_len, max_len;	// The minimum and maximum spring length.
	GLfloat len;				// The current length of this spring.
	SpringNode *node[2];		// The nodes connected by this spring.
	Vector color;				// The color of this spring if we want to draw it.
	int flags;					// Spring control flags.
};

#define MAX_EXT_FORCES		32
#define MAX_COLL_PGONS		32
#define MAX_COLL_PLANES		32

struct Lattice
{
	Vector *vertex;							// List of vertices.
	Vector *norm;							// List of vertex normals
	Spring *spring;							// List of springs.
	SpringNode *node;						// List of spring nodes.
	Polygon *pgon;							// List of polygons for rendering.
	Vector *ext_force[MAX_EXT_FORCES];		// Contiguous array of external forces.	
	Polygon *coll_pgon[MAX_COLL_PGONS];		// Contiguous array of collision polygons.
	Plane *coll_plane[MAX_COLL_PLANES];		// Contiguous array of collision planes.
	Vector torque;							// The applied torque.
	int num_springs, num_nodes, num_pgons;	// List counts.
};

void DrawLatticeSprings(Lattice *lattice);
void DrawLatticeNodes(Lattice *lattice);
void DrawLatticePolygons(Lattice *lattice, int flags);
void DrawLatticeFaceNorms(Lattice *lattice);
void DrawLatticeVertexNorms(Lattice *lattice);
void CalcLatticeForces(Lattice *lattice);
void IntegrateLatticeNodes(Lattice *lattice, GLfloat dt);
void CalcLatticeNorms(Lattice *lattice);
void CalcLatticeCenter(Lattice *lattice, Vector *center);

// Some misplaced polygon routines.
void SetupPolygon(Polygon *pgon);
int PolygonIntersection(Polygon *pgon, Vector *pt0, Vector *pt1, GLfloat *t);

// add transformation matrix arguments to these.
Lattice *CreateRegularLattice(int width, int length, int depth, GLfloat st_K, GLfloat sh_K, GLfloat bn_K, Matrix *m, int flags);
Lattice *CreateTetrahedronLattice(Matrix *m, int flags, GLfloat K);
Lattice *CreateIcosahedronLattice(Matrix *m, int flags, GLfloat st_K, GLfloat sh_K);
Lattice *CreateDodecahedronLattice(Matrix *m, int flags, GLfloat st_K, GLfloat sh_K, GLfloat bn_K);
Lattice *CreateSoccerBallLattice(Matrix *m, int flags, GLfloat st_K, GLfloat sh_K, GLfloat bn_K);
Lattice *CreateFlag(Matrix *m, int flags);
Lattice *CreateCarLattice(Matrix *m, int flags, GLfloat K);
void DestroyLattice(Lattice *lattice);

int RegisterLatticeCollisionPolygon(Lattice *lattice, Polygon *pgon);
int UnregisterLatticeCollisionPolygon(Lattice *lattice, Polygon *pgon);

int RegisterLatticeCollisionPlane(Lattice *lattice, Plane *plane);
int UnregisterLatticeCollisionPlane(Lattice *lattice, Plane *plane);

int RegisterLatticeExternalForce(Lattice *lattice, Vector *force);
int UnregisterLatticeExternalForce(Lattice *lattice, Vector *force);

#endif __Spring_h__

// endof Spring.h
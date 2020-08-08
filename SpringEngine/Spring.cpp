// Spring.cpp

// BUG: sometimes lattice explode...not sure why.  i guess the
//      spring force just builds and builds in a feed-back loop.
//      or there is a divide by zero singularity or something.
//      the bug fix could just be a tweak of parameters.  it would
//      be a good idea to get a handle on all physics units.

#include <GL/glut.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "Spring.h"
#include "Vector.h"

// Put the given pointer in an empty slot of
// the given array.  If the array is kept
// contiguous, this will always be the slot
// at the end.  Zero is returned if no slot
// is available, otherwise one is returned.
static int RegisterPointer(void **array, int len, void *ptr)
{
	int k;
	for(k = 0; k < len; k++)
	{
		if(!array[k])
		{
			array[k] = ptr;
			return 1;
		}
	}
	return 0;
}

// Remove the given pointer from the given
// array of pointers.  The array is kept
// contiguous.  Zero is returned if the given
// pointer did not exist in the array, otherwise
// zero is returned.
static int UnregisterPointer(void **array, int len, void *ptr)
{
	int k;
	for(k = 0; k < len; k++)
	{
		if(array[k] == ptr)
		{
			for( ; k < len - 1; k++)
			{
				array[k] = array[k + 1];
				if(!array[k])
					return 1;
			}
			array[k] = 0;
			return 1;
		}
	}
	return 0;
}

// Note that this routine is only needed during the
// creation of spring lattices.  It is not meant
// for real-time use.  There wouldn't be any need for
// this routine if we didn't need to store the vertices
// in an array sepparate from the list of spring nodes
// in a lattice.  The reason the vertices are put in
// their own array is so that they can be shipped to the
// convex-hull finder algorithm the way it wants it.
static SpringNode *NodeFromVertex(Lattice *lattice, Vector *vtx)
{
	int k;
	for(k = 0; k < lattice->num_nodes; k++)
		if(lattice->node[k].pos == vtx)
			return &lattice->node[k];
	return 0;
}

void DrawLatticeSprings(Lattice *lattice)
{
	glBegin(GL_LINES);
	Spring *spring;
	int k;
	for(k = 0; k < lattice->num_springs; k++)
	{
		spring = &lattice->spring[k];
		if(spring->flags & SPR_FLG_LOCKED)
			glColor3f(0.f, 0.f, 0.f);
		else
			glColor3fv(&spring->color.x);
		glVertex3fv(&spring->node[0]->pos->x);
		glVertex3fv(&spring->node[1]->pos->x);
	}
	glEnd();
	// draw a coil here?
}

void DrawLatticeNodes(Lattice *lattice)
{
	glBegin(GL_POINTS);
	SpringNode *node;
	int k;
	for(k = 0; k < lattice->num_nodes; k++)
	{
		node = &lattice->node[k];
		if(node->flags & SN_FLG_STATIONARY)
			glColor3f(1.f, 0.f, 0.f);
		else
			glColor3f(0.f, 0.f, 0.f);
		glVertex3fv(&node->pos->x);
	}
	glEnd();
}

void DrawLatticePolygons(Lattice *lattice, int flags)
{
	int k;
	for(k = 0; k < lattice->num_pgons; k++)
		RenderPolygon(&lattice->pgon[k], flags);
}

void DrawLatticeFaceNorms(Lattice *lattice)
{
	glBegin(GL_LINES);
	Polygon *pgon;
	Vector vec;
	int k;
	for(k = 0; k < lattice->num_pgons; k++)
	{
		pgon = &lattice->pgon[k];
		glVertex3fv(&pgon->plane.pt.x);
		VectorScale(&vec, &pgon->plane.norm, 0.2f);
		VectorAdd(&vec, &pgon->plane.pt, &vec);
		glVertex3fv(&vec.x);
	}
	glEnd();
}

void DrawLatticeVertexNorms(Lattice *lattice)
{
	glBegin(GL_LINES);
	SpringNode *node;
	Vector vec;
	int k;
	for(k = 0; k < lattice->num_nodes; k++)
	{
		node = &lattice->node[k];
		glVertex3fv(&node->pos->x);
		VectorScale(&vec, node->norm, 0.2f);
		VectorAdd(&vec, node->pos, &vec);
		glVertex3fv(&vec.x);
	}
	glEnd();
}

void CalcLatticeForces(Lattice *lattice)
{
	// Reset the net forces of all spring
	// nodes to zero.
	Vector force;	
	SpringNode *node;
	int apply_torque = 0;
	int i, j;
	for(i = 0; i < lattice->num_nodes; i++)
	{
		node = &lattice->node[i];
		VectorSet(&node->net_force, 0.f, 0.f, 0.f);

		// Account for the force due to gravity.
		if(node->flags & SN_FLG_GRAVITY)
		{
			VectorScale(&force, &Gravity, node->mass);
			VectorAdd(&node->net_force, &node->net_force, &force);
		}

		// Account for any registered external forces.
		if(node->flags & SN_FLG_EXT_FORCES)
		{
			for(j = 0; j < MAX_EXT_FORCES && lattice->ext_force[j]; j++)			
			{
				VectorAdd(&node->net_force, &node->net_force, lattice->ext_force[j]);
			}
		}

		if(node->flags & SN_FLG_TORQUE)
			apply_torque |= 1;
	}

	// Apply torque to some subset of the set
	// of all nodes on the non-rigid jelloy body,
	// if that subset is non-empty.
	if(apply_torque && !VectorEq(&lattice->torque, &Origin))
	{
		// Find the center of mass.  This is the total moments
		// over the total mass.  In other words, we're just taking
		// a weighted average based on mass.
		Vector com, moment;
		VectorSet(&com, 0.f, 0.f, 0.f);
		GLfloat mass = 0.f;
		SpringNode *node;
		int k;
		for(k = 0; k < lattice->num_nodes; k++)
		{
			node = &lattice->node[k];
			mass += node->mass;
			VectorScale(&moment, node->pos, node->mass);
			VectorAdd(&com, &com, &moment);
		}
		VectorScale(&com, &com, 1.f / mass);

		// Add the contribution of the torque force
		// for each node to the net-force.
		Vector force, vec;
		for(k = 0; k < lattice->num_nodes; k++)
		{
			node = &lattice->node[k];
			if(node->flags & SN_FLG_TORQUE)
			{
				VectorSub(&vec, node->pos, &com);
				VectorCross(&force, &lattice->torque, &vec);
				VectorScale(&force, &force, 1.f / VectorDot(&vec, &vec));
				VectorAdd(&node->net_force, &node->net_force, &force);
			}
		}
	}

	// Accumulate forces on the spring nodes
	// due to each spring in the lattice.
	// These would be the internal forces.
	Spring *spring;
	Vector vec;	
	for(i = 0; i < lattice->num_springs; i++)
	{
		spring = &lattice->spring[i];
		VectorSub(&vec, spring->node[1]->pos, spring->node[0]->pos);
		spring->len = VectorLength(&vec);
		VectorScale(&force, &vec, spring->K * (spring->len - spring->equib_len));
		VectorAdd(&spring->node[0]->net_force, &spring->node[0]->net_force, &force);
		VectorSub(&spring->node[1]->net_force, &spring->node[1]->net_force, &force);		
	}

	// All other forces considered, now lets
	// factor in friction forces applied to
	// the nodes.  We'll need all of the other
	// forces to do this because they contribute
	// to the normal force to the frictin plane.
	GLfloat norm_force;	
	for(i = 0; i < lattice->num_nodes; i++)
	{
		node = &lattice->node[i];
		if(!(node->flags & SN_FLG_FRICTION))
			continue;
		if(!(node->flags & SN_FLG_CONTACT))
			continue;
		
		// Factor in rolling friction forces here.
		// The rolling force is in the direction
		// opposite the direction of motion and is
		// directly proportional to the normal force
		// to the friction plane.
		assert(node->friction_plane != 0);
		norm_force = VectorDot(&node->friction_plane->norm, &node->net_force);
		if(norm_force > 0.f)
			continue;

		VectorSub(&force, &node->lpos, node->pos);
		VectorNormalize(&force, &force);
		VectorScale(&force, &force, node->u_r * -norm_force);
		VectorAdd(&node->net_force, &node->net_force, &force);
	}
}

// Use Verlet integration to move the lattice
// node points in response to the forces applied to them.
void IntegrateLatticeNodes(Lattice *lattice, GLfloat dt)
{
	// Integrate.
	SpringNode *node;
	Plane *plane;
	Vector tmp, vec;
	int i, j;
	for(i = 0; i < lattice->num_nodes; i++)
	{
		node = &lattice->node[i];
		node->flags &= ~SN_FLG_CONTACT;
		if(node->flags & SN_FLG_STATIONARY)
			continue;
		
		// Integrate with damping factor.
		VectorCopy(&tmp, node->pos);
		VectorScale(node->pos, node->pos, 2.f - node->D);
		VectorScale(&vec, &node->lpos, 1.f - node->D);
		VectorSub(node->pos, node->pos, &vec);
		VectorScale(&vec, &node->net_force, (dt * dt) / node->mass);
		VectorAdd(node->pos, node->pos, &vec);
		VectorCopy(&node->lpos, &tmp);

		// Handle collisions with registered collision planes.
		if(node->flags & SN_FLG_COLL_PLANE)
		{
			for(j = 0; j < MAX_COLL_PLANES && lattice->coll_plane[j]; j++)
			{
				plane = lattice->coll_plane[j];
				if(SideOfPlane(plane, node->pos) != ON_PLANE_BACK)
					continue;								
				PushToPlaneSurface(plane, node->pos, node->pos);
				node->flags |= SN_FLG_CONTACT;
				node->friction_plane = plane;
			}
		}
	}

	// Handle collisions with registered collision polygons.
	int do_spring_locks = 0;
	Spring *spring;
	Polygon *pgon;
	SpringNode *n0, *n1;
	GLfloat t;	
	for(i = 0; i < lattice->num_springs; i++)
	{
		spring = &lattice->spring[i];
		if(spring->flags & SPR_FLG_LOCKED)
			do_spring_locks |= 1;
		n0 = spring->node[0];
		n1 = spring->node[1];
		if((n0->flags & SN_FLG_COLL_PGON) || (n1->flags & SN_FLG_COLL_PGON))
		{
			for(j = 0; j < MAX_COLL_PGONS && lattice->coll_pgon[j]; j++)
			{
				pgon = lattice->coll_pgon[j];
				if(LinePolygonIntersection(pgon, n0->pos, n1->pos, &t))
				{
					// Push the offending points to the surface of the plane.
					if(SideOfPlane(&pgon->plane, n0->pos) == ON_PLANE_BACK)
					{
						PushToPlaneSurface(&pgon->plane, &tmp, n0->pos);
						VectorSub(&vec, &tmp, n0->pos);
						if(VectorDot(&vec, &vec) < 0.05f)	// Reject bogus corrections.  (Not a clean solution.)
						{
							VectorCopy(n0->pos, &tmp);
							n0->flags |= SN_FLG_CONTACT;
							n0->friction_plane = &pgon->plane;
						}
					}
					else if(SideOfPlane(&pgon->plane, n1->pos) == ON_PLANE_BACK)
					{
						PushToPlaneSurface(&pgon->plane, &tmp, n1->pos);
						VectorSub(&vec, &tmp, n1->pos);
						if(VectorDot(&vec, &vec) < 0.05f)	// Reject bogus corrections.  (Not a clean solution.)
						{
							VectorCopy(n1->pos, &tmp);
							n1->flags |= SN_FLG_CONTACT;
							n1->friction_plane = &pgon->plane;
						}
					}
				}
			}
		}
	}

	// Make sure each spring is within its exceptable bounds of length.
	// We assume here that the spring length has been computed by the
	// CalcLatticeForces() routine.
#if 0			// We might get perpetual movement with this anyway.
	GLfloat scale;
	for(i = 0; i < lattice->num_springs; i++)
	{
		spring = &lattice->spring[i];
		if(spring->len < spring->min_len)
			scale = spring->min_len / spring->len;
		else if(spring->len > spring->max_len)
			scale = spring->max_len / spring->len;
		else
			continue;
		n0 = spring->node[0];
		n1 = spring->node[1];
		if((n0->flags & SN_FLG_CONTACT) && (n1->flags & SN_FLG_CONTACT))
			continue;		// For now, wimp-out on this case.
		else if(n0->flags & SN_FLG_CONTACT)
		{
			VectorSub(&vec, n1->pos, n0->pos);
			VectorScale(&vec, &vec, scale);
			VectorAdd(n1->pos, n0->pos, &vec);			
		}
		else if(n1->flags & SN_FLG_CONTACT)
		{
			VectorSub(&vec, n0->pos, n1->pos);
			VectorScale(&vec, &vec, scale);
			VectorAdd(n0->pos, n1->pos, &vec);
		}
		else
		{
			VectorSub(&vec, n1->pos, n0->pos);
			VectorScale(&vec, &vec, scale * 0.5f);
			VectorCopy(&tmp, n0->pos);
			VectorSub(n0->pos, n1->pos, &vec);
			VectorAdd(n1->pos, &tmp, &vec);
		}		
	}
#endif

	// Process locked springs.  Be mindful of
	// nodes having made contact with any collision planes.
	// This is expensive because it is an interative
	// process.  There is also the potential here of
	// infinite loopsing.  Note that this is a very inneficient
	// way to try and simulate rigid body dynamics.  In fact,
	// it's out-right crazy-slow.  So this feature will probably
	// not be used much, if at all.  Since this is not based
	// on physics principles, the affects of this do not
	// always preserve realism.
	int bail_out = 0;
	while(do_spring_locks && bail_out++ < 10)
	{	
		GLfloat len;

		// Assume this will be the last iteration.
		do_spring_locks = 0;

		for(i = 0; i < lattice->num_springs; i++)
		{
			spring = &lattice->spring[i];
			if(!(spring->flags & SPR_FLG_LOCKED))
				continue;
			VectorSub(&vec, spring->node[1]->pos, spring->node[0]->pos);
			len = VectorLength(&vec);
			len -= spring->equib_len;
			if(fabs(len) >= EPS)
			{
				// If any one spring needs to be adjusted, then
				// we have to redo everything in case this adjustment
				// unadjusted another lock spring.
				do_spring_locks |= 1;			
				VectorScale(&vec, &vec, len * 0.5f);
				VectorAdd(spring->node[0]->pos, spring->node[0]->pos, &vec);
				VectorSub(spring->node[1]->pos, spring->node[1]->pos, &vec);
			}
		}
	}

	// What should really be done with locked springs is
	// to calculate and accumulate a constraint force for
	// each node particle.  This is not trivial.
}

// Use the Newel method to compute the
// surface normals of each polgon in the lattice.
// Since the lattice can wobble like jello,
// sometimes the polygon faces will shear and so
// not all vertices of each face will remain coplanar.
// This is where the Newel method helps us out.
void CalcLatticeNorms(Lattice *lattice)
{
	Polygon *pgon;	
	int i, j;
	for(i = 0; i < lattice->num_pgons; i++)
	{
		pgon = &lattice->pgon[i];
		CalcPolygonPlane(pgon);
	}

	SpringNode *node;
	for(i = 0; i < lattice->num_nodes; i++)
	{
		node = &lattice->node[i];
		VectorSet(node->norm, 0.f, 0.f, 0.f);
		for(j = 0; j < MAX_ADJ_PGONS && node->pgon[j]; j++)		
			VectorAdd(node->norm, node->norm, &node->pgon[j]->plane.norm);
		VectorNormalize(node->norm, node->norm);
	}
}

// Calculate the geometric center of the lattice.
// This will not necessarily be the center of mass.
void CalcLatticeCenter(Lattice *lattice, Vector *center)
{
	VectorSet(center, 0.f, 0.f, 0.f);
	int k;
	for(k = 0; k < lattice->num_nodes; k++)
	{
		VectorAdd(center, center, &lattice->vertex[k]);
	}
	VectorScale(center, center, 1.f / GLfloat(lattice->num_nodes));
}

// This routine is called by lattice creation
// routines to make each node aware of the
// polygons using it as a vertex.  This is for
// convenience and efficiency.
static void DetermineAdjacentPolygonsForNodes(Lattice *lattice)
{
	Polygon *pgon;
	SpringNode *node;
	int i, j, k;
	for(i = 0; i < lattice->num_nodes; i++)
	{
		node = &lattice->node[i];
		for(j = 0; j < lattice->num_pgons; j++)
		{
			pgon = &lattice->pgon[j];
			for(k = 0; k < pgon->num_vertices; k++)
				if(node->pos == pgon->vtx[k])
					break;
			if(k < pgon->num_vertices)
				RegisterPointer((void **)node->pgon, MAX_ADJ_PGONS, pgon);
		}
	}
}

// This routine is used by lattice creation routines
// to skin the lattices.  It is assumed that the skin
// forms a convex polyhedron.
void CreateLatticeMesh(Lattice *lattice, float diolate)
{
	int i, j, k;

	// The purpose of dialating the mesh in preparation
	// for finding the convex hull is so that the resulting
	// hull will have a higher level of geometric detail
	// that would normally be found in the initial state
	// of the lattice.
	static Vector *vtx_buf = 0;
	if(diolate > 0.f)
	{
		// Save the original vertex data.
		vtx_buf = new Vector[lattice->num_nodes];
		memcpy((void *)vtx_buf, (void *)lattice->vertex, sizeof(Vector) * lattice->num_nodes);
		// Find the geometric center.
		Vector center, vec;
		GLfloat max_dist, dist;		
		VectorSet(&center, 0.f, 0.f, 0.f);		
		for(i = 0; i < lattice->num_nodes; i++)		
			VectorAdd(&center, &center, &lattice->vertex[i]);
		VectorScale(&center, &center, 1.f / GLfloat(lattice->num_nodes));
		// Find the radius of the smallest enclosing sphere of the lattice
		// where the sphere's center is the geometric center of the lattice.
		max_dist = 0.f;
		for(i = 0; i < lattice->num_nodes; i++)
		{
			dist = VectorDistance(&center, &lattice->vertex[i]);
			if(dist > max_dist)
				max_dist = dist;
		}
		// Diolate vertices that don't touch the sphere by
		// the given diolation percentage.  Scale each vertex
		// away from the geometric center.
		for(i = 0; i < lattice->num_nodes; i++)
		{
			VectorSub(&vec, &lattice->vertex[i], &center);
			dist = VectorLength(&vec);
			if(dist < max_dist)
			{
				VectorRescale(&vec, &vec, dist + (max_dist - dist) * diolate);
				VectorAdd(&lattice->vertex[i], &center, &vec);
			}
		}
	}

	// Use convex hull finder algorithm to generate polygon-mesh.
	// in other lattice creation routines, the results of the
	// convex hull algorithm may be used to find spring positions
	// from the edges found.
	pt_cloud = lattice->vertex;
	num_cloud_pts = lattice->num_nodes;
	Find3DConvexHull();
	lattice->num_pgons = PolygonList.item_count;
	lattice->pgon = new Polygon[lattice->num_pgons];
	Polygon *pgon;
	i = 0;
	for(pgon = (Polygon *)PolygonList.head; pgon; pgon = (Polygon *)pgon->next)
	{
		memset((void *)&lattice->pgon[i], 0, sizeof(Polygon));
		memcpy((void *)lattice->pgon[i].vtx, (void *)pgon->vtx, sizeof(Vector *) * pgon->num_vertices);
		lattice->pgon[i].num_vertices = pgon->num_vertices;
		for(j = 0; j < pgon->num_vertices; j++)
		{
			k = pgon->vtx[j] - lattice->vertex;
			lattice->pgon[i].norm[j] = &lattice->norm[k];			
		}
		i++;
	}

	// Restore the original vertices of the lattice.
	if(diolate > 0.f)
	{
		memcpy((void *)lattice->vertex, (void *)vtx_buf, sizeof(Vector) * lattice->num_nodes);
		delete [] vtx_buf;
	}
}

// This routine is used by the CreateRegularLattice() routine and other lattice creation routines.
static Spring *AddSpring(Lattice *lattice, Spring *spring, SpringNode *n0, SpringNode *n1, GLfloat K, GLfloat r, GLfloat g, GLfloat b)
{
	// Don't add the spring if there's already one in place.
	int k;
	for(k = 0; k < lattice->num_springs && k < spring - lattice->spring; k++)
	{		
		if(lattice->spring[k].node[0] == n0 && lattice->spring[k].node[1] == n1)
			return spring;
		if(lattice->spring[k].node[0] == n1 && lattice->spring[k].node[1] == n0)
			return spring;
	}

	// Add the spring.
	assert(spring - lattice->spring < lattice->num_springs);
	spring->K = K;
	VectorSet(&spring->color, r, g, b);
	spring->node[0] = n0;
	spring->node[1] = n1;
	spring->equib_len = VectorDistance(n0->pos, n1->pos);
	spring->min_len = spring->equib_len / 2.f; //0.0001f;      tmp
	spring->max_len = spring->equib_len * 1.5f;
	return ++spring;
}

// This macro is used exclusively by the CreateRegularLattice() routine.
#define SPR_MAT(i, j, k)	(lattice->node[((i) * length * depth + (j) * depth + (k))])

// Construct a regular array of spring nodes connected by springs.
// Constructing this lattice is a real pain in the butt as I
// suspect the construction of most lattices will be.
Lattice *CreateRegularLattice(int width, int length, int depth,
							GLfloat st_K, GLfloat sh_K, GLfloat bn_K,
							Matrix *m, int flags)
{
	// Barf if more than one of the dimensions is less than 2.
	if((width < 2) + (length < 2) + (depth < 2) > 1)
		return 0;
	// If one of the dimensions is 1, then let it be the depth.
	if(width == 1)
		_SWAP(width, depth);
	else if(length == 1)
		_SWAP(length, depth);

	Lattice *lattice = new Lattice;
	memset((void *)lattice, 0, sizeof(Lattice));

	lattice->num_nodes = width * length * depth;
	lattice->node = new SpringNode[lattice->num_nodes];	
	lattice->vertex = new Vector[lattice->num_nodes];
	lattice->norm = new Vector[lattice->num_nodes];
	memset((void *)lattice->node, 0, sizeof(SpringNode) * lattice->num_nodes);

	// Initialize the nodes.
	SpringNode *node;
	Vector *vtx = lattice->vertex;
	Vector *norm = lattice->norm;
	int i, j, k;
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < length; j++)
		{
			for(k = 0; k < depth; k++)
			{
				node = &SPR_MAT(i, j, k);
				node->pos = vtx++;
				node->norm = norm++;
				VectorSet(node->pos, GLfloat(i), GLfloat(j), GLfloat(k), 1.f);
				MatrixTransform(m, node->pos);
				VectorCopy(&node->lpos, node->pos);
				node->mass = 1.f;	//for now
				node->D = 0.0025f;	//for now
				node->u_r = 1.f;	//for now
				node->flags = flags;				
				
				// calc texture uvs here.
			}
		}
	}

	// Skin the lattice.
	CreateLatticeMesh(lattice, 0.1f);

	// So that vertex lighting will work.
	DetermineAdjacentPolygonsForNodes(lattice);

	// Count how many springs we'll need.  This is
	// a problem in combinatorics.
	lattice->num_springs = 0;
	if(st_K != 0.f)
		lattice->num_springs += (width - 1) * length * depth +
								width * (length - 1) * depth +
								width * length * (depth - 1);
	if(sh_K != 0.f)
		lattice->num_springs += (width > 1 && depth > 1 && length > 1) ?
								(4 * (width - 1) * (length - 1) * (depth - 1)) : (
								(width > 1 && length > 1) ?
								(2 * (width - 1) * (length - 1)) : (
								(length > 1 && depth > 1) ?
								(2 * (length - 1) * (depth - 1)) : (
								(width > 1 && depth > 1) ?
								(2 * (width - 1) * (depth - 1)) : 0)));
	if(bn_K != 0.f)
		lattice->num_springs += (width > 2 && length > 2 && depth > 2) ?	// This is a mess, but it works.
								((width - 2) * length * depth +
								width * (length - 2) * depth +
								width * length * (depth - 2)) : (
								(width > 2 && length > 2) ?
								(depth * ((width - 2) * length +
											width * (length - 2))) : (
								(length > 2 && depth > 2) ?
								(width * ((length - 2) * depth +
											length * (depth - 2))) : (
								(width > 2 && depth > 2) ?
								(length * ((width - 2) * depth +
											width * (depth - 2))) : (
								(width > 2) ?
								((width - 2) * length * depth) : (
								(length > 2) ?
								(width * (length - 2) * depth) : (
								(depth > 2) ?
								(width * length * (depth - 2)) : 0))))));

	lattice->spring = new Spring[lattice->num_springs];
	memset((void *)lattice->spring, 0, sizeof(Spring) * lattice->num_springs);
	Spring *spring = lattice->spring;

	// Construct the structural springs.
	if(st_K != 0.f)
	{
		for(i = 0; i < width; i++)
		{
			for(j = 0; j < length; j++)
			{
				for(k = 0; k < depth; k++)
				{
					if(i > 0)					
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i - 1, j, k), st_K, 1.f, 0.f, 0.f);
					if(i < width - 1)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i + 1, j, k), st_K, 1.f, 0.f, 0.f);
					if(j > 0)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j - 1, k), st_K, 1.f, 0.f, 0.f);
					if(j < length - 1)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j + 1, k), st_K, 1.f, 0.f, 0.f);
					if(k > 0)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j, k - 1), st_K, 1.f, 0.f, 0.f);
					if(k < depth - 1)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j, k + 1), st_K, 1.f, 0.f, 0.f);					
				}
			}
		}
	}

	// Construct the shearing springs.
	if(sh_K != 0.f)
	{
		for(i = 0; i < width - 1; i++)
		{
			for(j = 0; j < length - 1; j++)
			{
				if(depth > 1)
				{
					for(k = 0; k < depth - 1; k++)
					{
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i + 1, j + 1, k + 1), sh_K, 0.f, 1.f, 0.f);
						spring = AddSpring(lattice, spring, &SPR_MAT(i + 1, j, k), &SPR_MAT(i, j + 1, k + 1), sh_K, 0.f, 1.f, 0.f);
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j + 1, k), &SPR_MAT(i + 1, j, k + 1), sh_K, 0.f, 1.f, 0.f);
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k + 1), &SPR_MAT(i + 1, j + 1, k), sh_K, 0.f, 1.f, 0.f);						
					}
				}
				else
				{
					spring = AddSpring(lattice, spring, &SPR_MAT(i, j, 0), &SPR_MAT(i + 1, j + 1, 0), sh_K, 0.f, 1.f, 0.f);
					spring = AddSpring(lattice, spring, &SPR_MAT(i + 1, j, 0), &SPR_MAT(i, j + 1, 0), sh_K, 0.f, 1.f, 0.f);					
				}
			}
		}
	}

	// Construct the bending springs.
	if(bn_K != 0.f)
	{
		for(i = 0; i < width; i++)
		{
			for(j = 0; j < length; j++)
			{
				for(k = 0; k < depth; k++)
				{
					if(i > 1)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i - 2, j, k), bn_K, 0.f, 0.f, 1.f);
					if(i < width - 2)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i + 2, j, k), bn_K, 0.f, 0.f, 1.f);
					if(j > 1)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j - 2, k), bn_K, 0.f, 0.f, 1.f);
					if(j < length - 2)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j + 2, k), bn_K, 0.f, 0.f, 1.f);
					if(k > 1)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j, k - 2), bn_K, 0.f, 0.f, 1.f);
					if(k < depth - 2)
						spring = AddSpring(lattice, spring, &SPR_MAT(i, j, k), &SPR_MAT(i, j, k + 2), bn_K, 0.f, 0.f, 1.f);					
				}
			}
		}
	}

	// Assert that we used all the spring memory.
	assert(spring - lattice->spring == lattice->num_springs);

	CalcLatticeNorms(lattice);
	return lattice;
}

// This routine is used to easily generate the vertices
// of common polyhedrah.
static Vector *SetVertexCombos(Vector *vtx, GLfloat x, GLfloat y, GLfloat z)
{	
	GLfloat sx, sy, sz;
	int k;
	for(k = 0; k < 8; k++)
	{
		if((k & 1) && (x == 0.f)) continue;
		if((k & 2) && (y == 0.f)) continue;
		if((k & 4) && (z == 0.f)) continue;
		sx = (k & 1) ? -1.f : 1.f;
		sy = (k & 2) ? -1.f : 1.f;
		sz = (k & 4) ? -1.f : 1.f;		
		VectorSet(vtx++, x * sx, y * sy, z * sz);
	}
	return vtx;
}

// Assuming that the vertex array of the given lattice
// has been initialized, load the spring-node array
// with the default parameters.
static void PopulateDefaultNodeData(Lattice *lattice, Matrix *m, int flags)
{
	SpringNode *node;
	int k;
	for(k = 0; k < lattice->num_nodes; k++)
	{
		node = &lattice->node[k];
		node->pos = &lattice->vertex[k];		
		node->norm = &lattice->norm[k];
		MatrixTransform(m, node->pos);
		VectorCopy(&node->lpos, node->pos);
		node->mass = 1.f;
		node->D = 0.0025f;
		node->u_r = 1.f;
		node->flags = flags;
	}
}

// Use the results of the convex hull finder
// algorithm to generate structural springs
// for a convex polyhedron lattice structure thingy.
static Spring *CreateSpringsFromEdgeList(Lattice *lattice, List *e_list, Spring *spring, GLfloat K, GLfloat r, GLfloat g, GLfloat b)
{
	Edge *edge;
	SpringNode *n0, *n1;
	for(edge = (Edge *)e_list->head; edge; edge = (Edge *)edge->next)
	{
		n0 = NodeFromVertex(lattice, edge->vtx[0]);
		n1 = NodeFromVertex(lattice, edge->vtx[1]);
		assert(n0 && n1);
		spring = AddSpring(lattice, spring, n0, n1, K, r, g, b);
	}
	return spring;
}

static Spring *CreateBendySpringsForPolyhedronLattice(Lattice *lattice, Spring *spring, GLfloat K, GLfloat r, GLfloat g, GLfloat b)
{
	SpringNode *n0, *n1;
	Polygon *pgon;
	int i, j, k;
	for(i = 0; i < lattice->num_pgons; i++)
	{
		pgon = &lattice->pgon[i];
		for(j = 0; j < pgon->num_vertices; j++)
		{
			k = (j + 2) % pgon->num_vertices;
			n0 = NodeFromVertex(lattice, pgon->vtx[j]);
			n1 = NodeFromVertex(lattice, pgon->vtx[k]);
			assert(n0 && n1);
			spring = AddSpring(lattice, spring, n0, n1, K, r, g, b);
		}
	}
	return spring;
}

static Spring *CreateShearSpringsForPolyhedronLattice(Lattice *lattice, Spring *spring, GLfloat K, GLfloat r, GLfloat g, GLfloat b)
{
	SpringNode *n0, *n1;
	Vector vec;
	int i, j;
	for(i = 0; i < lattice->num_nodes; i++)
	{
		n0 = NodeFromVertex(lattice, &lattice->vertex[i]);
		assert(n0);
		VectorScale(&vec, &lattice->vertex[i], -1.f);
		for(j = i + 1; j < lattice->num_nodes; j++)
		{
			if(VectorEq(&vec, &lattice->vertex[j]))
			{
				n1 = NodeFromVertex(lattice, &lattice->vertex[j]);
				assert(n1);
				spring = AddSpring(lattice, spring, n0, n1, K, r, g, b);
				break;
			}
		}
	}
	return spring;
}

Lattice *CreateTetrahedronLattice(Matrix *m, int flags, GLfloat K)
{
	Lattice *lattice = new Lattice;
	memset((void *)lattice, 0, sizeof(Lattice));

	lattice->num_nodes = 4;
	lattice->node = new SpringNode[lattice->num_nodes];
	lattice->vertex = new Vector[lattice->num_nodes];
	lattice->norm = new Vector[lattice->num_nodes];
	memset((void *)lattice->node, 0, sizeof(SpringNode) * lattice->num_nodes);

	VectorSet(&lattice->vertex[0], -1.f, -0.5f, 0.f, 1.f);
	VectorSet(&lattice->vertex[1], 1.f, -0.5f, 0.f, 1.f);
	VectorSet(&lattice->vertex[2], 0.f, 0.5f, -1.f, 1.f);
	VectorSet(&lattice->vertex[3], 0.f, 0.5f, 1.f, 1.f);

	PopulateDefaultNodeData(lattice, m, flags);
	CreateLatticeMesh(lattice, 0.f);
	DetermineAdjacentPolygonsForNodes(lattice);

	lattice->num_springs = 6;
	lattice->spring = new Spring[lattice->num_springs];
	memset((void *)lattice->spring, 0, sizeof(Spring) * lattice->num_springs);
	assert(FinalEdgeList.item_count == 6);
	CreateSpringsFromEdgeList(lattice, &FinalEdgeList, lattice->spring, K, 1.f, 0.f, 0.f);

	CalcLatticeNorms(lattice);
	return lattice;
}

#define PHI		1.61803398875f

Lattice *CreateIcosahedronLattice(Matrix *m, int flags, GLfloat st_K, GLfloat sh_K)
{
	Lattice *lattice = new Lattice;
	memset((void *)lattice, 0, sizeof(Lattice));

	lattice->num_nodes = 12;
	lattice->node = new SpringNode[lattice->num_nodes];
	lattice->vertex = new Vector[lattice->num_nodes];
	lattice->norm = new Vector[lattice->num_nodes];
	memset((void *)lattice->node, 0, sizeof(SpringNode) * lattice->num_nodes);

	Vector *vtx = lattice->vertex;
	vtx = SetVertexCombos(vtx, 0.f, 1.f, PHI);
	vtx = SetVertexCombos(vtx, 1.f, PHI, 0.f);
	vtx = SetVertexCombos(vtx, PHI, 0.f, 1.f);	

	PopulateDefaultNodeData(lattice, m, flags);
	CreateLatticeMesh(lattice, 0.f);
	DetermineAdjacentPolygonsForNodes(lattice);

	lattice->num_springs = 0.f;
	if(st_K > 0.f)
		lattice->num_springs += FinalEdgeList.item_count;
	if(sh_K > 0.f)
		lattice->num_springs += 2 * 3;

	lattice->spring = new Spring[lattice->num_springs];
	memset((void *)lattice->spring, 0, sizeof(Spring) * lattice->num_springs);
	Spring *spring = lattice->spring;
	if(st_K > 0.f)
		spring = CreateSpringsFromEdgeList(lattice, &FinalEdgeList, spring, st_K, 1.f, 0.f, 0.f);
	if(sh_K > 0.f)
		spring = CreateShearSpringsForPolyhedronLattice(lattice, spring, sh_K, 0.f, 1.f, 0.f);

	assert(spring - lattice->spring == lattice->num_springs);
	CalcLatticeNorms(lattice);
	return lattice;
}

// It does bother me that some of the code here
// identical to that in the Icosahedron routine,
// but there are some important differences, obviously.
Lattice *CreateDodecahedronLattice(Matrix *m, int flags, GLfloat st_K, GLfloat sh_K, GLfloat bn_K)
{
	Lattice *lattice = new Lattice;
	memset((void *)lattice, 0, sizeof(Lattice));

	lattice->num_nodes = 20;
	lattice->node = new SpringNode[lattice->num_nodes];
	lattice->vertex = new Vector[lattice->num_nodes];
	lattice->norm = new Vector[lattice->num_nodes];
	memset((void *)lattice->node, 0, sizeof(SpringNode) * lattice->num_nodes);

	Vector *vtx = lattice->vertex;
	vtx = SetVertexCombos(vtx, 1.f, 1.f, 1.f);
	vtx = SetVertexCombos(vtx, 0.f, 1.f / PHI, PHI);
	vtx = SetVertexCombos(vtx, 1.f / PHI, PHI, 0.f);
	vtx = SetVertexCombos(vtx, PHI, 0.f, 1.f / PHI);	

	PopulateDefaultNodeData(lattice, m, flags);
	CreateLatticeMesh(lattice, 0.f);
	DetermineAdjacentPolygonsForNodes(lattice);

	lattice->num_springs = 0.f;
	if(st_K > 0.f)
		lattice->num_springs += FinalEdgeList.item_count;
	if(sh_K > 0.f)
		lattice->num_springs += 10;
	if(bn_K > 0.f)
		lattice->num_springs += 60;

	lattice->spring = new Spring[lattice->num_springs];
	memset((void *)lattice->spring, 0, sizeof(Spring) * lattice->num_springs);
	Spring *spring = lattice->spring;
	if(st_K > 0.f)
		spring = CreateSpringsFromEdgeList(lattice, &FinalEdgeList, spring, st_K, 1.f, 0.f, 0.f);
	if(sh_K > 0.f)
		spring = CreateShearSpringsForPolyhedronLattice(lattice, spring, sh_K, 0.f, 1.f, 0.f);
	if(bn_K > 0.f)
		spring = CreateBendySpringsForPolyhedronLattice(lattice, spring, bn_K, 0.f, 0.f, 1.f);

	assert(spring - lattice->spring == lattice->num_springs);
	CalcLatticeNorms(lattice);
	return lattice;
}

Lattice *CreateSoccerBallLattice(Matrix *m, int flags, GLfloat st_K, GLfloat sh_K, GLfloat bn_K)
{
	Lattice *lattice = new Lattice;
	memset((void *)lattice, 0, sizeof(Lattice));

	lattice->num_nodes = 60;
	lattice->node = new SpringNode[lattice->num_nodes];
	lattice->vertex = new Vector[lattice->num_nodes];
	lattice->norm = new Vector[lattice->num_nodes];
	memset((void *)lattice->node, 0, sizeof(SpringNode) * lattice->num_nodes);

	Vector *vtx = lattice->vertex;
	vtx = SetVertexCombos(vtx, 0.f, 1.f, 3.f * PHI);
	vtx = SetVertexCombos(vtx, 1.f, 3.f * PHI, 0.f);
	vtx = SetVertexCombos(vtx, 3.f * PHI, 0.f, 1.f);
	vtx = SetVertexCombos(vtx, 2.f, 1.f + 2.f * PHI, PHI);
	vtx = SetVertexCombos(vtx, 1.f + 2.f * PHI, PHI, 2.f);
	vtx = SetVertexCombos(vtx, PHI, 2.f, 1.f + 2.f * PHI);
	vtx = SetVertexCombos(vtx, 1.f, 2.f + PHI, 2.f * PHI);
	vtx = SetVertexCombos(vtx, 2.f + PHI, 2.f * PHI, 1.f);
	vtx = SetVertexCombos(vtx, 2.f * PHI, 1.f, 2.f + PHI);

	PopulateDefaultNodeData(lattice, m, flags);
	CreateLatticeMesh(lattice, 0.f);
	DetermineAdjacentPolygonsForNodes(lattice);

	lattice->num_springs = 0.f;
	if(st_K > 0.f)
		lattice->num_springs += FinalEdgeList.item_count;
	if(sh_K > 0.f)
		lattice->num_springs += 2 * 3 + 4 * 6;
	if(bn_K > 0.f)
		lattice->num_springs += 12 * 5 + (32 - 12) * 6;

	lattice->spring = new Spring[lattice->num_springs];
	memset((void *)lattice->spring, 0, sizeof(Spring) * lattice->num_springs);
	Spring *spring = lattice->spring;

	if(st_K > 0.f)
		spring = CreateSpringsFromEdgeList(lattice, &FinalEdgeList, spring, st_K, 1.f, 0.f, 0.f);
	if(sh_K > 0.f)
		spring = CreateShearSpringsForPolyhedronLattice(lattice, spring, sh_K, 0.f, 1.f, 0.f);
	if(bn_K > 0.f)
		spring = CreateBendySpringsForPolyhedronLattice(lattice, spring, bn_K, 0.f, 0.f, 1.f);

	assert(spring - lattice->spring == lattice->num_springs);
	CalcLatticeNorms(lattice);
	return lattice;
}

Lattice *CreateFlag(Matrix *m, int flags)
{
	// only structural springs needed of regular lattice.
	return 0;
}

// The idea behind this lattice is to totally fake
// rigid-body-dynamics.  The position and orientation
// of the car will be taken from the general position
// and orientation of the car lattice.  The position
// of the car's shock obsorbers will also be derived
// from the lattice state.
// This idea will never work!
Lattice *CreateCarLattice(Matrix *m, int flags, GLfloat K)
{
	Lattice *lattice = new Lattice;
	memset((void *)lattice, 0, sizeof(Lattice));

	lattice->num_nodes = 9;
	lattice->node = new SpringNode[lattice->num_nodes];
	lattice->vertex = new Vector[lattice->num_nodes];
	lattice->norm = new Vector[lattice->num_nodes];
	memset((void *)lattice->node, 0, sizeof(SpringNode) * lattice->num_nodes);

	Vector *vtx = lattice->vertex;
	VectorSet(vtx++, 0.f, 1.f, 0.f);
	VectorSet(vtx++, -0.5f, 0.f, -0.5f);
	VectorSet(vtx++, 0.5f, 0.f, -0.5f);
	VectorSet(vtx++, 0.5f, 0.f, 0.5f);
	VectorSet(vtx++, -0.5f, 0.f, 0.5f);
	VectorSet(vtx++, -1.f, -0.5f, -1.f);
	VectorSet(vtx++, 1.f, -0.5f, -1.f);
	VectorSet(vtx++, 1.f, -0.5f, 1.f);
	VectorSet(vtx++, -1.f, -0.5f, 1.f);

	PopulateDefaultNodeData(lattice, m, flags);
	CreateLatticeMesh(lattice, 0.f);
	DetermineAdjacentPolygonsForNodes(lattice);

	lattice->num_springs = 10 + 3 * 4;
	lattice->spring = new Spring[lattice->num_springs];

	SpringNode *node = lattice->node;
	Spring *spring = lattice->spring;
	spring = AddSpring(lattice, spring, &node[0], &node[1], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[0], &node[2], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[0], &node[3], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[0], &node[4], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[1], &node[2], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[2], &node[3], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[3], &node[4], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[4], &node[1], K, 1.f, 0.f, 0.f);
	spring = AddSpring(lattice, spring, &node[1], &node[3], K, 0.f, 1.f, 0.f);
	spring = AddSpring(lattice, spring, &node[2], &node[4], K, 0.f, 1.f, 0.f);

	spring = AddSpring(lattice, spring, &node[0], &node[5], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[0], &node[6], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[0], &node[7], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[0], &node[8], K, 0.f, 0.f, 1.f);

	spring = AddSpring(lattice, spring, &node[1], &node[5], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[2], &node[6], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[3], &node[7], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[4], &node[8], K, 0.f, 0.f, 1.f);

	spring = AddSpring(lattice, spring, &node[4], &node[5], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[3], &node[6], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[1], &node[8], K, 0.f, 0.f, 1.f);
	spring = AddSpring(lattice, spring, &node[2], &node[7], K, 0.f, 0.f, 1.f);

	assert(spring - lattice->spring == lattice->num_springs);
	CalcLatticeNorms(lattice);
	return lattice;
}

// Free the memory used by the given lattice.
void DestroyLattice(Lattice *lattice)
{
	if(lattice->node)
		delete [] lattice->node;
	if(lattice->spring)
		delete [] lattice->spring;
	if(lattice->pgon)
		delete [] lattice->pgon;
	if(lattice->vertex)
		delete [] lattice->vertex;
	if(lattice->norm)
		delete [] lattice->norm;
	delete lattice;
}

int RegisterLatticeCollisionPolygon(Lattice *lattice, Polygon *pgon)
{
	return RegisterPointer((void **)lattice->coll_pgon, MAX_COLL_PGONS, pgon);
}

int UnregisterLatticeCollisionPolygon(Lattice *lattice, Polygon *pgon)
{
	return UnregisterPointer((void **)lattice->coll_pgon, MAX_COLL_PGONS, pgon);
}

int RegisterLatticeCollisionPlane(Lattice *lattice, Plane *plane)
{
	return RegisterPointer((void **)lattice->coll_plane, MAX_COLL_PLANES, plane);
}

int UnregisterLatticeCollisionPlane(Lattice *lattice, Plane *plane)
{
	return UnregisterPointer((void **)lattice->coll_plane, MAX_COLL_PLANES, plane);
}

int RegisterLatticeExternalForce(Lattice *lattice, Vector *force)
{
	return RegisterPointer((void **)lattice->ext_force, MAX_EXT_FORCES, force);
}

int UnregisterLatticeExternalForce(Lattice *lattice, Vector *force)
{
	return UnregisterPointer((void **)lattice->ext_force, MAX_EXT_FORCES, force);
}

// endof Spring.cpp
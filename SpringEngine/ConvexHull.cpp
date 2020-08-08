// ConvexHull.cpp -- do the gift-wrapping algorithm.

// this convex hull finder algorithm is
// pretty stable, but it does have some
// major numerical issues to work out.
// i'm hitting several different asserts
// on point-clouds with extreme cases.

// Notice that this algorithm is non-re-entrant!
// Also, routines that use the results of this
// algorithm are non-re-entrant.

#include "ConvexHull.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

// algorithm data:
Vector *pt_cloud = 0;
int num_cloud_pts = 0;
List PointList, EdgeList = {0, 0, 0}, PolygonList = {0, 0, 0};
List FinalEdgeList;
Edge ***EdgeMatrix = 0;

// memory for edges:
static const int NUM_EDGE_BLOCKS = 256;
static Edge edge_memory_buf[NUM_EDGE_BLOCKS];
static int edge_free_block_stack[NUM_EDGE_BLOCKS];
static MemPool edge_mp;

// memory for polygons:
static const int NUM_POLYGON_BLOCKS = 64;
static Polygon polygon_memory_buf[NUM_POLYGON_BLOCKS];
static int polygon_free_block_stack[NUM_POLYGON_BLOCKS];
static MemPool polygon_mp;

// memory for cloud points:
static const int NUM_POINT_BLOCKS = 128;
static CloudPoint point_memory_buf[NUM_POINT_BLOCKS];
static int point_free_block_stack[NUM_POINT_BLOCKS];
static MemPool point_mp;

// assuming the given points are unique,
// determine if they are colinear or not.
// if any two were the same, then they
// would all be colinear.
int PointsAreColinear(Vector *pt0, Vector *pt1, Vector *pt2)
{
	Vector v01, v02;
	VectorSub(&v01, pt1, pt0);
	VectorSub(&v02, pt2, pt0);
	Vector cross;
	VectorCross(&cross, &v01, &v02);
	if(VectorEq(&cross, &Origin))
		return 1;
	return 0;
}

// here we assume that the point list
// contains a set of unique co-planar
// points living in the given plane.
// note that no three consequtive vertices
// in the resulting polygon can be colinear.
// we need all edges to be as large as possible
// so that all edges are unique.  this is
// because we need to be able to identify
// existing edges within the developing 3D hull.
// return a convex polygon that is the 2D
// convex hull of the given points.  the points
// of the polygon will be ordered CCW WRT the
// front of the given plane.
Polygon *Find2DConvexHull(Plane *plane)
{
	assert(PointList.item_count > 2);

	// this will be an ordered looped-list of
	// the vertices making up the resulting
	// convex polygon.  the next direction is
	// CCW, and the prev direction is CW.
	List VertexList;
	WipeList(&VertexList);

	// use the first three points in the point
	// list.  make sure they're ordered the
	// right way: CCW!  this is also a point
	// where the algorithm could be sped up
	// by picking the largest possible tri-
	// angle.  i should look into this.
	// note that this triple-nested for-loop
	// looks bad, but it really isn't.
	CloudPoint *cpt0, *cpt1, *cpt2;	
	for(cpt0 = (CloudPoint *)PointList.head; cpt0; cpt0 = cpt0->next)
	{
		for(cpt1 = (CloudPoint *)PointList.head; cpt1; cpt1 = cpt1->next)
		{
			if(cpt1 == cpt0)
				continue;
			for(cpt2 = (CloudPoint *)PointList.head; cpt2; cpt2 = cpt2->next)
			{
				if(cpt2 == cpt0 || cpt2 == cpt1)
					continue;
				if(PointsAreColinear(cpt0->pt, cpt1->pt, cpt2->pt))
					continue;
				break;
			}
			if(cpt2)
				break;
		}
		if(cpt1)
			break;
	}
	assert(cpt0 && cpt1 && cpt2);

	ListRemove((ListItem *)cpt0);	
	ListRemove((ListItem *)cpt1);	
	ListRemove((ListItem *)cpt2);
	Vector v01, v02, vec;
	VectorSub(&v01, cpt1->pt, cpt0->pt);
	VectorSub(&v02, cpt2->pt, cpt0->pt);
	VectorCross(&vec, &v01, &v02);
	VectorNormalize(&vec, &vec);
	LoopListInsert(&VertexList, (ListItem *)cpt0);
	if(VectorEq(&vec, &plane->norm))
	{	
		LoopListInsert(&VertexList, (ListItem *)cpt2);
		LoopListInsert(&VertexList, (ListItem *)cpt1);
	}
	else
	{		
		LoopListInsert(&VertexList, (ListItem *)cpt1);
		LoopListInsert(&VertexList, (ListItem *)cpt2);
	}	

	// until we've emptied the point list...
	CloudPoint *cpt, *L_cpt, *R_cpt, *M_cpt;
	CloudPoint *next, *prev;	
	Plane edge_plane;
	int next_side, prev_side;
	int reject;	
	while(PointList.item_count > 0)
	{
		M_cpt = (CloudPoint *)PointList.head;
		ListRemove((ListItem *)M_cpt);

		// reject this cloud point if it
		// is inside of the currently
		// developping convex hull.  note
		// that this test works because
		// the vertices in the list always
		// form a convex polygon.
		cpt = (CloudPoint *)VertexList.head;
		reject = 1;
		do
		{
			VectorSub(&vec, cpt->next->pt, cpt->pt);
			VectorCross(&vec, &vec, &plane->norm);
			MakePlane(&edge_plane, cpt->pt, &vec);
			if(SideOfPlane(&edge_plane, M_cpt->pt) == ON_PLANE_FRONT)
				reject = 0;
			cpt = cpt->next;
		}
		while(cpt != (CloudPoint *)VertexList.head && reject);
		if(reject)
		{
			DeallocBlock(&point_mp, (char *)M_cpt);
			continue;
		}

		// find the left connecting cloud point.
		L_cpt = (CloudPoint *)VertexList.head;
		for(;/*ever*/;)
		{
			VectorSub(&vec, L_cpt->pt, M_cpt->pt);
			VectorCross(&vec, &plane->norm, &vec);
			MakePlane(&edge_plane, M_cpt->pt, &vec);

			prev_side = SideOfPlane(&edge_plane, L_cpt->prev->pt);
			next_side = SideOfPlane(&edge_plane, L_cpt->next->pt);

			if(prev_side != ON_PLANE_FRONT && next_side != ON_PLANE_FRONT)
				break;

			if(prev_side == ON_PLANE_FRONT && next_side == ON_PLANE_FRONT)
				L_cpt = L_cpt->next;		// Just pick one.  But be consistent.
			else if(next_side == ON_PLANE_FRONT)
				L_cpt = L_cpt->next;
			else
				L_cpt = L_cpt->prev;
		}
		
		// find the right connecting cloud point.
		R_cpt = (CloudPoint *)VertexList.head;
		for(;/*ever*/;)
		{
			VectorSub(&vec, R_cpt->pt, M_cpt->pt);
			VectorCross(&vec, &vec, &plane->norm);
			MakePlane(&edge_plane, M_cpt->pt, &vec);

			prev_side = SideOfPlane(&edge_plane, R_cpt->prev->pt);
			next_side = SideOfPlane(&edge_plane, R_cpt->next->pt);

			if(prev_side != ON_PLANE_FRONT && next_side != ON_PLANE_FRONT)
				break;

			if(prev_side == ON_PLANE_FRONT && next_side == ON_PLANE_FRONT)
				R_cpt = R_cpt->prev;		// Just pick one.  But be consistent.
			else if(next_side == ON_PLANE_FRONT)
				R_cpt = R_cpt->next;
			else
				R_cpt = R_cpt->prev;
		}

		assert(L_cpt != R_cpt);
		
		// now remove all the vertices between L_cpt and R_cpt
		// and insert M_cpt in their place.
		for(cpt = L_cpt->next; cpt != R_cpt; cpt = next)
		{
			next = cpt->next;
			LoopListRemove((ListItem *)cpt);
			DeallocBlock(&point_mp, (char *)cpt);
		}
		VertexList.head = (ListItem *)L_cpt;
		LoopListInsert(&VertexList, (ListItem *)M_cpt);
	
		// go through and weed out any vertex
		// points that are inbetween two vertices
		// that are colinear with it.  (when
		// there are only 3 vertices, this code
		// checks the same 3 points 3 times.)
		// notice that we must do this after
		// each additional vertex is added, because
		// having colinear edges will mess-up
		// the left/right point finding peice
		// of the algorithm.
		cpt = (CloudPoint *)VertexList.head;
		int i, j = VertexList.item_count;
		for(i = 0; i < j; i++)	
		{		
			next = cpt->next;
			prev = cpt->prev;
			if(PointsAreColinear(prev->pt, cpt->pt, next->pt))
			{
				assert(j != 3);
				LoopListRemove((ListItem *)cpt);
				DeallocBlock(&point_mp, (char *)cpt);
			}
			cpt = next;
		}
	}

	// construct the resulting polygon here
	// and then return it to the caller.
	Polygon *polygon = (Polygon *)AllocBlock(&polygon_mp);
	cpt = (CloudPoint *)VertexList.head;
	do
	{
		polygon->vtx[polygon->num_vertices++] = cpt->pt;
		cpt = cpt->next;
	}
	while(cpt != (CloudPoint *)VertexList.head);

	// free the remaining memory taken up by the vertex list.
	cpt = (CloudPoint *)VertexList.head;
	while(VertexList.item_count > 0)
	{
		next = cpt->next;
		LoopListRemove((ListItem *)cpt);
		DeallocBlock(&point_mp, (char *)cpt);
		cpt = next;
	}

	// okay, now return the polygon.	
	CopyPlane(&polygon->plane, plane);
	return polygon;
}

// build up a list of all the points in the cloud.
static void RebuildPointList(void)
{
	// kill the partial list if any.
	CloudPoint *cpt, *next;
	for(cpt = (CloudPoint *)PointList.head; cpt; cpt = next)
	{
		next = cpt->next;
		ListRemove((ListItem *)cpt);
		DeallocBlock(&point_mp, (char *)cpt);
	}	
	
	// populate the list.	
	int k;
	for(k = 0; k < num_cloud_pts; k++)
	{			
		cpt = (CloudPoint *)AllocBlock(&point_mp);
		cpt->pt = &pt_cloud[k];
		ListAppend(&PointList, (ListItem *)cpt);
	}
}

// remove all points in the point list
// that are strictly on the back-side
// of the given plane.
static void CullPoints(Plane *plane)
{
	CloudPoint *cpt, *next;
	for(cpt = (CloudPoint *)PointList.head; cpt; cpt = next)
	{		
		next = cpt->next;
		if(SideOfPlane(plane, cpt->pt) == ON_PLANE_BACK)
		{
			ListRemove((ListItem *)cpt);
			DeallocBlock(&point_mp, (char *)cpt);
		}
	}
}

// find and return the point in the point list
// that is furthest from the front of the given plane.
static CloudPoint *FurthestPoint(Plane *plane)
{
	double max_dist = 0.0, dist;
	CloudPoint *cpt, *fur_cpt = 0;
	for(cpt = (CloudPoint *)PointList.head; cpt; cpt = cpt->next)
	{
		if(SideOfPlane(plane, cpt->pt) == ON_PLANE_FRONT)
		{
			dist = DistanceToPlane(plane, cpt->pt);
			if(dist > max_dist)
			{
				max_dist = dist;
				fur_cpt = cpt;
			}
		}
	}
	return fur_cpt;
}

// assuming that the point cloud
// has been set and populated, find
// its convex hull.
void Find3DConvexHull(void)
{
	AllocEdgeMatrix();
	Reset3DConvexHullFinder();
	while(Iterate3DConvexHullFinder())
		;	// null statement.
	DeallocEdgeMatrix();
}

// colm through the point-cloud and remove
// as many points as possible that we know
// are not points on the surface of the
// hull.  one way to determine that a point
// is not on the surface is to see if it
// is inside any tetrahedron with vertices
// taken from the point-cloud.
static void TrimPointCloud(void)
{
	// think about it...
}

// here we assume that the pt_cloud
// buffer is a set of unique points
// in space.  if any two points are
// the same, then it may screw up
// some calculations, like that of
// constructing a plane or something
// like that.  it also messes with
// the uniqueness of edges we count on.
void Reset3DConvexHullFinder(void)
{
	RegisterMemPool(&edge_mp, (char *)edge_memory_buf, sizeof(Edge), NUM_EDGE_BLOCKS, edge_free_block_stack);
	RegisterMemPool(&polygon_mp, (char *)polygon_memory_buf, sizeof(Polygon), NUM_POLYGON_BLOCKS, polygon_free_block_stack);
	RegisterMemPool(&point_mp, (char *)point_memory_buf, sizeof(CloudPoint), NUM_POINT_BLOCKS, point_free_block_stack);
	
	WipeEdgeMatrix();

	WipeList(&EdgeList);
	WipeList(&FinalEdgeList);
	WipeList(&PolygonList);
	WipeList(&PointList);

	TrimPointCloud();
	RebuildPointList();

	// find 3 coplanar points on the surface
	// of the hull.  then find all points
	// coplanar to those points.  then find
	// the convex polygon of those points
	// and add that polygon to the list.
	// this is the part of the algorithm
	// that needs optimization the most.
	// there must be a better approach to
	// finding an initial polygon than this.	
	Vector pt;
	VectorCopy(&pt, &pt_cloud[0]);
	VectorSub(&pt, &pt, &Xaxis);
	Plane plane;
	MakePlane(&plane, &pt, &Xaxis);
	CloudPoint *cpt0, *cpt1, *cpt2;	
	cpt0 = FurthestPoint(&plane);	
	for(cpt1 = (CloudPoint *)PointList.head; cpt1; cpt1 = cpt1->next)
	{
		if(cpt1 == cpt0)
			continue;
		for(cpt2 = (CloudPoint *)PointList.head; cpt2; cpt2 = cpt2->next)
		{
			if(cpt2 == cpt0 || cpt2 == cpt1)
				continue;
			if(PointsAreColinear(cpt0->pt, cpt1->pt, cpt2->pt))
				continue;
			MakePlane(&plane, cpt0->pt, cpt1->pt, cpt2->pt);
			if(!FurthestPoint(&plane))
				break;
		}
		if(cpt2)
			break;
	}
	assert(cpt1 && cpt2);
	CullPoints(&plane);
	Polygon *polygon = Find2DConvexHull(&plane);
	assert(polygon);
	AddPolygon(polygon);
}

int Iterate3DConvexHullFinder(void)
{
	if(EdgeList.item_count <= 0)
		return 0;

	// pull an edge off the queue.
	// well, don't actually pull it off
	// the queue.  we'll let the AddPolygon()
	// routine do that when the time comes.
	Edge *e = (Edge *)EdgeList.head;

	// make a plane with this edge
	// and any point in the point cloud.
	// the plane will always be facing
	// the correct way since all of
	// the polygons are ordered CCW
	// WRT to the outside of the hull.
	RebuildPointList();
	Plane plane;
	CloudPoint *cpt = (CloudPoint *)PointList.head;
	for(cpt = (CloudPoint *)PointList.head; cpt; cpt = cpt->next)
	{
		if(cpt->pt == e->vtx[0] || cpt->pt == e->vtx[1])
			continue;
		if(PointsAreColinear(e->vtx[0], e->vtx[1], cpt->pt))
			continue;
		MakePlane(&plane, e->vtx[0], cpt->pt, e->vtx[1]);
		break;
	}
	assert(cpt);

	// swing the plane on the edge
	// as if the edge were a hinge
	// until the plane floats to
	// the surface.
	int k = 0;
	do
	{
		CullPoints(&plane);
		// since the furthest point must
		// be on the front side of the plane,
		// it should not be any one of the
		// two edges we're using here.
		cpt = FurthestPoint(&plane);
		if(cpt)
			MakePlane(&plane, e->vtx[0], cpt->pt, e->vtx[1]);
	}
	while(cpt);	

	// at this point, all the points in
	// the point-list should be those that
	// are coplanar to the plane.
	// create the 2d convex hull
	// polygon for those points
	// and add its polygon to the list.
	Polygon *polygon = Find2DConvexHull(&plane);
//	assert(polygon);
	if(polygon)
		AddPolygon(polygon);
	else
	{
		// HACK!!! -- remove the edge.  it didn't work out.
		ListRemove((ListItem *)e);
	}

	//tmp...
//	printf("edge count = %d\n", EdgeList.item_count);

	// return zero if we're done,
	// or one if we're not done.
	return(EdgeList.item_count > 0);
}

// add the given polygon to the polygon
// list.  also, add its edges to the
// edge list that are not already members
// of it.  for those that are, remove
// them from the edge list.
void AddPolygon(Polygon *polygon)
{
	ListAppend(&PolygonList, (ListItem *)polygon);
	// we depend on the vertices of the given
	// polygon to be ordered CCW with respect
	// to the front of the plane used in
	// determining the polygon.  it should also
	// be a convex polygon.
	Edge *e;
	int i, j;
	for(i = 0; i < polygon->num_vertices; i++)
	{
		j = (i + 1) % polygon->num_vertices;
		e = LookUpEdge(polygon->vtx[i], polygon->vtx[j]);
		// if the edge already exists, then it
		// should be a member of the edge list.
		if(e)
		{
//			assert(((ListItem *)e)->list != 0);
			if(!e->list)
			{
				// HACK!!! -- for now, just empty
				// the entire edge list.  also
				// remove the polygon we just added.
				ListRemove((ListItem *)polygon);
				while(EdgeList.item_count > 0)
					ListRemove(EdgeList.head);
				return;
			}
//			printf("edge 0x%08x -- 0x%08x removed\n", e->vtx[0], e->vtx[1]);
			ListRemove((ListItem *)e);
			ListAppend(&FinalEdgeList, (ListItem *)e);
		}
		else
		{
			e = (Edge *)AllocBlock(&edge_mp);
			// notice that the order of the
			// edge vertices is very important!
			e->vtx[0] = polygon->vtx[i];
			e->vtx[1] = polygon->vtx[j];
			RegisterEdge(e);
			ListAppend(&EdgeList, (ListItem *)e);
//			printf("edge 0x%08x -- 0x%08x added\n", e->vtx[0], e->vtx[1]);
		}
	}

//	printf("added poly with %d vertices\n", polygon->num_vertices);
}

void AllocEdgeMatrix(void)
{
	int i;
	EdgeMatrix = new Edge **[num_cloud_pts];
	for(i = 0; i < num_cloud_pts; i++)	
		EdgeMatrix[i] = new Edge *[num_cloud_pts];
}

void DeallocEdgeMatrix(void)
{
	int i;
	for(i = 0; i < num_cloud_pts; i++)
		delete [] EdgeMatrix[i];
	delete [] EdgeMatrix;
}

// zero the edge matrix pointer memory.
void WipeEdgeMatrix(void)
{
	int i, j;
	for(i = 0; i < num_cloud_pts; i++)
		for(j = 0; j < num_cloud_pts; j++)
			EdgeMatrix[i][j] = 0;
}

// put the given edge into the edge matrix
// for later reference.  notice that the
// edge matrix is symmetric.
void RegisterEdge(Edge *e)
{
	int i, j;
	i = e->vtx[0] - pt_cloud;
	j = e->vtx[1] - pt_cloud;
	EdgeMatrix[i][j] = e;
	EdgeMatrix[j][i] = e;
}

// lookup an edge by vertices.
Edge *LookUpEdge(Vector *vtx0, Vector *vtx1)
{
	int i, j;
	i = vtx0 - pt_cloud;
	j = vtx1 - pt_cloud;
	return EdgeMatrix[i][j];
}

// calculate six times the volume of the given tetrahedron.
// this would also be the volume of the parallel pipe
// determined by the 4 given points.
static double CalcSixTetrahedronVolume(Vector *v0, Vector *v1, Vector *v2, Vector *v3)
{
	Vector v01, v02, v03;
	VectorSub(&v01, v1, v0);
	VectorSub(&v02, v2, v0);
	VectorSub(&v03, v3, v0);
	Vector cross;
	VectorCross(&cross, &v02, &v03);
	return fabs(VectorDot(&v01, &cross));
}

// compute the volume of the given polygon-based pyramid.
static double CalcPyramidVolume(Polygon *polygon, Vector *vtx)
{
	double vol = 0.0;
	if(polygon->num_vertices == 3)
	{
		vol = CalcSixTetrahedronVolume(vtx, polygon->vtx[0], polygon->vtx[1], polygon->vtx[2]);
	}
	else
	{
		Vector center;
		VectorCopy(&center, &Origin);
		int i, j;
		for(i = 0; i < polygon->num_vertices; i++)
			VectorAdd(&center, &center, polygon->vtx[i]);
		VectorScale(&center, &center, 1.0 / double(polygon->num_vertices));
		for(i = 0; i < polygon->num_vertices; i++)
		{
			j = (i + 1) % polygon->num_vertices;
			vol += CalcSixTetrahedronVolume(vtx, &center, polygon->vtx[i], polygon->vtx[j]);
		}
	}
	return vol;
}

// assuming that the hull has been found
// compute the volume of the hull by using
// the polygon list.
double Calc3DConvexHullVolume(void)
{
	Vector center;
	VectorCopy(&center, &Origin);
	int k;
	for(k = 0; k < num_cloud_pts; k++)
		VectorAdd(&center, &center, &pt_cloud[k]);
	VectorScale(&center, &center, 1.0 / double(num_cloud_pts));
	double vol = 0.0;
	Polygon *polygon;
	for(polygon = (Polygon *)PolygonList.head; polygon; polygon = polygon->next)
		vol += CalcPyramidVolume(polygon, &center);
	return vol / 6.0;
}

double CalcTwoPolygonArea(Polygon *polygon)
{
	double area = 0.0;
	if(polygon->num_vertices == 3)
	{
		Vector v01, v02, cross;
		VectorSub(&v01, polygon->vtx[1], polygon->vtx[0]);
		VectorSub(&v02, polygon->vtx[2], polygon->vtx[0]);
		VectorCross(&cross, &v01, &v02);
		area = VectorLength(&cross);
	}
	else
	{
		int k;
		Vector v01, v02, cross, center;
		VectorCopy(&center, &Origin);
		for(k = 0; k < polygon->num_vertices; k++)
			VectorAdd(&center, &center, polygon->vtx[k]);
		VectorScale(&center, &center, 1.0 / double(polygon->num_vertices));
		for(k = 0; k < polygon->num_vertices; k++)
		{
			VectorSub(&v01, polygon->vtx[k], &center);
			VectorSub(&v02, polygon->vtx[(k+1)%polygon->num_vertices], &center);
			VectorCross(&cross, &v01, &v02);
			area += VectorLength(&cross);
		}
	}
	return area;
}

double Calc3DConvexHullSurfaceArea(void)
{
	double area = 0.0;
	Polygon *polygon;
	for(polygon = (Polygon *)PolygonList.head; polygon; polygon = polygon->next)
		area += CalcTwoPolygonArea(polygon);
	return area / 2.0;
}

// endof ConvexHull.cpp
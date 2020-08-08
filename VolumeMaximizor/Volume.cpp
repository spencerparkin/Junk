// Volume.cpp

// This software was written by Spencer T. Parkin, Copyright (c) 2007, All Rights Reserved.

// one of the many things that makes my convex-hull-finder algorithm
// so terrible is that it deals with intermediate hulls that are not
// convex.  an algorithm first described by K.L. Clarkson and P.W. Shor
// in 1989 appears to be similar to mine while avoiding any concave
// hulls during the intermediate stages of the algorithm.

#include "Volume.h"
#include "List.h"
#include "MemMgr.h"
#include "Plane.h"
#include <math.h>
#include <stdio.h>

// this has the algorithm make more efficient
// use of memory, however, it will be slower.
#define USE_LESS_MEMORY

// note that this algorithm can't handle
// duplicate points in the point-cloud.

static const int TRI_BUFSIZ = 512;
static const int TET_BUFSIZ = 512;
static const int CAND_PT_BUFSIZ = MAX_CLOUD_POINTS;
static Triangle TriBuffer[TRI_BUFSIZ];
static Tetrahedron TetBuffer[TET_BUFSIZ];
static CandidatePoint CandPointBuffer[CAND_PT_BUFSIZ];
static int tri_free_block_stack[TRI_BUFSIZ];
static int tet_free_block_stack[TET_BUFSIZ];
static int cand_pt_free_block_stack[CAND_PT_BUFSIZ];
static MemPool tri_mp, tet_mp, cand_pt_mp;
#ifndef USE_LESS_MEMORY
static Triangle *TriMatrix[MAX_CLOUD_POINTS][MAX_CLOUD_POINTS][MAX_CLOUD_POINTS];	// yes, that's a lot of memory.
#endif // USE_LESS_MEMORY
static Vector *pt_cloud = 0;
struct List TriList = {0, 0, 0}, TetList = {0, 0, 0};
struct List TriMeshList = {0, 0, 0};
struct List CandPtList = {0, 0, 0};

void ResetVolumeAlgorithm(Vector *PointCloud, int NumCloudPoints)
{
	WipeList(&TriList);
	WipeList(&TriMeshList);
	WipeList(&TetList);
	WipeList(&CandPtList);

	if(NumCloudPoints < 4 || NumCloudPoints > MAX_CLOUD_POINTS)
		return;	

	pt_cloud = PointCloud;

	RegisterMemPool(&tri_mp, (char *)TriBuffer, sizeof(Triangle), TRI_BUFSIZ, tri_free_block_stack);
	RegisterMemPool(&tet_mp, (char *)TetBuffer, sizeof(Tetrahedron), TET_BUFSIZ, tet_free_block_stack);
	RegisterMemPool(&cand_pt_mp, (char *)CandPointBuffer, sizeof(CandidatePoint), CAND_PT_BUFSIZ, cand_pt_free_block_stack);			

	int i, j, k, l;
#ifndef USE_LESS_MEMORY
	for(i = 0; i < NumCloudPoints; i++)
		for(j = 0; j < NumCloudPoints; j++)
			for(k = 0; k < NumCloudPoints; k++)
				TriMatrix[i][j][k] = 0;
#endif // USE_LESS_MEMORY

	// build the initial candidate-point list.	

	CandidatePoint *cpt;
	for(i = 0; i < NumCloudPoints; i++)
	{
		cpt = (CandidatePoint *)AllocBlock(&cand_pt_mp);
		cpt->pt = &PointCloud[i];
		ListAppend(&CandPtList, (ListItem *)cpt);
	}

	// find an initial tetrahedron
	// in the point cloud.  add it
	// to the tet-list.

	Tetrahedron *tet = 0;
	Plane plane;
	Vector *v0, *v1, *v2, *v3;	
	for(i = 0; i < NumCloudPoints && !tet; i++)
	{
		v0 = &PointCloud[i];
		for(j = 0; j < NumCloudPoints && !tet; j++)
		{
			v1 = &PointCloud[j];
			if(v1 == v0)
				continue;
			for(k = 0; k < NumCloudPoints && !tet; k++)
			{
				v2 = &PointCloud[k];
				if(v2 == v0 || v2 == v1)
					continue;
				if(ArePointsColinear(v0, v1, v2))
					continue;
				MakePlane(&plane, v0, v1, v2);
				// find a point on the back-side of this plane.
				for(l = 0; l < NumCloudPoints && !tet; l++)
				{
					v3 = &PointCloud[l];
					if(v3 == v0 || v3 == v1 || v3 == v2)
						continue;
					if(ArePointsCoplanar(v0, v1, v2, v3))	// shouldn't be needed.
						continue;
					if(SideOfPlane(&plane, v3) == ON_PLANE_BACK)
					{
						tet = (Tetrahedron *)AllocBlock(&tet_mp);
						tet->tri[0] = (Triangle *)AllocBlock(&tri_mp);
						tet->tri[1] = (Triangle *)AllocBlock(&tri_mp);
						tet->tri[2] = (Triangle *)AllocBlock(&tri_mp);
						tet->tri[3] = (Triangle *)AllocBlock(&tri_mp);
						tet->tri[0]->vtx[0] = v0;	// order matters here!
						tet->tri[0]->vtx[1] = v1;
						tet->tri[0]->vtx[2] = v2;
						tet->tri[1]->vtx[0] = v0;
						tet->tri[1]->vtx[1] = v3;
						tet->tri[1]->vtx[2] = v1;
						tet->tri[2]->vtx[0] = v1;
						tet->tri[2]->vtx[1] = v3;
						tet->tri[2]->vtx[2] = v2;
						tet->tri[3]->vtx[0] = v2;
						tet->tri[3]->vtx[1] = v3;
						tet->tri[3]->vtx[2] = v0;
						tet->v[0] = v0;
						tet->v[1] = v1;
						tet->v[2] = v2;
						tet->v[3] = v3;
						tet->six_vol = CalcSixTetrahedronVol(tet);
						if(tet->six_vol < 0.5)
						{
							DeallocBlock(&tri_mp, (char *)tet->tri[0]);
							DeallocBlock(&tri_mp, (char *)tet->tri[1]);
							DeallocBlock(&tri_mp, (char *)tet->tri[2]);
							DeallocBlock(&tri_mp, (char *)tet->tri[3]);
							DeallocBlock(&tet_mp, (char *)tet);
							tet = 0;
						}
					}
				}
			}
		}
	}
	
	if(!tet)
		return;

	AddTetrahedron(tet);	
}

void AddTetrahedron(Tetrahedron *tet)
{
	ListAppend(&TetList, (ListItem *)tet);
	Triangle *tri;
	int k;
	int count = 0;
	for(k = 0; k < 4; k++)
	{
		tri = tet->tri[k];
		tri = FindTriangle(tri->vtx[0], tri->vtx[1], tri->vtx[2]);
		if(tri)
		{
			// we don't want to duplicate triangles in
			// the tri-list.  such triangles cannot
			// be seen since they're where the adjacent
			// tetrahedrons are flesh with one another.
			// remove the tri if it's in the list.			
			if(tri->list == &TriList)
			{
 				ListRemove((ListItem *)tri);
				count++;
			}
		}
		else
		{
			tri = tet->tri[k];
			ListAppend(&TriList, (ListItem *)tri);
			RegisterTriangle(tri);
		}
	}	

//	printf("%d tries removed!\n", count);

	// occlude candidate points that are inside
	// of this tetrahedron.  points inside of it
	// are obviously no longer candiates.  notice that
	// we're not getting rid of all of the non-candidates.
	// the ones that survive are those that are vertices
	// of the tetrahedrons. removing them is as difficult
	// as determining whether or not a point is inside of
	// a non-convex (concave) space, which is the situation
	// at the intermidate stages of the algorithm.  we
	// need to keep the tetrahedron vertices as candiates
	// so that the tetrahedrons all clump together.
	Plane plane_set[4];
	for(k = 0; k < 4; k++)
		MakePlaneFromTriangle(&plane_set[k], tet->tri[k]);
	CandidatePoint *cpt, *next;
	for(cpt = (CandidatePoint *)CandPtList.head; cpt; cpt = next)
	{
		next = cpt->next;
		for(k = 0; k < 4; k++)		
			if(SideOfPlane(&plane_set[k], cpt->pt) != ON_PLANE_BACK)
				break;
		if(k == 4)
		{
			ListRemove((ListItem *)cpt);
			DeallocBlock(&cand_pt_mp, (char *)cpt);
		}
	}
}

// assuming that the flags in the candidate-point
// list have been setup properly, return the furthest
// point on the front side of the given plane that
// has not yet been chosen.  1 is returned if found,
// otherwise zero is returned if not.  if a point
// is chosen, then its flag is set.
static int PickBestCandidatePoint(CandidatePoint **best_cpt, Plane *plane)
{
	*best_cpt = 0;
	CandidatePoint *cpt;
	double dist, max_dist = 0.0;
	for(cpt = (CandidatePoint *)CandPtList.head; cpt; cpt = cpt->next)
	{
		if(cpt->flag)
			continue;
		if(SideOfPlane(plane, cpt->pt) == ON_PLANE_FRONT)
		{
			// perform a vector projection of this candidate point
			// onto the norm of the given plane.  note that we are
			// assuming that the plane normal is of unit length!
//			dist = VectorDot(cpt->pt, &plane->norm);
			dist = DistanceToPlane(plane, cpt->pt);
			if(dist > max_dist)
			{
				max_dist = dist;
				*best_cpt = cpt;
			}
		}
	}
	if(*best_cpt)
	{
		(*best_cpt)->flag = 1;
		return 1;
	}
	return 0;
}

static int PassesTriFleshTest(Triangle *tri0)
{
	Triangle *tri1= FindTriangle(tri0->vtx[0], tri0->vtx[1], tri0->vtx[2]);
	if(tri1)
		return 1;
	int i, j;
	Plane plane0, plane1, plane_set0[3], plane_set1[3];
	Vector edge_vec, norm;		
	MakePlaneFromTriangle(&plane0, tri0);
	double t;
	for(tri1 = (Triangle *)TriList.head; tri1; tri1 = tri1->next)
	{
		// are the triangles coplanar?
		if(SideOfPlane(&plane0, tri1->vtx[0]) != ON_PLANE)
			continue;
		if(SideOfPlane(&plane0, tri1->vtx[1]) != ON_PLANE)
			continue;
		if(SideOfPlane(&plane0, tri1->vtx[2]) != ON_PLANE)
			continue;
		
		// are the triangles facing one another?
		MakePlaneFromTriangle(&plane1, tri1);
		if(VectorAngle(&plane0.norm, &plane1.norm) < PI / 2.0)
			continue;

		// the question now is: do they overlap in anyway?

		// do all the vertices of one triangle lie completely on
		// one side of a plane determined by an edge of the other triangle?
		for(i = 0; i < 3; i++)
		{
			VectorSub(&edge_vec, tri0->vtx[(i+1)%3], tri0->vtx[i]);
			VectorCross(&norm, &edge_vec, &plane0.norm);
			MakePlane(&plane_set0[i], tri0->vtx[i], &norm);

			for(j = 0; j < 3; j++)			
				if(SideOfPlane(&plane_set0[i], tri1->vtx[j]) == ON_PLANE_BACK)
					break;
			if(j == 3)
				break;

			VectorSub(&edge_vec, tri1->vtx[(i+1)%3], tri1->vtx[i]);
			VectorCross(&norm, &edge_vec, &plane1.norm);
			MakePlane(&plane_set1[i], tri1->vtx[i], &norm);

			for(j = 0; j < 3; j++)
				if(SideOfPlane(&plane_set1[i], tri0->vtx[j]) == ON_PLANE_BACK)
					break;
			if(j == 3)
				break;
		}
		if(i < 3)
			continue;

		// is any one point of one triangle inside the other triangle?
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
				if(SideOfPlane(&plane_set0[j], tri1->vtx[i]) != ON_PLANE_BACK)
					break;
			if(j == 3)
				break;

			for(j = 0; j < 3; j++)
				if(SideOfPlane(&plane_set1[j], tri0->vtx[i]) != ON_PLANE_BACK)
					break;
			if(j == 3)
				break;
		}
		if(i < 3)
			return 0;

		// do any of the edges of one triangle cross a plane determined
		// by an edge of the other triangle?
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
			{
				t = LinePlaneIntersection(&plane_set0[i], tri1->vtx[j], tri1->vtx[(j+1)%3]);
				if(t > 0.0 && t < 1.0)
					return 0;
				t = LinePlaneIntersection(&plane_set1[j], tri0->vtx[j], tri0->vtx[(j+1)%3]);
				if(t > 0.0 && t < 1.0)
					return 0;
			}
		}

		// at this point, i believe that the triangles
		// do not overlap.
	}
	return 1;
}

// a fatal flaw has been found in the idea/algorithm: we
// must be smarter about which point we pick to complete
// a new tetrahedron.  we don't want tetrahedrons having
// faces that are flesh with one another, while at the
// same time not sharing all vertices in common.  this
// causes tri-mesh triangles to be generated that are not
// really on the surface of the hull.
static int PassesTriFleshTest(Tetrahedron *tet)
{
	Triangle *tri;
	int k;	
	for(k = 0; k < 4; k++)
	{
		tri = tet->tri[k];
		if(!PassesTriFleshTest(tri))
			return 0;
	}
	return 1;
}

int PerformVolumeAlgorithmIteration(void)
{
	if(TriList.item_count <= 0)
		return 0;

	// notice that we do not remove the tri
	// from the list.
	Triangle *tri = (Triangle *)TriList.head;	

	// find the tetrahedron.  if it can't be found,
	// then this tri must be on the surface of the hull.

	// compute the plane determined by the triangle.
	Plane plane;
	MakePlaneFromTriangle(&plane, tri);

	// initialize all of the candidate point's flags to zero,
	// indicating that we have not tried them yet.
	CandidatePoint *cpt;
	for(cpt = (CandidatePoint *)CandPtList.head; cpt; cpt = cpt->next)
		cpt->flag = 0;

	// allocate memory for a tetrahedron.
	Tetrahedron *tet0 = (Tetrahedron *)AllocBlock(&tet_mp), *tet1;
	tet0->tri[0] = (Triangle *)AllocBlock(&tri_mp);
	tet0->tri[1] = (Triangle *)AllocBlock(&tri_mp);
	tet0->tri[2] = (Triangle *)AllocBlock(&tri_mp);
	tet0->tri[3] = (Triangle *)AllocBlock(&tri_mp);

	// partially initialize the tetrahedron.
	Vector *v0, *v1, *v2, *v3;
	v0 = tri->vtx[2];	// notice the reverse order!!
	v1 = tri->vtx[1];
	v2 = tri->vtx[0];
	tet0->tri[0]->vtx[0] = v0;
	tet0->tri[0]->vtx[1] = v1;
	tet0->tri[0]->vtx[2] = v2;
	tet0->tri[1]->vtx[0] = v0;
	tet0->tri[1]->vtx[2] = v1;
	tet0->tri[2]->vtx[0] = v1;
	tet0->tri[2]->vtx[2] = v2;
	tet0->tri[3]->vtx[0] = v2;
	tet0->tri[3]->vtx[2] = v0;
	tet0->v[0] = v0;
	tet0->v[1] = v1;
	tet0->v[2] = v2;

	// cycle through all of the candidate points
	// until we find one that works, or if none
	// do, then tri must be on the surface of the hull.
	while(PickBestCandidatePoint(&cpt, &plane))
	{
		// finish initializing the tetrahedron.
		v3 = cpt->pt;
		tet0->tri[1]->vtx[1] = v3;		
		tet0->tri[2]->vtx[1] = v3;		
		tet0->tri[3]->vtx[1] = v3;
		tet0->v[3] = v3;

		// weed out tetrahedrons that don't
		// make a difference in volume.  this
		// might be more of a hack-solution
		// than anything else.
		tet0->six_vol = CalcSixTetrahedronVol(tet0);
		if(tet0->six_vol < 0.05)
			continue;

		// see the routine header comment
		if(!PassesTriFleshTest(tet0))
			continue;

		// now go make sure it doesn't intersect any
		// of the other tetrahedrons.
		for(tet1 = (Tetrahedron *)TetList.head; tet1; tet1 = tet1->next)		
			if(TetSideOfPlane(&plane, tet1) & TET_PT_ON_PLANE_FRONT)			
				if(DoTetrahedronsIntersect(tet0, tet1))				
					break;				
		
		// if no tetrahedron intersected the one
		// that we constructed, then keep it.
		if(!tet1)
		{
			AddTetrahedron(tet0);
			break;
		}
	}

	// if no tetrahedron worked out, then free the memory,
	// and mark the triangle face as one being on the surface
	// of the hull, by removing it to the tri-mesh list.
	if(tet1)
	{
		DeallocBlock(&tri_mp, (char *)tet0->tri[0]);
		DeallocBlock(&tri_mp, (char *)tet0->tri[1]);
		DeallocBlock(&tri_mp, (char *)tet0->tri[2]);
		DeallocBlock(&tri_mp, (char *)tet0->tri[3]);
		DeallocBlock(&tet_mp, (char *)tet0);
		ListRemove((ListItem *)tri);
		ListAppend(&TriMeshList, (ListItem *)tri);
	}	

	// the algorithm terminates
	// when the tri-list becomes empty.
	return TriList.item_count;
}

Triangle *FindTriangle(Vector *v0, Vector *v1, Vector *v2)
{
#ifndef USE_LESS_MEMORY
	int i, j, k;
	i = v0 - pt_cloud;
	j = v1 - pt_cloud;
	k = v2 - pt_cloud;
	Triangle *tri = TriMatrix[i][j][k];
	return tri;
#else
	Triangle *tri;
	for(tri = (Triangle *)TriList.head; tri; tri = tri->next)
	{
		// every triangle should have unique vertices.  it is
		// also assumed that the given vertices are unique.
		if(tri->vtx[0] != v0 && tri->vtx[1] != v0 && tri->vtx[2] != v0)
			continue;
		if(tri->vtx[0] != v1 && tri->vtx[1] != v1 && tri->vtx[2] != v1)
			continue;
		if(tri->vtx[0] == v2 || tri->vtx[1] == v2 || tri->vtx[2] == v2)
			return tri;
	}
	return 0;
#endif // USE_LESS_MEMORY
}

void RegisterTriangle(Triangle *tri)
{
#ifndef USE_LESS_MEMORY
	int i, j, k;
	i = tri->vtx[0] - pt_cloud;
	j = tri->vtx[1] - pt_cloud;
	k = tri->vtx[2] - pt_cloud;
	TriMatrix[i][j][k] = tri;
	TriMatrix[i][k][j] = tri;
	TriMatrix[j][k][i] = tri;
	TriMatrix[j][i][k] = tri;
	TriMatrix[k][i][j] = tri;
	TriMatrix[k][j][i] = tri;
#endif // USE_LESS_MEMORY
}

double CalcVolume(Vector *PointCloud, int NumCloudPoints)
{
	ResetVolumeAlgorithm(PointCloud, NumCloudPoints);
	int loop_count = 0;
	while(PerformVolumeAlgorithmIteration())
	{
		if(TetList.item_count > 64)
		{
			int q = 0;
			break;
		}

		// if this happens, there is a bug
		// in the volume code.  and it is
		// currently happening.  i'm not
		// sure what the bug is yet.
		if(++loop_count > 1024)
		{
//			assert(0);
			printf("bailed!\n");
			break;
		}
	}	
	return CalcVolume();
}

// call this after the convex hull has been found.
double CalcVolume(void)
{
	double vol;
//	if(TriList.item_count < TetList.item_count)
//		vol = CalcVolumeFromTriMeshList();
//	else
		vol = CalcVolumeFromTetList();
	return vol;
}

double CalcVolumeFromTriMeshList(void)
{
	Vector center, tri_center;
	VectorCopy(&center, &Origin);
	Triangle *tri;
	for(tri = (Triangle *)TriMeshList.head; tri; tri = tri->next)
	{
		VectorAdd(&tri_center, tri->vtx[0], tri->vtx[1]);
		VectorAdd(&tri_center, &tri_center, tri->vtx[2]);
		VectorScale(&tri_center, &tri_center, 1.0 / 3.0);
		VectorAdd(&center, &center, &tri_center);
	}
	VectorScale(&center, &center, 1.0 / double(TriMeshList.item_count));
	double six_vol = 0.0;
	Vector v0, v1, v2;
	Vector cross;
	for(tri = (Triangle *)TriMeshList.head; tri; tri = tri->next)
	{
		VectorSub(&v0, tri->vtx[0], &center);
		VectorSub(&v1, tri->vtx[1], &center);
		VectorSub(&v2, tri->vtx[2], &center);
		VectorCross(&cross, &v1, &v2);
		six_vol += fabs(VectorDot(&v0, &cross));		
	}
	return six_vol / 6.0;
}

double CalcSixTetrahedronVol(Tetrahedron *tet)
{
	Vector v01, v02, v03;
	Vector cross;
	VectorSub(&v01, tet->v[1], tet->v[0]);
	VectorSub(&v02, tet->v[2], tet->v[0]);
	VectorSub(&v03, tet->v[3], tet->v[0]);
	VectorCross(&cross, &v02, &v03);
	return fabs(VectorDot(&v01, &cross));
}

double CalcVolumeFromTetList(void)
{	
	double six_vol = 0.0;
	Tetrahedron *tet;
	for(tet = (Tetrahedron *)TetList.head; tet; tet = tet->next)	
		six_vol += tet->six_vol; //CalcSixTetrahedronVol(tet);	
	return six_vol / 6.0;
}

// determine the situation of the points of the
// given tetrahedron with respect to the given plane.
int TetSideOfPlane(Plane *plane, Tetrahedron *tet)
{
	int side_bits = 0, k;
	for(k = 0; k < 4; k++)
	{
		switch(SideOfPlane(plane, tet->v[k]))
		{
			case ON_PLANE:			side_bits |= TET_PT_ON_PLANE;		break;
			case ON_PLANE_FRONT:	side_bits |= TET_PT_ON_PLANE_FRONT;	break;
			case ON_PLANE_BACK:		side_bits |= TET_PT_ON_PLANE_BACK;	break;
		}
	}
	return side_bits;
}

// make the plane determined by the given triangle.
// notice that the order of the vertices of the tri
// will determine the front-face of the plane.
void MakePlaneFromTriangle(Plane *plane, Triangle *tri)
{
	Vector vec01, vec02;
	VectorSub(&vec01, tri->vtx[1], tri->vtx[0]);
	VectorSub(&vec02, tri->vtx[2], tri->vtx[0]);
	Vector vec;
	VectorCross(&vec, &vec01, &vec02);
	MakePlane(plane, tri->vtx[0], &vec);
}

int DoTetrahedronsIntersect(Tetrahedron *tet0, Tetrahedron *tet1)
{
	Plane plane_set0[4], plane_set1[4];
	int i, j;
	int side_bits;	

	// for each plane of tet0, determine if the points of tet1
	// lie entirely on one side of that plane or not.  if this
	// happens at least once, then there is no intersection.
	// also do this for tet1 with respect to tet0.
	for(i = 0; i < 4; i++)
	{
		MakePlaneFromTriangle(&plane_set0[i], tet0->tri[i]);		
		side_bits = TetSideOfPlane(&plane_set0[i], tet1);
		if(!(side_bits & TET_PT_ON_PLANE_BACK))
			return 0;
		MakePlaneFromTriangle(&plane_set1[i], tet1->tri[i]);
		side_bits = TetSideOfPlane(&plane_set1[i], tet0);
		if(!(side_bits & TET_PT_ON_PLANE_BACK))
			return 0;
	}

	// if any one point of one tetrahedron lies inside
	// of the other tetrahedron, then there is an intersection.
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)		
			if(SideOfPlane(&plane_set0[j], tet1->v[i]) != ON_PLANE_BACK)
				break;		
		if(j == 4)
			return 1;

		for(j = 0; j < 4; j++)
			if(SideOfPlane(&plane_set1[j], tet0->v[i]) != ON_PLANE_BACK)
				break;
		if(j == 4)
			return 1;
	}

	// if any edge of tet0 intersects any triangle face of tet1,
	// then we have an intersection.  the same is true about tet1
	// with respect to tet0.
	for(i = 0; i < 4; i++)
	{
		if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->v[0], tet1->v[1])) return 1;
		if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->v[0], tet1->v[2])) return 1;
		if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->v[0], tet1->v[3])) return 1;
		if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->v[1], tet1->v[2])) return 1;
		if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->v[1], tet1->v[3])) return 1;
		if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->v[2], tet1->v[3])) return 1;
		
		if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->v[0], tet0->v[1])) return 1;
		if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->v[0], tet0->v[2])) return 1;
		if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->v[0], tet0->v[3])) return 1;
		if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->v[1], tet0->v[2])) return 1;
		if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->v[1], tet0->v[3])) return 1;
		if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->v[2], tet0->v[3])) return 1;
	}
	
	// *** at this point it is still possible that the tetrahedrons intersect!!!
	// but i'm banking on that being fairly unlikely.  the obviouse example
	// is when tet0 == tet1 or they just happen to have the same vertex data.
	// i can think of other examples where tet0 and tet1 have different vertex data.
	// the following code might help.  at the moment, i can't see why it is
	// not a solution to the problem.  in other words, i can't think of an example
	// where this still won't catch an intersection.  well, the only example i
	// can think of is if tet0 == tet1 again, but that won't happen.
#if 0		// but if this code isn't needed for the most part, then don't include it since it's slow.
	Vector pt;
	int k;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			for(k = 0; k < 3; k++)
			{
				VectorAdd(&pt, tet1->tri[j]->vtx[(k+1)%3], tet1->tri[j]->vtx[(k+2)%3]);
				VectorScale(&pt, &pt, 0.5);
				if(DoesLineSegIntersectTriangle(tet0->tri[i], &plane_set0[i], tet1->tri[j]->vtx[k], &pt))
					return 1;
				VectorAdd(&pt, tet0->tri[j]->vtx[(k+1)%3], tet0->tri[j]->vtx[(k+2)%3]);
				VectorScale(&pt, &pt, 0.5);
				if(DoesLineSegIntersectTriangle(tet1->tri[i], &plane_set1[i], tet0->tri[j]->vtx[k], &pt))
					return 1;
			}
		}
	}
#endif

	// if we past that gauntlet, then i sure hope there is no intersection.
	return 0;
}

// compute twice the area of the triangle with the given vertices.
double TwoAreaOfTriangle(Vector *v0, Vector *v1, Vector *v2)
{
	Vector vec01, vec02;
	VectorSub(&vec01, v1, v0);
	VectorSub(&vec02, v2, v0);
	Vector cross;
	VectorCross(&cross, &vec01, &vec02);
	return VectorLength(&cross);
}

// determine if the given line-segment intersects the
// given triangle.  the given plane is assumed to be
// the plane determined by the given triangle.
int DoesLineSegIntersectTriangle(Triangle *tri, Plane *plane, Vector *pt0, Vector *pt1)
{
	if(!plane)
	{
		static Plane __plane__;
		plane = &__plane__;
		MakePlaneFromTriangle(plane, tri);
	}

	// if one of the end-points is on the plane
	// or if both are on the same side of the
	// plane, then there is no intersection.
	int side0, side1;
	side0 = SideOfPlane(plane, pt0);
	if(side0 == ON_PLANE)
		return 0;
	side1 = SideOfPlane(plane, pt1);
	if(side1 == ON_PLANE)
		return 0;
	if(side0 == side1)
		return 0;
	
	// determine if the point of intersection
	// between the line-segment and the plane
	// is inside of the given triangle.
	double t = LinePlaneIntersection(plane, pt0, pt1);
	Vector pt;
	VectorLerp(&pt, pt0, pt1, t);
	double area = TwoAreaOfTriangle(tri->vtx[0], tri->vtx[1], tri->vtx[2]);
	double total_area = 0.0;
	total_area += TwoAreaOfTriangle(tri->vtx[0], tri->vtx[1], &pt);
	if(total_area > area + EPS)
		return 0;
	total_area += TwoAreaOfTriangle(tri->vtx[1], tri->vtx[2], &pt);
	if(total_area > area + EPS)
		return 0;
	total_area += TwoAreaOfTriangle(tri->vtx[2], tri->vtx[0], &pt);
	if(total_area > area + EPS)
		return 0;

	return 1;
}

int ArePointsColinear(Vector *pt0, Vector *pt1, Vector *pt2)
{
	Vector v0, v1;
	VectorSub(&v0, pt1, pt0);
	VectorSub(&v1, pt2, pt1);
	// can v1 be written as a scalar multiple of v0?
	double scalar;
	if(v0.x != 0.f)
	{
		scalar = v1.x / v0.x;
		if(fabs(scalar * v0.y - v1.y) < EPS &&
			fabs(scalar * v0.z - v1.z) < EPS)
			return 1;
		else
			return 0;
	}
	if(v0.y != 0.f)
	{
		scalar = v1.y / v0.y;
		if(fabs(scalar * v0.x - v1.x) < EPS &&
			fabs(scalar * v0.z - v1.z) < EPS)
			return 1;
		else
			return 0;
	}
	if(v0.z != 0.f)
	{
		scalar = v1.z / v0.z;
		if(fabs(scalar * v0.x - v1.x) < EPS &&
			fabs(scalar * v0.y - v1.y) < EPS)
			return 1;
		else
			return 0;
	}
	// if v0 is the zero vector, then they are all co-linear.
	return 1;
}

int ArePointsCoplanar(Vector *pt0, Vector *pt1, Vector *pt2, Vector *pt3)
{
	if(ArePointsColinear(pt0, pt1, pt2))
		return 1;
	Plane plane;
	MakePlane(&plane, pt0, pt1, pt2);
	return(SideOfPlane(&plane, pt3) == ON_PLANE);
}

// endof Volume.cpp
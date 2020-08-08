// Polygon.cpp

#include "Polygon.h"
#include <assert.h>

// Compute the plane and associated with
// the given polygon as a function of its vertices.
void CalcPolygonPlane(Polygon *pgon)
{
	Vector pt, norm;
	VectorSet(&norm, 0.f, 0.f, 0.f);
	VectorSet(&pt, 0.f, 0.f, 0.f);
	int i, j;
	for(i = 0; i < pgon->num_vertices; i++)
	{
		// Use the Newel method.  It's does a good
		// job when not all vertices of the polygon
		// are coplanar.
		j = (i + 1) % pgon->num_vertices;
		norm.x += (pgon->vtx[i]->y - pgon->vtx[j]->y) * (pgon->vtx[i]->z + pgon->vtx[j]->z);
		norm.y += (pgon->vtx[i]->z - pgon->vtx[j]->z) * (pgon->vtx[i]->x + pgon->vtx[j]->x);
		norm.z += (pgon->vtx[i]->x - pgon->vtx[j]->x) * (pgon->vtx[i]->y + pgon->vtx[j]->y);
		VectorAdd(&pt, &pt, pgon->vtx[j]);
	}
	VectorScale(&pt, &pt, 1.f / GLfloat(pgon->num_vertices));
	MakePlane(&pgon->plane, &pt, &norm);
}

// Determine the intersection point of the given line
// segment with the given polygon, if any.  If there
// isn't one, zero is returned, otherwise one is.
// In either case, the given t value is messed with.
int LinePolygonIntersection(Polygon *pgon, Vector *pt0, Vector *pt1, GLfloat *t)
{
	*t = LinePlaneIntersection(&pgon->plane, pt0, pt1);
	if(*t <= 0.f || *t >= 1.f)
		return 0;
	
	Vector pt;
	VectorLerp(&pt, pt0, pt1, *t);

	Vector v0, v1, cross;
	int i, j;

	// A projection would be faster, but
	// for now, this will do.
	for(i = 0; i < pgon->num_vertices; i++)
	{
		j = (i + 1) % pgon->num_vertices;
		VectorSub(&v0, pgon->vtx[i], &pt);
		VectorSub(&v1, pgon->vtx[j], &pt);
		VectorCross(&cross, &v0, &v1);
		VectorNormalize(&cross, &cross);
		if(VectorDot(&cross, &pgon->plane.norm) < -EPS)
			return 0;
	}

	return 1;
}

// Return 1 if the given polygon is convex,
// otherwise return 0 if it's concave.
// It is assumed that the vertices of the given
// polygon are ordered CCW from the perspective
// of looking at the polygon from the front-side
// of the plane containing it.
int PolygonIsConvex(Polygon *pgon)	// This routine is untested.
{
	// Yeah, there has to be a faster way than this.
	Plane plane;
	Vector vec;
	int i, j, k;
	for(i = 0; i < pgon->num_vertices; i++)
	{
		j = (i + 1) % pgon->num_vertices;
		VectorSub(&vec, pgon->vtx[j], pgon->vtx[i]);
		VectorCross(&vec, &vec, &pgon->plane.norm);
		MakePlane(&plane, pgon->vtx[i], &vec);
		for(k = 0; k < pgon->num_vertices; k++)
		{
			if(k == i || k == j)
				continue;
			if(SideOfPlane(&plane, pgon->vtx[k]) == ON_PLANE_FRONT)
				return 0;
		}
	}

	return 1;
}

// If the given polygon strattles the given plane,
// then split it into two polygons.  Otherwise,
// leave the polygon alone.  It is assumed that the
// vertices of the given polygon are ordered.
int SplitPolygon(Plane *plane, Polygon *pgon, Polygon *pgon_front, Polygon *pgon_back, MemPool *vtx_mp)
{
	int side[MAX_POLYGON_VERTICES];
	int i, j;
	for(i = 0; i < pgon->num_vertices; i++)
		side[i] = SideOfPlane(plane, pgon->vtx[i]);

	// Determine if the polygon straddles the
	// plane or not.
	for(i = 1; i < pgon->num_vertices; i++)
		if(side[0] != side[i])
			break;
	if(i == pgon->num_vertices)
	{
		if(side[0] == ON_PLANE_FRONT)
			return PGON_ON_PLANE_FRONT;
		else if(side[0] == ON_PLANE_BACK)
			return PGON_ON_PLANE_BACK;
		return PGON_IN_PLANE;
	}

	// Split the polygon.
	pgon_front->num_vertices = 0;
	pgon_back->num_vertices = 0;
	Vector *vtx;
	GLfloat t;
	for(i = 0; i < pgon->num_vertices; i++)
	{
		j = (i + 1) % pgon->num_vertices;
		if(side[i] == ON_PLANE || side[i] == ON_PLANE_FRONT)
			pgon_front->vtx[pgon_front->num_vertices++] = pgon->vtx[i];
		if(side[i] == ON_PLANE || side[i] == ON_PLANE_BACK)
			pgon_back->vtx[pgon_back->num_vertices++] = pgon->vtx[i];
		if(side[i] == ON_PLANE || side[j] == ON_PLANE)
			continue;
		if(side[i] != side[j])
		{
			vtx = (Vector *)AllocBlock(vtx_mp);
			assert(vtx != 0);
			t = LinePlaneIntersection(plane, pgon->vtx[i], pgon->vtx[j]);
			assert(t > 0.f && t < 1.f);
			VectorLerp(vtx, pgon->vtx[i], pgon->vtx[j], t);
			pgon_front->vtx[pgon_front->num_vertices++] = vtx;
			pgon_back->vtx[pgon_back->num_vertices++] = vtx;			
		}
	}

	return PGON_STRADDLES_PLANE;
}

// Make use of the texture loader here too.
void LoadPolygonSoup(PolygonSoup *pgon_soup, char *filename)
{
}

void FreePolygonSoup(PolygonSoup *pgon_soup)
{
	// We can safely free all polygons
	// in the given list of polygons,
	// because that list will be empty
	// if it was stuffed in a BSP tree.
}

void RenderPolygonSoup(PolygonSoup *pgon_soup)
{
}

// add texture coordinate stuff here.
void RenderPolygon(Polygon *pgon, int flags)
{
	// bind texture here.
	glBegin(GL_POLYGON);
	if(!(flags & PGON_DRAW_SMOOTH) && (flags & PGON_DRAW_LIT))
		glNormal3fv(&pgon->plane.norm.x);
	int k;
	for(k = 0; k < pgon->num_vertices; k++)
	{
		if((flags & PGON_DRAW_SMOOTH) && (flags & PGON_DRAW_LIT))
			glNormal3fv(&pgon->norm[k]->x);
		glVertex3fv(&pgon->vtx[k]->x);
	}
	glEnd();
}

// endof Polygon.cpp
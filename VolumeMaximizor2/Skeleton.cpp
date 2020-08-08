// Skeleton.cpp

#define _CRT_TERMINATE_DEFINED
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <direct.h>
#include "Skeleton.h"
#include "ConvexHull.h"
#include "Main.h"

Bone bone_list[MAX_BONES];
int num_bones = 0;
Joint joint_list[MAX_JOINTS];
int num_joints = 0;

// parse a string having the format {x, y, z, w, ...}
// where each element is a real number.  return these
// numbers in the given vector.  return the number of
// elements in the returned vector.
static int ParseLine(char *line_buf, double *vector, int vec_len)
{
	int count = 0;
	const int tok_len = 64;
	char token[tok_len];

	char *tok = line_buf;
	int k;

	while(count < vec_len)
	{
		// advance the token pointer until we
		// reach a number.
		while(*tok != '.' && !isdigit(*tok) && *tok != '-')
		{
			if(*tok == '\0' || *tok == '}')
				return count;
			tok++;
		}
		
		// copy the number into the token buffer.
		k = 0;
		while((*tok == '.' || isdigit(*tok) || *tok == '-') && k < tok_len - 1)		
			token[k++] = *tok++;
		token[k] = '\0';

		// convert the token to a real number.
		vector[count++] = atof(token);		
	}

	return count;
}

int MakeSkeleton(char *skel_file)
{
	FILE *fp = fopen(skel_file, "r");
	if(!fp)
	{
		if(chdir("..") < 0)
		{
			fprintf(stderr, "Failed to open skeleton file %s!\n", skel_file);
			return 0;
		}

		// this is retarded since this routine allocate
		// alot of locals on the call stack.
		return MakeSkeleton(skel_file);
	}

	num_bones = 0;
	num_joints = 0;

	const int line_len = 1024;
	char line_buf[line_len];	
	const int vec_len = 16;
	double vector[vec_len];
	int count;
	int i, j;
	Bone *bone;
	Joint *jt;
	Vector *pt;
	while(fgets(line_buf, line_len, fp))
	{
		if(line_buf[0] != '{')
			continue;
		count = ParseLine(line_buf, vector, vec_len);
		switch(count)
		{
			case 2:	// process bone.
			{
				if(num_bones >= MAX_BONES)
				{
					fprintf(stderr, "Warning: bone max reached!\n");
					break;
				}
				i = int(vector[0]);
				j = int(vector[1]);
				if(i < 0 || i >= num_joints ||
					j < 0 || j >= num_joints)
				{
					fprintf(stderr, "Warning: ignoring bogus bone joints %d and %d.\n", i, j);
					break;
				}				
				bone = &bone_list[num_bones++];
				bone->jt[0] = &joint_list[i];
				bone->jt[1] = &joint_list[j];
				bone->length = VectorDistance(bone->jt[0]->vtx, bone->jt[1]->vtx);
				break;
			}
			case 3:	// process joint.
			{
				if(num_cloud_pts >= MAX_POINTS)
				{
					fprintf(stderr, "Warning: max cloud points reached!\n");
					break;
				}
				if(num_joints >= MAX_JOINTS)
				{
					fprintf(stderr, "Warning: max joints reached!\n");
					break;
				}
				// there is always a one-to-one relationship
				// between cloud points and joints, so this is
				// a bit silly, but...whatever...
				pt = &pt_cloud[num_cloud_pts++];
				jt = &joint_list[num_joints++];
				memset((void *)jt, 0, sizeof(Joint));
				VectorSet(pt, vector[0], vector[1], vector[2]);
				jt->vtx = pt;
				jt->lon_count = 10;
				jt->lat_count = 16;
				jt->sphere_radius = 0.5;
				break;
			}
			default:
			{
				fprintf(stderr, "Warning: ignoring line %s.\n", line_buf);
				break;
			}
		}
	}

	// by now the bones are connected to the joints, but
	// the joints are not connected to the bones.  do that.
	for(i = 0; i < num_bones; i++)
	{
		bone = &bone_list[i];
		jt = bone->jt[0];
		if(jt->num_bones < MAX_BONES_PER_JOINT)				
			jt->bone[jt->num_bones++] = bone;
		else
			fprintf(stderr, "Warning: max joints per bone reached!\n");
		jt = bone->jt[1];
		if(jt->num_bones < MAX_BONES_PER_JOINT)
			jt->bone[jt->num_bones++] = bone;
		else
			fprintf(stderr, "Warning: max joints per bone reached!\n");
	}

	printf("skeleton file %s loaded!\n", skel_file);
	printf("\tnum bones = %d\n", num_bones);
	printf("\tnum joints = %d\n", num_joints);
	fclose(fp);
	return 1;
}

void DrawSkeleton(void)
{
	Vector *vtx0, *vtx1;
	glBegin(GL_POINTS);
	
	int k;
	for(k = 0; k < num_joints; k++)
	{
		if(k == jt_idx)
			glColor3f(0.f, 1.f, 0.f);
		else
			glColor3f(1.f, 0.f, 0.f);
		vtx0 = joint_list[k].vtx;
		glVertex3f(vtx0->x, vtx0->y, vtx0->z);
	}
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.f, 0.f, 0.f);
	Bone *bone;	
	for(k = 0; k < num_bones; k++)
	{
		bone = &bone_list[k];
		vtx0 = bone->jt[0]->vtx;
		vtx1 = bone->jt[1]->vtx;
		glVertex3f(vtx0->x, vtx0->y, vtx0->z);
		glVertex3f(vtx1->x, vtx1->y, vtx1->z);
	}
	glEnd();
#if 0
	for(k = 0; k < num_joints; k++)
	{
		vtx0 = joint_list[k].vtx;
		glPushMatrix();
		glTranslatef(vtx0->x, vtx0->y, vtx0->z);
		glColor3f(1.f, 0.f, 1.f);
		glutWireSphere(joint_list[k].sphere_radius, 5, 5);
		glPopMatrix();
	}
#endif
}

static Joint *AdjacentJoint(Joint *jt, int k)
{
	if(k < 0 || k >= jt->num_bones)
		return 0;
	Bone *bone = jt->bone[k];
	Joint *adj_jt = (bone->jt[0] == jt) ? bone->jt[1] : bone->jt[0];
	return adj_jt;
}

// rewrite this BFS to be based on the bones, not on the joints!!

// btw, if we never had to adjust a joint, then
// there's no need to continue the BFS through the graph...fix that.
void MoveJoint(Joint *jt, Vector *delta)
{
	// begin with all joints non-fixed.
	int i, j;
	for(i = 0; i < num_joints; i++)
		joint_list[i].fixed = 0;

	// move the given vertex and fix its location.
	VectorAdd(jt->vtx, jt->vtx, delta);
	jt->fixed = 1;

	// let the moving-joint-list be initially empty.
	List MovingJointList;
	WipeList(&MovingJointList);

	// add to the moving-joint-list all adjacent
	// joints of the moved joint.
	Joint *adj_jt;
	for(i = 0; i < jt->num_bones; i++)
	{
		adj_jt = AdjacentJoint(jt, i);
		ListAppend(&MovingJointList, (ListItem *)adj_jt);
	}

	// until the list of joints that need to
	// be moved is empty...
	Vector vec, bone_vec;
	double len;
	while(MovingJointList.item_count > 0)
	{
		// grab a joint off the queue.
		jt = (Joint *)MovingJointList.head;
		ListRemove((ListItem *)jt);

		// move this joint in the direction
		// toward all of the adjacent joints
		// that are fixed until the joint satisfies
		// all of the bone-length constraints.
		static const int bail_out = 128;
		for(i = 0; i < bail_out; i++)
		{
			// compute the direction we need to move.
			VectorCopy(&vec, &Origin);			
			for(j = 0; j < jt->num_bones; j++)
			{
				adj_jt = AdjacentJoint(jt, j);
				if(!adj_jt->fixed)
					continue;
			
				// compute a vector that, if we moved
				// soley along, it would satisfy the
				// bone-length constraints with this bone
				// and its fixed adjacent joint.
				VectorSub(&bone_vec, adj_jt->vtx, jt->vtx);
				len = VectorLength(&bone_vec);
				VectorScale(&bone_vec, &bone_vec, (len - jt->bone[j]->length) / len);
				VectorAdd(&vec, &vec, &bone_vec);				
			}			

			// if the delta vector we need to
			// move by is within epsilon of the
			// zero vector, then we have approx-
			// imately satisfied all of the bone-
			// length constraints for fixed adj-
			// acent joints.
			if(VectorEq(&vec, &Origin))			
				break;

			// move in that direction.
			VectorAdd(jt->vtx, jt->vtx, &vec);			
		}

		// if we bailed out, then the above heuristic
		// won't work in this situation.  really, it's
		// a bug.  but i'm not sure how to fix it.
		// so instead, switch over to a different approach.
		if(i >= bail_out)
		{
			// hopefully we are not in a dead-lock
			// situation.  this is where the intersection
			// of spheres determined by the bone-lengths
			// to all fixed adjacent joints is empty.

			// for now, this algorithm cannot
			// handle graphs with cycles in them.
			assert(0);
		}

		// indicate that this joint is now fixed.
		jt->fixed = 1;

		// now that the joint has been moved
		// to a place that satisfies its fixed
		// adjacent joints, add any non-fixed
		// adjacent joints it has to the queue.
		for(i = 0; i < jt->num_bones; i++)
		{
			adj_jt = AdjacentJoint(jt, i);
			if(adj_jt->fixed)
				continue;
			ListAppend(&MovingJointList, (ListItem *)adj_jt);
		}
	}

	// perform a sanity check on the graph.
	// make sure that all of the bone length
	// constrainst are satisfied.
#if 0
	Bone *bone;
	double error;
	for(i = 0; i < num_bones; i++)
	{
		bone = &bone_list[i];
		len = VectorDistance(bone->jt[0]->vtx, bone->jt[1]->vtx);
		error = fabs(len - bone->length);
		if(error > 0.001) //EPS)		
			assert(0);		
	}
#endif
}

// we need a way to quickly save
// and restore the point cloud's
// current configuration.
static Vector CloudCopy[MAX_POINTS];
static inline void SaveCloud(void)
{
	memcpy((void *)CloudCopy, (const void *)pt_cloud, sizeof(Vector) * num_cloud_pts);
}
static inline void RestoreCloud(void)
{
	memcpy((void *)pt_cloud, (const void *)CloudCopy, sizeof(Vector) * num_cloud_pts);
}

const int MAX_SAMPLE_VECTORS = 256;
static Vector vec_sample[MAX_SAMPLE_VECTORS];
int num_sample_vecs = 0;

// generate a sphere of sampling vectors.
static void GenerateSampleVectors(int lat_count, int lon_count, double sphere_radius)
{	
	double lat_angle, lon_angle;
	double x, y, z, r;
	int i, j;
	num_sample_vecs = 0;
	for(i = 0; i <= lat_count; i++)
	{
		lat_angle = PI * double(i) / double(lat_count);
		z = sphere_radius * cos(lat_angle);
		r = sphere_radius * sin(lat_angle);
		for(j = 0; j < lon_count; j++)
		{
			lon_angle = 2.0 * PI * double(j) / double(lon_count);
			x = r * cos(lon_angle);
			y = r * sin(lon_angle);
			assert(num_sample_vecs < MAX_SAMPLE_VECTORS);
			VectorSet(&vec_sample[num_sample_vecs++], x, y, z);
			if(i == 0 || i == lat_count)
				break;
		}
	}
}

// one of the difficulties faced by this routine
// is that of trying to figure out what to do
// with joints that are housed intirely inside
// of the convex hull.  if this is the case, then
// it may or may not be a good idea to try and
// get it to touch the surface.  some joints need
// to be in the interior.  for now, interior joints
// are not dealt with in any special way.
void OptimizeJoint(Joint *jt)
{
	GenerateSampleVectors(jt->lat_count, jt->lon_count, jt->sphere_radius);
	SaveCloud();
	Find3DConvexHull();
	double vol, max_vol = Calc3DConvexHullVolume();
	int i, j = -1;	
	for(i = 0; i < num_sample_vecs; i++)
	{		
		MoveJoint(jt, &vec_sample[i]);
		Find3DConvexHull();
		vol = Calc3DConvexHullVolume();
		if(vol > max_vol)
		{
			max_vol = vol;
			j = i;
		}
		RestoreCloud();
	}
	if(j != -1)	
		MoveJoint(jt, &vec_sample[j]);	
	else
	{
		jt->sphere_radius *= 0.95;
//		if((jt->lat_count - 1) * (jt->lon_count + 2) + 2 < MAX_SAMPLE_VECTORS)
//			jt->lon_count += 2;
//		if(jt->lat_count * jt->lon_count + 2 < MAX_SAMPLE_VECTORS)
//			jt->lat_count += 1;		
	}	
}

// optimize all joints in the skeleton.
// if no joint moved, then return zero,
// otherwise return one.
void OptimizeSkeleton(void)
{
	int k;
	for(k = 0; k < num_joints; k++)
		OptimizeJoint(&joint_list[k]);	
}

// try to maximize the volume taken up
// by the 3d convex hull of the skeleton.
void MaximizeVolume(void)
{
	
}

// endof Skeleton.cpp
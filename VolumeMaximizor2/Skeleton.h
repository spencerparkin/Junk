// Skeleton.h

#ifndef __Skeleton_h__
#define __Skeleton_h__

#include "Vector.h"
#include "List.h"

#define MAX_BONES				32
#define MAX_JOINTS				32
#define MAX_BONES_PER_JOINT		32

struct Bone
{
	struct Joint *jt[2];
	double length;
};

extern Bone bone_list[MAX_BONES];
extern int num_bones;

struct Joint
{
	Joint *next, *prev;
	List *list;
	Bone *bone[MAX_BONES_PER_JOINT];
	int num_bones;
	Vector *vtx;
	int fixed;
	int lon_count, lat_count;
	double sphere_radius;
};

extern Joint joint_list[MAX_JOINTS];
extern int num_joints;

int MakeSkeleton(char *skel_file);
void DrawSkeleton(void);
void MoveJoint(Joint *jt, Vector *delta);
void OptimizeJoint(Joint *jt);
void OptimizeSkeleton(void);

#endif //__Skeleton_h__

// endof Skeleton.h
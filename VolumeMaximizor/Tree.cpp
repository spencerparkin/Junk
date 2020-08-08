// Tree.cpp

// This software was written by Spencer T. Parkin, Copyright (c) 2007, All Rights Reserved.

#include <GL/glut.h>
#include <math.h>
#include <assert.h>
#include "Tree.h"
#include "Volume.h"

// these will need to be refined as the
// tree gets closer to convergence.
int num_tweak_samples = 8;
double tweak_angle = PI / 32.0;

void DrawTree(Node *root)
{
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(root->vec.x, root->vec.y, root->vec.z);	
	glEnd();
	if(root->num_sub_nodes == 0)
		return;
	glPushMatrix();
	glTranslatef(root->vec.x, root->vec.y, root->vec.z);	
	int k;
	for(k = 0; k < root->num_sub_nodes; k++)	
		DrawTree(root->sub_node[k]);	
	glPopMatrix();
}

static Vector *CalcTreeCloudPoints(Node *root, Vector *pt, Vector *O)
{
//	double len = VectorLength(&root->vec);
//	if(fabs(len - 1.0) > EPS)
//		assert(0);

	VectorAdd(pt, O, &root->vec);
	O = pt++;
	int k;
	for(k = 0; k < root->num_sub_nodes; k++)	
		pt = CalcTreeCloudPoints(root->sub_node[k], pt, O);	
	return pt;
}

// warning: it is assumed here that the given buffer is big enough.
int CalcTreeCloudPoints(Node *root, Vector *pt_cloud_buf)
{
	Vector O = {0.0, 0.0, 0.0};
	VectorSet(&pt_cloud_buf[0], 0.0, 0.0, 0.0);
	Vector *pt = CalcTreeCloudPoints(root, &pt_cloud_buf[1], &O);
	return(pt - pt_cloud_buf);
}

double CalcTreeVolume(Node *root)
{
	static Vector pt_cloud_buf[MAX_CLOUD_POINTS];
	int num_pt_cloud_pts = CalcTreeCloudPoints(root, pt_cloud_buf);
	return CalcVolume(pt_cloud_buf, num_pt_cloud_pts);
}

// this is an extremely expensive routine.
int TweakTreeNode(Node *node, Node *root)
{
	// it never makes sense to tweak the root node.
	// this is because the shape doesn't change
	// when the vector is moved.  instead the entire
	// shape just moves with the vector.
	if(node == root)
		return 0;

	// i'm not sure how to insure that a tweak
	// will always result in improvement.  sometimes
	// a vector may be too far into the inside of
	// the hull.  the initial configuration of the
	// tree will be important.

	// calculate the original volume before we tweak anything.
	double orig_vol;
	orig_vol = CalcTreeVolume(root);

	// find an arbitrary vector that is ortho-
	// gonal to the axis vector.
	Vector axis, cross;
	VectorCopy(&axis, &node->vec);
	if(fabs(axis.x) > fabs(axis.y))
		VectorCross(&cross, &Yaxis, &axis);
	else
		VectorCross(&cross, &Xaxis, &axis);	

	// get the vector into rotating position by
	// rotating it on the vector orthogonal to the axis.
	VectorRotate(&node->vec, &node->vec, &cross, tweak_angle);

	// wind the vector around while computing
	// the volume for each tweak.  pick the
	// tweak that resulted in the biggest
	// volume increase.  In no tweak made an
	// increase, revert back to the original vector.
	double angle = 2.0 * PI / double(num_tweak_samples);
	double vol, max_vol = orig_vol;
	Vector best_tweak_vec; 
	int k;
	for(k = 0; k < num_tweak_samples; k++)
	{
		vol = CalcTreeVolume(root);
		if(vol > max_vol)
		{
			max_vol = vol;
			VectorCopy(&best_tweak_vec, &node->vec);
		}
		VectorRotate(&node->vec, &node->vec, &axis, angle);
	}

	// stick with the best results.
	int tweak_flag;
	if(max_vol > orig_vol)
	{
		VectorCopy(&node->vec, &best_tweak_vec);
		tweak_flag = 1;
	}
	else
	{
		VectorCopy(&node->vec, &axis);
		tweak_flag = 0;
	}

	// indicate if we tweaked or not.
	return tweak_flag;
}

// pass in the root of the tree twice.
int OptimizeTreeIteration(Node *node, Node *root)
{
	int tweak_flag = TweakTreeNode(node, root);
	int k;
	for(k = 0; k < node->num_sub_nodes; k++)
		tweak_flag |= OptimizeTreeIteration(node->sub_node[k], root);
	return tweak_flag;
}

void FreeTree(Node *root)
{
	int k;
	for(k = 0; k < root->num_sub_nodes; k++)
		FreeTree(root->sub_node[k]);
	delete root;
}

// endof Tree.cpp
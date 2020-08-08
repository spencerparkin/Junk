// Tree.h

// This software was written by Spencer T. Parkin, Copyright (c) 2007, All Rights Reserved.

#ifndef __Tree_h__
#define __Tree_h__

#include "Vector.h"

// i wouldn't know how to write the optimization
// algorithm for graphs with cycles in them, so
// we'll just do trees.

#define MAX_SUB_NODES	16
struct Node
{
	Vector vec;
	Node *sub_node[MAX_SUB_NODES];
	int num_sub_nodes;
};

// the tweak parameters.
extern int num_tweak_samples;
extern double tweak_angle;

void DrawTree(Node *root);
int CalcTreeCloudPoints(Node *root, Vector *pt_cloud_buf);
double CalcTreeVolume(Node *root);
int TweakTreeNode(Node *node, Node *root);
int OptimizeTreeIteration(Node *node, Node *root);
void FreeTree(Node *root);

#endif //__Tree_h__

// endof Tree.h
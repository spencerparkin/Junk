// AVL_tree.cpp - Written by Spencer T. Parkin

// This is my first AVL tree.  I wanted to
// make the code as neat and clean as I could
// and to document it as well as possible.
// Also, I wanted to impliment the most
// important tree operations as non-recursive.
// It should be possible to rip the tree
// code out of this demo and turn it into
// a stand-alone library.

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL_tree.h"

// A pointer to the user selected node in the tree.
AVL_node *SelectedNode = 0;

/*-----------------------------------------------*\
\*-----------------------------------------------*/
AVL_tree::AVL_tree()
{
	root = 0;
	node_count = 0;
}

/*-----------------------------------------------*\
\*-----------------------------------------------*/
AVL_tree::~AVL_tree()
{
}

/*-----------------------------------------------*\
\*-----------------------------------------------*/
AVL_node::AVL_node(int key)
{
	this->key = key;
	x = 0.f;
	y = 0.f;
	left = right = 0;
	parent = 0;
}

/*-----------------------------------------------*\
\*-----------------------------------------------*/
AVL_node::~AVL_node()
{
}

/*-----------------------------------------------*\
	Traverse this tree in-order and visit
	each node with the given callback.
\*-----------------------------------------------*/
int AVL_tree::Visit(int (* visit_CB)(class AVL_node *n))
{
	if(root)
		return root->Visit(visit_CB);

	return 0;
}

/*-----------------------------------------------*\
	Perform an in-order tree-walk.  If the given
	callback returns a non-zero value, terminate
	the tree-walk and return that code.
\*-----------------------------------------------*/
int AVL_node::Visit(int (* visit_CB)(AVL_node *n))
{
	int ret = 0;

	if(left)
		ret = left->Visit(visit_CB);

	if(!ret)
		ret = visit_CB(this);

	if(!ret && right)
		ret = right->Visit(visit_CB);

	return ret;
}

/*-----------------------------------------------*\
	Find the node with the given key in this
	binary tree.  Return it once found.  If it
	does not exist within this tree, return null.
\*-----------------------------------------------*/
AVL_node *AVL_tree::FindNode(int key)
{
#if 0		// There's no good reason for this routine to be recursive.
	if(root)
		return root->Find(key);
	
	return 0;
#else		// This is much faster.
	AVL_node *n = root;
	while(n && n->key != key)
	{
		if(key < n->key)
			n = n->left;
		else
			n = n->right;
	}
	return n;
#endif
}

/*-----------------------------------------------*\
	Traverse the binary tree until we find the
	node with the given key.  Return the node
	once found.  If not found, null is returned.
\*-----------------------------------------------*/
AVL_node *AVL_node::Find(int key)
{
	if(key == this->key)
		return this;

	if(key < this->key)
	{
		if(left)
			return left->Find(key);
	
		return 0;
	}
	else if(right)
		return right->Find(key);

	return 0;
}

/*-----------------------------------------------*\
	Move the given node to the root of the tree
	by doing the proper sequence of rotations.
\*-----------------------------------------------*/
void AVL_tree::MoveToRoot(AVL_node *n)
{
	while(!n->IsRoot())
		n->MoveToRoot(this);
}

/*-----------------------------------------------*\
	Rotate the given node up one generation.	
\*-----------------------------------------------*/
void AVL_node::MoveToRoot(AVL_tree *tree)
{
	if(parent)
	{
		if(parent->left == this)
			parent->RotateRight(tree);
		else
			parent->RotateLeft(tree);
	}
}

/*-----------------------------------------------*\
	Move the given node down the tree until
	it is a leaf.  This is a silly routine.
	I'm not sure if it has any use.
\*-----------------------------------------------*/
void AVL_tree::MoveToLeaf(AVL_node *n)
{
	while(!n->IsLeaf())
		n->MoveToLeaf(this);
}

/*-----------------------------------------------*\
	Rotate the given node down one generation.
	Try to keep the tree balanced when doing it.
\*-----------------------------------------------*/
void AVL_node::MoveToLeaf(AVL_tree *tree)
{
	if(left && right)
	{
		if(bal_factor < 0)
			RotateRight(tree);
		else
			RotateLeft(tree);
	}
	else if(left)
		RotateRight(tree);
	else if(right)
		RotateLeft(tree);
}

/*-----------------------------------------------*\
	This is not a splay tree, but I wanted to
	play with this operation anyway.  Splay the
	given node to the top of the tree.
\*-----------------------------------------------*/
void AVL_tree::Splay(AVL_node *n)
{
	while(!n->IsRoot())
		n->Splay(this);
}

/*-----------------------------------------------*\
	Splay this node up one or two generations.
	The local structure of the tree is analysed
	to see what type of splay is required.
\*-----------------------------------------------*/
void AVL_node::Splay(AVL_tree *tree)
{
	// If this is the root, we're done.
	if(!parent)
		return;

	// If the parent has no parent, (is the
	// root node), then simply rotate this
	// node to the top.  This is also known
	// as the "zig" case.
	if(!parent->parent)
	{
		printf("ZIG!!!\n");
		MoveToRoot(tree);
		return;
	}

	// There are 4 cases remaining: The two
	// ways this node is related to its parent
	// for the two cases this node's parent
	// is related to its parent.
	if(parent->left == this && parent->parent->right == parent ||
		parent->right == this && parent->parent->left == parent)
	{
		// This is called the "zig-zag" case.
		// Again, simply rotate this node up the tree.
		printf("ZIG-ZAG!!!\n");
		MoveToRoot(tree);
		MoveToRoot(tree);
		return;
	}

	// Finally, what we do in the "zig-zig" distinguishes
	// this operation from just rotate this node up the tree.
	// It is also counter-intuitive with respect to the goal
	// of a splay tree.  But it happens to be what makes the
	// splay tree theorems work.
	printf("ZIG-ZIG!!!\n");
	parent->MoveToRoot(tree);
	MoveToRoot(tree);
}

/*-----------------------------------------------*\
	Return 1 if this node is the root of its
	tree.  Return 0 otherwise.
\*-----------------------------------------------*/
int AVL_node::IsRoot(void)
{
	return(parent ? 0 : 1);
}

/*-----------------------------------------------*\
	Return 1 if this node is a leaf node.
	Return 0 otherwise.
\*-----------------------------------------------*/
int AVL_node::IsLeaf(void)
{
	return(!left && !right);
}

/*-----------------------------------------------*\
	This routine is the mirror image of the
	RotateRight() member function.  I might
	have combined the left and right rotation
	routines into one function using modulo
	two arithimatic, but that's a bit too silly.
  
   A < B < C < D               A < B < C < D

         B                              D
	   /   \                          /
      A     D         --->           B    
	       /                       /   \
		 C                        A     C

\*-----------------------------------------------*/
int AVL_node::RotateLeft(AVL_tree *tree)
{
	// If B has no right child D, then
	// we cannot right-rotate.
	if(!right)
		return 0;

	printf("Rotating node with key %d to the LEFT.\n", key);

	// Identify the new and old roots.
	AVL_node *new_root = right;
	AVL_node *old_root = this;

	// Reassign the new root of this subtree.
	new_root->parent = old_root->parent;
	if(old_root->parent)
	{
		if(old_root->parent->left == old_root)
			old_root->parent->left = new_root;
		else
			old_root->parent->right = new_root;
	}
	else
		tree->root = new_root;

	// If the new root had a left branch,
	// then detach it and make it the right
	// branch of the old root.
	if(new_root->left)
		new_root->left->parent = old_root;
	old_root->right = new_root->left;

	// Reassign the old root of this subtree.
	new_root->left = old_root;
	old_root->parent = new_root;

	// Notice that the balance factors of the
	// subtrees rooted at A and C do not change.
	// Therefore we can use them to recalculate
	// the balance factor at B, then recalculate
	// the balance factor at D.  The order here
	// is important.
//	old_root->UpdateBalanceFactor();
//	new_root->UpdateBalanceFactor();

	// We need to do more than that.
	AVL_node *n = old_root;
	while(n)
	{
		n->UpdateBalanceFactor();
		n = n->parent;
	}

	return 1;
}

/*-----------------------------------------------*\
	Rotate the subtree rooted at this node to
	the right.  The order of the tree is kept
	and the balance factors are updated.

   A < B < C < D               A < B < C < D

         C                        A
	   /   \                        \
	  A     D         --->           C
	   \                           /   \
	     B                        B     D

\*-----------------------------------------------*/
int AVL_node::RotateRight(AVL_tree *tree)
{
	// If C has no left child A, then
	// we cannot left-rotate.
	if(!left)
		return 0;

	printf("Rotating node with key %d to the RIGHT.\n", key);

	// Identify the new and old roots.
	AVL_node *new_root = left;
	AVL_node *old_root = this;

	// Reassign the new root of this subtree.
	new_root->parent = old_root->parent;
	if(old_root->parent)
	{
		if(old_root->parent->left == old_root)
			old_root->parent->left = new_root;
		else
			old_root->parent->right = new_root;
	}
	else
		tree->root = new_root;

	// If the new root had a right branch,
	// then detach it and make it the left
	// branch of the old root.
	if(new_root->right)
		new_root->right->parent = old_root;
	old_root->left = new_root->right;

	// Reassign the old root of this subtree.
	new_root->right = old_root;
	old_root->parent = new_root;

	// Notice that the balance factors of the
	// subtrees rooted at B and D do not change.
	// Therefore we can use them to recalculate
	// the balance factor at C, then recalculate
	// the balance factor at A.  The order here
	// is important.
//	old_root->UpdateBalanceFactor();
//	new_root->UpdateBalanceFactor();

	// We need to do more than that for
	// demonstration perposes.  But the
	// former code would have been fine
	// if the caller intended on continuing
	// up the tree and fixing the balance factors.
	AVL_node *n = old_root;
	while(n)
	{
		n->UpdateBalanceFactor();
		n = n->parent;
	}

	return 1;
}

/*-----------------------------------------------*\
	Insert the given node into the binary tree.
	It is assumed that the key field of the given
	node is already filled out.  The balance
    factors for each node on the path to where
	the given node is placed in the tree are
	updated.  Therefore, the resulting tree may
	no longer be an AVL tree.  The AVL conditions
	are resatisfied if the given flag is set.
	Return zero if a node with the same key
	already exists in the tree, one otherwise.
\*-----------------------------------------------*/
int AVL_tree::InsertNode(AVL_node *n, int AVL_flag)
{
	// A recursive implimentation of this routine
	// would find the new home for the node by
	// head recursion, then fix the tree by tail
	// recursion.  This implimentation is much
	// faster, though, because it's non-recursive.
	if(root)
	{
		AVL_node *k = root;
		for(;;)
		{
			// If a node in the tree already
			// exists with the given node's
			// key, then barf on it.
			if(n->key == k->key)
				return 0;

			// Traverse the tree and assign the parent's
			// branch pointer when we figure that out.
			if(n->key < k->key)
			{
				if(k->left)
					k = k->left;
				else
				{
					k->left = n;
					break;
				}
			}
			else
			{
				if(k->right)
					k = k->right;
				else
				{
					k->right = n;
					break;
				}
			}
		}

		// Patch in the new node.
		n->parent = k;
		n->left = n->right = 0;
//		n->depth = k->depth + 1;	// No need to track this.
		n->max_depth = 1;
		n->bal_factor = 0;

		// Now fix the tree.
		while(k)
		{
			k->UpdateBalanceFactor();

			if(AVL_flag)
				k->BalanceSubtree(this);

			k = k->parent;
		}
	}
	else
	{
		// If the tree was empty, the given
		// node just becomes the new root.
		root = n;
//		n->depth = 1;			// No need to track this.
		n->max_depth = 1;
		n->bal_factor = 0;
		n->parent = 0;
		n->left = n->right = 0;
	}

	// Keep track of how many nodes there
	// are in the tree.
	node_count++;
	return 1;
}

/*-----------------------------------------------*\
	Assuming that the subtrees rooted at the
	left and right are AVL trees, perform a
	single or double tree rotation at this
	node if needed so that the subtree rooted
	at this node is also an AVL tree.  The given
	tree should be the one containing this node,
	and is needed in case its root changes.
\*-----------------------------------------------*/
void AVL_node::BalanceSubtree(AVL_tree *tree)
{
	if(bal_factor > 1)
	{
		if(right && right->bal_factor < 0)
			right->RotateRight(tree);

		RotateLeft(tree);
	}
	else if(bal_factor < -1)
	{
		if(left && left->bal_factor > 0)
			left->RotateLeft(tree);

		RotateRight(tree);
	}
}

/*-----------------------------------------------*\
	Assuming that the subtrees rooted at the
	left and right of this node are updated,
	update the balance factor of this node.
\*-----------------------------------------------*/
void AVL_node::UpdateBalanceFactor(void)
{
	if(left && right)
	{
		if(left->max_depth > right->max_depth)
			max_depth = left->max_depth + 1;
		else
			max_depth = right->max_depth + 1;
	
		bal_factor = right->max_depth - left->max_depth;
	}
	else if(left)
	{
		max_depth = left->max_depth + 1;
		bal_factor = -left->max_depth;
	}
	else if(right)
	{
		max_depth = right->max_depth + 1;
		bal_factor = right->max_depth;
	}
	else
	{
		max_depth = 1;
		bal_factor = 0;
	}
}

/*-----------------------------------------------*\
	Find and return the node with the greatest
	key less than this node's key.  Return null
	if no such node exists.
\*-----------------------------------------------*/
AVL_node *AVL_node::Predecessor(void)
{
	AVL_node *n;

	if(!left)
	{
		AVL_node *k;
		n = this;
		do
		{
			k = n;
			n = n->parent;
		}
		while(n && n->right != k);
	}
	else
	{
		n = left;
		while(n->right)
			n = n->right;
	}

	return n;
}

/*-----------------------------------------------*\
	Find and return the node with the smallest
	key greater than this node's key.  Return
	null if no such node exists.
\*-----------------------------------------------*/
AVL_node *AVL_node::Successor(void)
{
	AVL_node *n;

	if(!right)
	{
		AVL_node *k;
		n = this;
		do
		{
			k = n;
			n = n->parent;
		}
		while(n && n->left != k);
	}
	else
	{
		n = right;
		while(n->left)
			n = n->left;
	}

	return n;
}

/*-----------------------------------------------*\
	Remove the given node from this tree.
	The node is not deallocated.  It was up
	to the caller to allocate it in the
	first place.  After this call, the given
	node is no longer a part of the tree structure.
\*-----------------------------------------------*/
void AVL_tree::RemoveNode(AVL_node *n, int AVL_flag)
{
	if(n->left && n->right)
	{
		// It doesn't matter which we use.
		// Perhaps it's a good idea to uniformly
		// pick either one.
		AVL_node *k;
		if(n->key & 1)
			k = n->Predecessor();
		else
			k = n->Successor();

		// Temporarily remove the predecessor
		// or successor node.
		(void)RemoveNode(k, AVL_flag);

		// Now let it take the place of the
		// node we actually want to delete.
		n->Hijack(k, this, 1);
		k->UpdateBalanceFactor();
	}
	else
	{
		// Note that in this case, (the given node having
		// zero or one subtrees), is not only an easy case
		// to deal with in removing the node and maintaining
		// the order of the tree, but also in maintaining
		// the AVL conditions.  There are other ways we might
		// remove a node from the tree, but in general, this
		// is the best, I think.
	
		// There may be one or no children of this node.
		AVL_node *k = (n->left ? n->left : n->right);

		// Have the only child, whether there is one or
		// not, take its parent's place.
		n->Hijack(k, this, 0);

		// Notice that the new children of the parent
		// of the deleted node have updated and valid balance factors.
		// We need now only update the balance factors
		// of their ancestors up to the root node, and
		// fix any invalid balance factors along the way.
		for(k = n->parent; k; k = k->parent)
		{
			k->UpdateBalanceFactor();

			if(AVL_flag)
				k->BalanceSubtree(this);
		}

		// Keep track of how many nodes are in the tree.
		node_count--;
	}

	// To reinforce the fact that the given node
	// is no longer part of the binary tree data
	// structure, nullify its branch pointers.
	n->parent = 0;
	n->left = n->right = 0;
}

/*-----------------------------------------------*\
	Detach this node from the tree and attach
	the given node in its place.  This routine
	does not care about the order of the tree.
	The adoption flag gives the caller the option
	to have the given node adopt this node's
	children.  However, the given node may already
	have children of its own.  Only the caller
	knows the parental situation and needs to
	set the flag accordingly.  The branch pointers
	of this node remain untouched.
\*-----------------------------------------------*/
void AVL_node::Hijack(AVL_node *n, AVL_tree *tree, int adopt)
{
	if(n == this)
		return;

	if(n)
	{
		if(adopt)
		{
			n->left = left;
			n->right = right;

			if(left)
				left->parent = n;
			if(right)
				right->parent = n;
		}

		n->parent = parent;
	}

	if(parent)
	{
		if(parent->left == this)
			parent->left = n;
		else
			parent->right = n;
	}
	else
		tree->root = n;
}

/*-----------------------------------------------*\
	Remove all the nodes from this tree.
	In the callback provided, the caller
	should free the satallite data if needed,
	and then free the node.
\*-----------------------------------------------*/
void AVL_tree::Kill(void (* kill_CB)(AVL_node *n))
{
	AVL_node *n;
	while(root)
	{
		// Save a pointer to the old root node
		// and remove it from the tree.
		n = root;
		RemoveNode(root, 1);

		// Give the caller a chance to
		// free the node and the satallite data.
		kill_CB(n);
	}
}

/*-----------------------------------------------*\
	This is a silly routine that turns the
	tree into a link-list using the left branches.
\*-----------------------------------------------*/
void AVL_tree::LopLeft(void)
{
	AVL_node *n = root;

	while(n)
	{
		while(n->right)
		{
			n->RotateLeft(this);
			n = n->parent;
		}

		n = n->left;
	}
}

/*-----------------------------------------------*\
	This is a silly routine that turns the
	tree into a link-list using the right branches.
\*-----------------------------------------------*/
void AVL_tree::LopRight(void)
{
	AVL_node *n = root;

	while(n)
	{
		while(n->left)
		{
			n->RotateRight(this);
			n = n->parent;
		}

		n = n->right;
	}
}

/*-----------------------------------------------*\
	Return the node in the tree with the
	smallest key.
\*-----------------------------------------------*/
AVL_node *AVL_tree::Minimum(void)
{
	if(!root)
		return 0;

	AVL_node *n = root;

	while(n->left)
		n = n->left;

	return n;
}

/*-----------------------------------------------*\
	Return the node in the tree with the
	largest key.
\*-----------------------------------------------*/
AVL_node *AVL_tree::Maximum(void)
{
	if(!root)
		return 0;

	AVL_node *n = root;

	while(n->right)
		n = n->right;

	return n;
}

/*-----------------------------------------------*\
	No matter what the current state of the
	tree, (balanced or completely unbalanced),
	restore this tree to an AVL tree.  I'm sure
	there has to be a better and faster way.
\*-----------------------------------------------*/
void AVL_tree::RestoreAVL(void)		// NEEDS REWRITE
{
	AVL_node *n, *x;
	int offenders;

	do
	{
		offenders = 0;
		x = 0;
		n = Minimum();

		// Scan the tree left to right until
		// we find the offender at the greatest
		// depth in the tree.  We want to make
		// sure that the subtrees rooted at the
		// left and right of the offending node
		// are still AVL trees.
		while(n)
		{
			if(n->bal_factor < -1 || n->bal_factor > 1)
			{
				offenders++;

				// Since I didn't bother to keep track of
				// the depth of each node, we might as well
				// break out of this loop once we find an offender.
//				if(!x || n->depth > x->depth)
					x = n;

				break;
			}

			n = n->Successor();
		}

		// This is no longer counted.
//		printf("%d offenders detected.\n", offenders);

		// Balance the subtree rooted
		// at x and all its ancestors
		// up to the root.
		while(x)
		{
			x->BalanceSubtree(this);
			x = x->parent;
		}
	}
	while(offenders > 0);
}

/*-----------------------------------------------*\
	Return 1 if this tree is a valid AVL tree.
\*-----------------------------------------------*/
int AVL_tree::IsAVLTree(void)
{
	if(root)
		return root->IsAVLTree();

	return 1;
}

/*-----------------------------------------------*\
	Return 1 if the subtree rooted at this node
	is an AVL tree.
\*-----------------------------------------------*/
int AVL_node::IsAVLTree(void)
{
	return(bal_factor >= -1 && bal_factor <= 1) &&
			(!left || left->IsAVLTree()) &&
			(!right || right->IsAVLTree());
}

/*-----------------------------------------------*\
	Render the given null-terminated string
	in the given font at the origin.
\*-----------------------------------------------*/
static void RenderString(char *string, void *font, int center)
{
	// First, find out how wide the string is.
	GLfloat string_width = 0.f;
	int k;
	if(center)
		for(k = 0; string[k] != '\0'; k++)
			string_width += glutStrokeWidth(font, string[k]);

	// Assuming we're in modelview matrix mode,
	// get ready to render the string centered
	// at the origin.
	glPushMatrix();
	glScalef(0.002f, 0.002f, 0.002f);
	if(center)
		glTranslatef(-string_width / 2.f, 0.f, 1.f);
	glColor3f(0.f, 0.f, 0.f);
	for(k = 0; string[k] != '\0'; k++)
		glutStrokeCharacter(font, string[k]);
	glPopMatrix();
}

/*-----------------------------------------------*\
	Render the binary tree in the given mode.
	This routine assumes that it will render
	the entire OpenGL scene.  The given mouse
	coordinates are only used if we're asked
	to draw in GL_SELECT mode.
\*-----------------------------------------------*/
void AVL_tree::Render(GLenum mode, int mouse_x, int mouse_y)	// NEEDS FIXING
{
	// Find the maximum number of leaf nodes that
	// could exist in a binary tree with the current
	// maximum depth of this tree.
	int pow_of_2 = 1;
	if(root)
	{
		int k;
		// After a depth of 4, stop adjusting.
		for(k = 0; /*k < root->max_depth &&*/ k < 4; k++)
			pow_of_2 *= 2;
	}

	// Calculate the dimensions of our window.
	GLfloat x_min, x_max;
	GLfloat y_min, y_max;
	x_min = -GLfloat(pow_of_2) / 2.f;
	x_max = -x_min;
	y_min = -(root ? GLfloat(root->max_depth + 1) : 1.f);
	y_max = 1.f;

	// Make the window square.
	if(x_max > y_max)
		y_max = x_max;
	else
		x_max = y_max;
	if(x_min < y_min)
		y_min = x_min;
	else
		x_min = y_min;
	if(-x_min > x_max)
		x_max = -x_min;
	else
		x_min = -x_max;
	if(-y_min > y_max)
		y_max = -y_min;
	else
		y_min = -y_max;

//	printf(" %f,%f   %f,%f  \n", x_min, x_max, y_min, y_max);

	// Setup the projection and modelview matrices.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(mode == GL_SELECT)
	{
		GLint Viewport[4];
		glGetIntegerv(GL_VIEWPORT, Viewport);
		gluPickMatrix(GLdouble(mouse_x), GLdouble(Viewport[3] - mouse_y), 3, 3, Viewport);
	}
	extern GLfloat aspect_ratio;
	if(aspect_ratio > 1.f)
		glOrtho(x_min * aspect_ratio, x_max * aspect_ratio, y_min, y_max, -10.f, 100.f);
	else
		glOrtho(x_min, x_max, y_min / aspect_ratio, y_max / aspect_ratio, -10.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Finally, go render the tree.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(root)
		root->Render(mode, 0.f, 0.f, (x_max - x_min) / 4.f);
	else
	{
		// If the tree is empty, display the key map.
		glTranslatef(-0.9, 0.8f, 0.f);
		glScalef(0.15f, 0.15f, 0.15f);
		RenderString("I -- Insert node with random key in [0,999].", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.4f, 0.f);
		RenderString("R -- Remove the selected node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.4f, 0.f);
		RenderString("X -- Remove the root node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("< -- Rotate the selected node left.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("> -- Rotate the selected node right.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("P -- Go to the predecessor of the selected node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("S -- Go to the successor of the selected node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("U -- Go to the parent of the selected node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("[ -- Go to the left child of the selected node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("] -- Go to the right child of the selected node.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("F1 -- Display keys using in-order tree-walk.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("F2 -- Toggle tree fix mode.  (Default is ON.)", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("F12 -- Kill the entire tree.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("Use the mouse to click on a node to select it.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("Programmed by Spencer T. Parkin.", GLUT_STROKE_ROMAN, 0);
		glTranslatef(0.f, -0.5f, 0.f);
		RenderString("Copyright (c) 2006, All Rights Reserved.", GLUT_STROKE_ROMAN, 0);
	}

	// Do double buffering.
	if(mode == GL_RENDER)
		glutSwapBuffers();
	else if(mode == GL_SELECT)
		glFlush();
}

/*-----------------------------------------------*\
	Render the given node at the given location
	in the given mode.
\*-----------------------------------------------*/
void AVL_node::Render(GLenum mode, GLfloat x, GLfloat y, GLfloat fraction)
{
	// LERP this node to where it should
	// be drawn.  But draw it where it is.
	// This way we get a smooth animation.
	static const GLfloat lerp_percentage = 0.1f;
	this->x = this->x + (x - this->x) * lerp_percentage;
	this->y = this->y + (y - this->y) * lerp_percentage;

	// Finish drawing a line to
	// denote the branch from the
	// parent to this child node.
	if(parent && mode == GL_RENDER)
	{
		glVertex3f(this->x, this->y, -1.f);
		glEnd();
	}

	glPushMatrix();
	glTranslatef(this->x, this->y, 0.f);

	// If we're drawing in GL_SELECT mode,
	// then use this node's key to name the
	// primitives that represent it on screen.
	if(mode == GL_SELECT)
		glPushName(key);

	// Color nodes red that violate
	// the AVL tree condition.
	if(bal_factor < -1 || bal_factor > 1)
		glColor3f(1.f, 0.f, 0.f);
	else
		glColor3f(0.f, 1.f, 0.f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.f, 0.f);
	int k;
	const int num_triangles = 20;
	GLfloat angle, radius = 0.5f;
	for(k = 0; k <= num_triangles; k++)
	{
		angle = (3.1415926536f * 2.f) * GLfloat(k) / GLfloat(num_triangles);
		glVertex2f(radius * cos(angle),
					radius * sin(angle));
	}
	glEnd();

	if(mode == GL_RENDER)
	{
		// Label the node by its key and
		// its balance factor.
//		glPushMatrix();
//		if(parent && parent->right == this)
//			glTranslatef(0.f, -0.15f, 0.f);
//		else
//			glTranslatef(0.f, 0.15f, 0.f);
		char string[128];
//		sprintf(string, "k(%d),md(%d),bf(%d)", key, max_depth, bal_factor);
		sprintf(string, "%d(%d)", key, bal_factor);
		RenderString(string, GLUT_STROKE_ROMAN, 1);
//		glPopMatrix();
	
		// Draw node highlight here if
		// this node is selected by the user.
		if(this == SelectedNode)
		{
			glBegin(GL_LINE_LOOP);
			glColor3f(0.f, 0.f, 0.f);
			for(k = 0; k < num_triangles; k++)
			{
				angle = (3.1415926536f * 2.f) * GLfloat(k) / GLfloat(num_triangles);
				glVertex2f((radius + 0.1f) * cos(angle),
							(radius + 0.1f) * sin(angle));
			}
			glEnd();
		}
	}

	glPopMatrix();

	// Draw the subtrees rooted at the
	// left and right branches if they exist.

	if(left)
	{
		if(mode == GL_RENDER)
		{
			glBegin(GL_LINES);
			glColor3f(0.8f, 0.8f, 0.8f);
			glVertex3f(this->x, this->y, -1.f);
		}

		left->Render(mode, x - fraction, y - 1.f, fraction / 2.f);
	}

	if(right)
	{
		if(mode == GL_RENDER)
		{
			glBegin(GL_LINES);
			glColor3f(0.8f, 0.8f, 0.8f);
			glVertex3f(this->x, this->y, -1.f);
		}

		right->Render(mode, x + fraction, y - 1.f, fraction / 2.f);
	}
}

/*-----------------------------------------------*\
	Render this tree in GL_SELECT mode and
	determine what node, if any, is at the
	given mouse coordinates and return it.
\*-----------------------------------------------*/
AVL_node *AVL_tree::Select(int mouse_x, int mouse_y)
{
	GLuint SelectBuffer[512];
	GLint SelectHits;

	glSelectBuffer(512, SelectBuffer);
	(void)glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(-1);

	Render(GL_SELECT, mouse_x, mouse_y);

	SelectHits = glRenderMode(GL_RENDER);

	int k;
	GLuint *HitData = SelectBuffer;
	GLuint NumNames;
	int key;

	for(k = 0; k < SelectHits; k++)
	{
		NumNames = *HitData++;

		HitData += NumNames + 1;

		key = *HitData++;
		printf("hit: key = %d\n", key);
	}

	return FindNode(key);
}

// endof AVL_tree.cpp
// AVL_tree.h -- Spencer T. Parkin

#ifndef __AVL_tree_h__
#define __AVL_tree_h__

class AVL_node;

class AVL_tree
{
	public:

	AVL_tree();
	~AVL_tree();

	// Binary tree operation prototypes:
	AVL_node *FindNode(int key);
	int InsertNode(AVL_node *n, int AVL_flag);
	void RemoveNode(AVL_node *n, int AVL_flag);
	int Visit(int (* visit_CB)(AVL_node *n));
	void Kill(void (* kill_CB)(AVL_node *n));
	void Splay(AVL_node *n);
	void MoveToRoot(AVL_node *n);
	void MoveToLeaf(AVL_node *n);
	void LopLeft(void);
	void LopRight(void);
	AVL_node *Minimum(void);
	AVL_node *Maximum(void);
	void RestoreAVL(void);
	int IsAVLTree(void);

	// OpenGL operation prototypes:
	void Render(GLenum mode, int mouse_x, int mouse_y);
	AVL_node *Select(int mouse_x, int mouse_y);

	// Tree contents:
	AVL_node *root;
	int node_count;
};

class AVL_node
{
	public:

	AVL_node(int key);
	~AVL_node();

	// Node operation prototypes:
	int RotateLeft(AVL_tree *tree);
	int RotateRight(AVL_tree *tree);
	void UpdateBalanceFactor(void);
	void BalanceSubtree(AVL_tree *tree);
	AVL_node *Predecessor(void);
	AVL_node *Successor(void);
	int IsRoot(void);
	int IsLeaf(void);
	int IsAVLTree(void);
	void Hijack(AVL_node *n, AVL_tree *tree, int adopt);

	// Binary tree co-operation prototypes:
	AVL_node *Find(int key);
	int Visit(int (* visit_CB)(AVL_node *n));
	void Splay(AVL_tree *tree);
	void MoveToRoot(AVL_tree *tree);
	void MoveToLeaf(AVL_tree *tree);

	// OpenGL co-operation prototypes:
	void Render(GLenum mode, GLfloat x, GLfloat y, GLfloat fraction);

	// Node contents:
	AVL_node *left, *right;
	AVL_node *parent;
	int bal_factor, key;
//	int depth;				// Depth of this node in the binary tree.
	int max_depth;			// Maximum depth of subtree rooted at this node.
	GLfloat x, y;			// 2D coordinates for drawing the tree.
	void *sat_data;			// Pointer to satellite data.
};

extern AVL_node *SelectedNode;

#endif __AVL_tree_h__

// endof AVL_tree.h
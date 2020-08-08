// main.cpp -- Written by Spencer T. Parkin

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include "AVL_tree.h"

// GLUT callback prototypes:
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void visibility(int state);

// An instance of the AVL tree.
AVL_tree tree;

// Flag to determine whether or not we
// automatically maintain the AVL tree conditions.
int fix_tree = 1;

// AVL tree demo...
int main(int argc, char **argv)
{
	// Seed the random number generator.
	srand((unsigned)time(NULL));

	// Initialize GLUT.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	// Setup OpenGL state machine.
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);
	glutVisibilityFunc(visibility);

	// Enter the message loop.
	glutMainLoop();

	return 0;
}

// Render the AVL tree so that we can
// all see what's going on with it.
void display(void)
{
	tree.Render(GL_RENDER, 0, 0);
}

// Readjust the viewport.
GLfloat aspect_ratio = 1.f;
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	if(h != 0)
		aspect_ratio = GLfloat(w) / GLfloat(h);
	else
		aspect_ratio = 0.f;
}

// Handle keyboard input.
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'i':
		case 'I':
		{
			AVL_node *n = new AVL_node(rand() % 1000);
//			static int k = 0;
//			AVL_node *n = new AVL_node(k++);
			if(!tree.InsertNode(n, fix_tree))
			{
				fprintf(stderr, "Failed to insert new node with "
								"key %d into the tree.\n", n->key);
				delete n;
			}
			else
				printf("Max tree depth = %d\n", tree.root->max_depth);

			break;
		}
		case 'r':
		case 'R':
		{
			if(SelectedNode)
			{
				tree.RemoveNode(SelectedNode, fix_tree);
				printf("Deleted node with key %d.\n", SelectedNode->key);
				delete SelectedNode;
				SelectedNode = 0;
			}
			break;
		}
		case 'x':
		case 'X':
		{
			if(tree.root)
			{
				AVL_node *n = tree.root;
				tree.RemoveNode(n, fix_tree);
				delete n;
			}
			break;
		}
		case ',':
		case '<':
		{
			// Rotate the selected node, if any, to the left.
			if(SelectedNode)
			{
				if(SelectedNode->RotateLeft(&tree))
					printf("Rotated node with key %d to the left.\n", SelectedNode->key);
				else
					printf("Failed to rotate node with key %d to the left.\n", SelectedNode->key);
			}
			break;
		}
		case '.':
		case '>':
		{
			// Rotate the selected node, if any, to the right.
			if(SelectedNode)
			{
				if(SelectedNode->RotateRight(&tree))
					printf("Rotated node with key %d to the right.\n", SelectedNode->key);
				else
					printf("Failed to rotate node with key %d to the right.\n", SelectedNode->key);
			}
			break;
		}
		case 'p':
		case 'P':
		{
			if(SelectedNode)
			{
				AVL_node *n = SelectedNode->Predecessor();
				if(n)
				{
					printf("The predecessor to the node with key %d is the node with key %d.\n", SelectedNode->key, n->key);
					SelectedNode = n;
				}
				else
					printf("The node with key %d has no predecessor.\n", SelectedNode->key);
			}
			break;
		}
		case 's':
		case 'S':
		{
			if(SelectedNode)
			{
				AVL_node *n = SelectedNode->Successor();
				if(n)
				{
					printf("The successor to the node with key %d is the node with key %d.\n", SelectedNode->key, n->key);
					SelectedNode = n;
				}
				else
					printf("The node with key %d has no successor.\n", SelectedNode->key);
			}
			break;
		}
		case '[':
		case '{':
		{
			if(SelectedNode && SelectedNode->left)
				SelectedNode = SelectedNode->left;
			break;
		}
		case ']':
		case '}':
		{
			if(SelectedNode && SelectedNode->right)
				SelectedNode = SelectedNode->right;
			break;
		}
		case 'u':
		case 'U':
		{
			if(SelectedNode && SelectedNode->parent)
				SelectedNode = SelectedNode->parent;
			break;
		}
	}
}

// Used to free a node.
static void kill_CB(AVL_node *n)
{
	delete n;
}

// Print all the keys in order.
// This callback is called in an
// in-order tree walk.
static int visit_CB(AVL_node *n)
{
	printf("%d", n->key);

	AVL_node *s = n->Successor();
	if(!s)
		printf("\n");
	else
		printf(", ");

	// Stay alive.
	return 0;
}

// Handle keyboard input of "special" keys.
void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_F1:
		{
			tree.Visit(visit_CB);
			break;
		}
		case GLUT_KEY_F2:
		{
			fix_tree = 1 - fix_tree;
			if(fix_tree)
				printf("tree fix mode on!\n");
			else
				printf("tree fix mode off!\n");
			break;
		}
		case GLUT_KEY_F3:
		{
			if(SelectedNode)
				SelectedNode->MoveToLeaf(&tree);
			break;
		}
		case GLUT_KEY_F4:
		{
			if(SelectedNode)
				tree.MoveToLeaf(SelectedNode);
			break;
		}
		case GLUT_KEY_F5:
		{
			if(SelectedNode)
				SelectedNode->Splay(&tree);
			break;
		}
		case GLUT_KEY_F6:
		{
			if(SelectedNode)
				tree.Splay(SelectedNode);
			break;
		}
		case GLUT_KEY_F8:
		{
			if(tree.IsAVLTree())
				printf("The tree is VALID.\n");
			else
				printf("The tree is INVALID.\n");
			break;
		}
		case GLUT_KEY_F9:
		{
			int k;
			for(k = 0; k < 30; k++)
				keyboard('I', 0, 0);
			break;
		}
		case GLUT_KEY_F10:
		{
			printf("The tree has %d nodes.\n", tree.node_count);
			break;
		}
		case GLUT_KEY_F12:
		{
			tree.Kill(kill_CB);
			break;
		}
		case GLUT_KEY_PAGE_UP:
		{
			tree.LopLeft();
			break;
		}
		case GLUT_KEY_PAGE_DOWN:
		{
			tree.LopRight();
			break;
		}
		case GLUT_KEY_HOME:
		{
			tree.RestoreAVL();
			break;
		}
	}
}

// Handle mouse input.
void mouse(int button, int state, int x, int y)
{
	switch(button)
	{
		case GLUT_LEFT_BUTTON:
		{
			if(state == GLUT_DOWN)
				SelectedNode = tree.Select(x, y);
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			break;
		}
		default:
		{
			break;
		}
	}
}

// Draw all the time.  If our window,
// however, is not in the foreground,
// then we may not want to do this.
void idle(void)
{
	glutPostRedisplay();
}

// Try to be a nice program that
// plays well with others.
void visibility(int state)
{
	if(state == GLUT_NOT_VISIBLE)
		glutIdleFunc(0);
	else if(state == GLUT_VISIBLE)
		glutIdleFunc(idle);
}

// endof main.cpp
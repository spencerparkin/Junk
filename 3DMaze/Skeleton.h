// Skeleton.h

#ifndef __Skeleton_h__
#define __Skeleton_h__

#include "vec3d.h"
#include "Field.h"

class Maze
{
	public:

	Maze();
	~Maze();

	virtual void Render(int Tube) = 0;
	virtual void Generate(void) = 0;
	virtual void PatchNavigationLinks(void) = 0;
	virtual void Navigate(void) = 0;
};

class CorridorMaze : public Maze
{
	public:

	CorridorMaze();
	~CorridorMaze();

	void Render(int Tube);
	void Generate(void);
	void PatchNavigationLinks(void);
	void Navigate(void);

#define X_NODES		10
#define Y_NODES		10
	Node Matrix[X_NODES][Y_NODES];

#define BOX_WIDTH	10.f
#define BOX_LENGTH	10.f
#define BOX_DEPTH	10.f
};

class HollowSphereMaze : public Maze
{
	public:

	HollowSphereMaze(GLfloat Radius, int Slices, int Stacks);
	~HollowSphereMaze();

//	Node *
	GLfloat Radius;
	int Slices, Stacks;
};

class HollowCylinderMaze : public Maze
{
	public:

	HollowCylinderMaze(GLfloat Length, int Slices, int Stacks);
	~HollowCylinderMaze();

	Node ***Matrix;
	GLfloat Length;
	int Slices, Stacks;
};

class HollowTorusMaze : public Maze
{
	public:

	HollowTorusMaze(GLfloat RingRadius, GLfloat TubeRadius, int RingDensity, int TubeDensity);
	~HollowTorusMaze();

	void Render(int Tube);
	void Generate(void);
	void PatchNavigationLinks(void);
	void Navigate(void);

	Node ***Matrix;
	GLfloat RingRadius, TubeRadius;
	int RingDensity, TubeDensity;
};

class SolidCubeMaze : public Maze
{
	public:

	SolidCubeMaze(int Width, int Height, int Depth);
	~SolidCubeMaze();

	void Render(int Tube);
	void Generate(void);
	void PatchNavigationLinks(void);
	void Navigate(void);

	Node ****Matrix;
	int Width, Height, Depth;
};

class HexigonNode : public Node
{
	public:

	HexigonNode();
	~HexigonNode();

	void CalculateCoordinates(GLfloat X, GLfloat Y, GLfloat Z);

	int Calc;
};

class HexigonMaze : public Maze
{
	public:

	HexigonMaze(int Radius);
	~HexigonMaze();

	void Render(int Tube);
	void Generate(void);
	void PatchNavigationLinks(void);
	void Navigate(void);

	HexigonNode ***Matrix;
	int Radius;
};

#endif __Skeleton_h__

// endof Skeleton.h
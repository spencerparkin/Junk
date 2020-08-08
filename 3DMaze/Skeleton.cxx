// Skeleton.cxx - Construct node-network skeletons - By Spencer T. Parkin

#include <GL/glut.h>
#include <math.h>
#include "Node.h"
#include "Skeleton.h"
#include "Tube.h"
#include <assert.h>

Maze::Maze()
{
}

Maze::~Maze()
{
}

CorridorMaze::CorridorMaze()
{
	int i, j;
	Node *N;

	// This will be a simple flat 2D maze, but we're going
	// to be able to manually navigate it in a 3D environment.

	for(i = 0; i < X_NODES; i++)
	{
		for(j = 0; j < Y_NODES; j++)
		{
			N = &Matrix[i][j];

			// Unfortunately, this order is significant.
			N->Skeleton[0] = (i > 0 ? &Matrix[i-1][j] : 0);			
			N->Skeleton[1] = (j < Y_NODES - 1 ? &Matrix[i][j+1] : 0);
			N->Skeleton[2] = (i < X_NODES - 1 ? &Matrix[i+1][j] : 0);
			N->Skeleton[3] = (j > 0 ? &Matrix[i][j-1] : 0);						

			vec3d_set(&N->Pos,
						GLfloat(i - X_NODES/2) * BOX_WIDTH,
						GLfloat(j - Y_NODES/2) * BOX_LENGTH,
						0.f);
		}
	}
}

CorridorMaze::~CorridorMaze()
{
}

void CorridorMaze::Render(int Tube)
{
}

void CorridorMaze::Generate(void)
{
	GenerateMaze(&Matrix[0][0], 1);
}

void CorridorMaze::PatchNavigationLinks(void)
{
	int i, j;
	for(i = 0; i < X_NODES; i++)
		for(j = 0; j < Y_NODES; j++)
			Matrix[i][j].PatchNavigationLinks();
}

void CorridorMaze::Navigate(void)
{
}

HollowTorusMaze::HollowTorusMaze(GLfloat RingRadius, GLfloat TubeRadius, int RingDensity, int TubeDensity)
{
	this->RingRadius = RingRadius;
	this->TubeRadius = TubeRadius;
	this->RingDensity = RingDensity;
	this->TubeDensity = TubeDensity;

	int i, j;
	GLfloat S, T;
	GLfloat W;

	Matrix = new Node **[TubeDensity];

	for(i = 0; i < TubeDensity; i++)
	{
		Matrix[i] = new Node *[RingDensity];

		S = GLfloat(i) / GLfloat(TubeDensity);

		for(j = 0; j < RingDensity; j++)
		{
			T = GLfloat(j) / GLfloat(RingDensity);

			Matrix[i][j] = new Node();

			W = RingRadius + TubeRadius * cos(S * TWO_PI);
			vec3d_set(&Matrix[i][j]->Pos,
							W * cos(T * TWO_PI),
							W * sin(T * TWO_PI),
							TubeRadius * sin(S * TWO_PI));
		}
	}

	Node *N;

	for(i = 0; i < TubeDensity; i++)
	{
		for(j = 0; j < RingDensity; j++)
		{
			N = Matrix[i][j];

			if(i > 0)
			{
				N->Skeleton[0] = Matrix[i - 1][j];
			}
			else
			{
				N->Skeleton[0] = Matrix[TubeDensity - 1][j];
			}

			if(i < TubeDensity - 1)
			{
				N->Skeleton[1] = Matrix[i + 1][j];
			}
			else
			{
				N->Skeleton[1] = Matrix[0][j];
			}

			if(j > 0)
			{
				N->Skeleton[2] = Matrix[i][j - 1];
			}
			else
			{
				N->Skeleton[2] = Matrix[i][RingDensity - 1];
			}

			if(j < RingDensity - 1)
			{
				N->Skeleton[3] = Matrix[i][j + 1];
			}
			else
			{
				N->Skeleton[3] = Matrix[i][0];
			}
		}
	}
}

HollowTorusMaze::~HollowTorusMaze()
{
	int i, j;

	for(i = 0; i < TubeDensity; i++)
	{
		for(j = 0; j < RingDensity; j++)
		{
			delete Matrix[i][j];
		}

		delete Matrix[i];
	}

	delete Matrix;
}

void HollowTorusMaze::Generate(void)
{
	GenerateMaze(Matrix[0][0], 0);
}

void HollowTorusMaze::Render(int Tube)
{
	int i, j;

	if(!Tube)
	{
		glBegin(GL_LINES);
	}

	for(i = 0; i < TubeDensity; i++)
	{
		for(j = 0; j < RingDensity; j++)
		{
			Matrix[i][j]->Render(Tube);
		}
	}

	if(!Tube)
	{
		glEnd();
	}
}

void HollowTorusMaze::PatchNavigationLinks(void)
{
	int i, j;

	for(i = 0; i < TubeDensity; i++)
	{
		for(j = 0; j < RingDensity; j++)
		{
			Matrix[i][j]->PatchNavigationLinks();
		}
	}
}

void HollowTorusMaze::Navigate(void)
{
	NavTarget = Matrix[TubeDensity - 1][TubeDensity - 1];
	Matrix[0][0]->Navigate();
}

SolidCubeMaze::SolidCubeMaze(int Width, int Height, int Depth)
{
	// Okay, if this was actually a cube then
	// Width would equal Height would equal Depth.
	// But who cares, this is just a silly program anyway.

	this->Width = Width;
	this->Height = Height;
	this->Depth = Depth;

	int i, j, k;

	Matrix = new Node ***[Width];

	for(i = 0; i < Width; i++)
	{
		Matrix[i] = new Node **[Height];

		for(j = 0; j < Height; j++)
		{
			Matrix[i][j] = new Node *[Depth];

			for(k = 0; k < Depth; k++)
			{
				Matrix[i][j][k] = new Node;
			}
		}
	}

	Node *N;

	for(i = 0; i < Width; i++)
	{
		for(j = 0; j < Height; j++)
		{
			for(k = 0; k < Depth; k++)
			{
				N = Matrix[i][j][k];

				vec3d_set(&N->Pos,
						GLfloat(i - Width / 2),
						GLfloat(j - Height / 2),
						GLfloat(k - Depth / 2));

				if(i > 0) N->Skeleton[0] = Matrix[i - 1][j][k];
				if(j > 0) N->Skeleton[1] = Matrix[i][j - 1][k];
				if(k > 0) N->Skeleton[2] = Matrix[i][j][k - 1];

				if(i < Width - 1) N->Skeleton[3] = Matrix[i + 1][j][k];
				if(j < Height - 1) N->Skeleton[4] = Matrix[i][j + 1][k];
				if(k < Depth - 1) N->Skeleton[5] = Matrix[i][j][k + 1];
			}
		}
	}
}

SolidCubeMaze::~SolidCubeMaze()
{
	int i, j, k;

	for(i = 0; i < Width; i++)
	{
		for(j = 0; j < Height; j++)
		{
			for(k = 0; k < Depth; k++)
			{
				delete Matrix[i][j][k];
			}

			delete Matrix[i][j];
		}

		delete Matrix[i];
	}

	delete Matrix;
}

void SolidCubeMaze::Render(int Tube)
{
	int i, j, k;

	if(!Tube)
	{
		glBegin(GL_LINES);
	}

	// For each node in the matrix...
	for(i = 0; i < Width; i++)
	{
		for(j = 0; j < Height; j++)
		{
			for(k = 0; k < Depth; k++)
			{
				Matrix[i][j][k]->Render(Tube);
			}
		}
	}

	if(!Tube)
	{
		glEnd();
	}
}

void SolidCubeMaze::PatchNavigationLinks(void)
{
	int i, j, k;

	for(i = 0; i < Width; i++)
	{
		for(j = 0; j < Height; j++)
		{
			for(k = 0; k < Depth; k++)
			{
				Matrix[i][j][k]->PatchNavigationLinks();
			}
		}
	}
}

void SolidCubeMaze::Generate(void)
{
	GenerateMaze(Matrix[Width / 2][Height / 2][Depth / 2], 0);
}

void SolidCubeMaze::Navigate(void)
{
	int i, j, k;

	for(i = 0; i < Width; i++)
	{
		for(j = 0; j < Height; j++)
		{
			for(k = 0; k < Depth; k++)
			{
				Matrix[i][j][k]->NavFlags = 0;
			}
		}
	}

	NavTarget = Matrix[Width - 1][Height - 1][Depth - 1];
	Matrix[0][0][0]->Navigate();
}

HexigonNode::HexigonNode()
{
	Calc = 1;
}

HexigonNode::~HexigonNode()
{
}

void HexigonNode::CalculateCoordinates(GLfloat X, GLfloat Y, GLfloat Z)
{
	assert(Calc);

	vec3d_set(&Pos, X, Y, Z);

	Calc = 0;

	static HexigonNode *N;
	static GLfloat Angle;
	int i;

	for(i = 0; i < 6; i++)
	{
		N = (HexigonNode *)Skeleton[i];

		if(N && N->Calc)
		{
			Angle = (GLfloat(i) / 6.f) * TWO_PI;
			N->CalculateCoordinates(X + cos(Angle), Y + sin(Angle), Z);
		}
	}
}

HexigonMaze::HexigonMaze(int Radius)
{
	this->Radius = Radius;

	int i, j, k = Radius * 2 - 1;	// k is odd.

	Matrix = new HexigonNode **[k];

	for(i = 0; i < k; i++)
	{
		Matrix[i] = new HexigonNode *[k];

		for(j = 0; j < k; j++)
		{
			if(ABS(i - j) < Radius)
			{
				Matrix[i][j] = new HexigonNode;
			}
			else
			{
				Matrix[i][j] = 0;
			}
		}
	}

	HexigonNode *N;

	for(i = 0; i < k; i++)
	{
		for(j = 0; j < k; j++)
		{
			N = Matrix[i][j];

			if(N)
			{
				int igz, jgz;
				int ilk, jlk;

				igz = i > 0;
				jgz = j > 0;
				ilk = i < k - 1;
				jlk = j < k - 1;

				N->Skeleton[0] = jlk		? Matrix[i    ][j + 1] : 0;
				N->Skeleton[1] = igz		? Matrix[i - 1][j    ] : 0;
				N->Skeleton[2] = igz && jgz ? Matrix[i - 1][j - 1] : 0;
				N->Skeleton[3] = jgz		? Matrix[i    ][j - 1] : 0;
				N->Skeleton[4] = ilk		? Matrix[i + 1][j    ] : 0;
				N->Skeleton[5] = ilk && jlk ? Matrix[i + 1][j + 1] : 0;
			}
		}
	}

	assert(Matrix[Radius - 1][Radius - 1]);
	Matrix[Radius - 1][Radius - 1]->CalculateCoordinates(0.f, 0.f, 0.f);
}

void HexigonMaze::Render(int Tube)
{
	int i, j, k = Radius * 2 - 1;

	if(!Tube)
	{
		glBegin(GL_LINES);
	}

	for(i = 0; i < k; i++)
	{
		for(j = 0; j < k; j++)
		{
			if(Matrix[i][j])
			{
				Matrix[i][j]->Render(Tube);
			}
		}
	}

	if(!Tube)
	{
		glEnd();
	}
}

void HexigonMaze::Generate(void)
{
	GenerateMaze(Matrix[Radius - 1][Radius - 1], 0);
}

void HexigonMaze::PatchNavigationLinks(void)
{
	int i, j, k = Radius * 2 - 1;

	for(i = 0; i < k; i++)
	{
		for(j = 0; j < k; j++)
		{
			if(Matrix[i][j])
			{
				Matrix[i][j]->PatchNavigationLinks();
			}
		}
	}
}

void HexigonMaze::Navigate(void)
{
	NavTarget = Matrix[0][0];
	Matrix[Radius - 1][Radius - 1]->Navigate();
}

// endof Skeleton.cxx
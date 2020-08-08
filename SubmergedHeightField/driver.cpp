// driver.cpp

// Prospective employee: Spencer T. Parkin (spencertparkin@gmail.com) Cell: 801-970-4578

#include <stdio.h>
#include <stdlib.h>

struct Cell
{
	int r, c;
	unsigned short level;
	bool inPlay;
	bool canEscapeFromHere;
	int queueKey;
	Cell* nextCell;
};

int CalcOffset( unsigned short width, unsigned short r, unsigned short c )
{
	return r * width + c;
}

Cell* FindLowestCellInPlay( Cell* cellArray, unsigned short width, unsigned short height )
{
	Cell* lowestCellInPlay = nullptr;

	for( int r = 0; r < height; r++ )
	{
		for( int c = 0; c < width; c++ )
		{
			Cell* cell = &cellArray[ CalcOffset( width, r, c ) ];
			if( cell->inPlay && ( !lowestCellInPlay || cell->level < lowestCellInPlay->level ) )
				lowestCellInPlay = cell;
		}
	}

	return lowestCellInPlay;
}

bool CanCellEscape( Cell* cellArray, unsigned short width, unsigned short height, Cell* cell )
{
	static int queueKey = 0;
	queueKey++;

	Cell* cellQueueHead = cell;
	cell->queueKey = queueKey;
	cell->nextCell = nullptr;

	while( cellQueueHead )
	{
		cell = cellQueueHead;
		cellQueueHead = cellQueueHead->nextCell;

		// If we run into the edge, we've escaped.
		if( cell->r == 0 || cell->r == height - 1 )
			return true;
		if( cell->c == 0 || cell->c == width - 1 )
			return true;

		// If we run into a cell that escaped, then we can escape too.
		if( cell->canEscapeFromHere )
			return true;

		for( int dir = 0; dir < 4; dir++ )
		{
			Cell* adjacentCell = nullptr;
			if( dir == 0 && cell->r > 0 )
				adjacentCell = &cellArray[ CalcOffset( width, cell->r - 1, cell->c ) ];
			else if( dir == 1 && cell->r < height - 1 )
				adjacentCell = &cellArray[ CalcOffset( width, cell->r + 1, cell->c ) ];
			else if( dir == 2 && cell->c > 0 )
				adjacentCell = &cellArray[ CalcOffset( width, cell->r, cell->c - 1 ) ];
			else if( dir == 3 && cell->c < width - 1 )
				adjacentCell = &cellArray[ CalcOffset( width, cell->r, cell->c + 1 ) ];

			// Is the cell already queued?
			if( adjacentCell->queueKey != queueKey )
			{
				// No.  Can water flow in this direction?
				if( adjacentCell->level <= cell->level )
				{
					// Yes, queue it up.
					adjacentCell->nextCell = cellQueueHead;
					adjacentCell->queueKey = queueKey;
					cellQueueHead = adjacentCell;
				}
			}
		}
	}

	return false;
}

// Calculate the volume of water retained by the given board (depth-field) when baptized in water.
unsigned __int64 CalculateVolume( const unsigned short* board, unsigned short width, unsigned short height )
{
	unsigned __int64 totalVolume = 0;
	
	Cell* cellArray = new Cell[ width * height ];

	for( int r = 0; r < height; r++ )
	{
		for( int c = 0; c < width; c++ )
		{
			Cell* cell = &cellArray[ CalcOffset( width, r, c ) ];
			cell->r = r;
			cell->c = c;
			cell->level = board[ CalcOffset( width, r, c ) ];
			cell->inPlay = true;
			cell->canEscapeFromHere = false;
			cell->queueKey = 0;
			cell->nextCell = nullptr;
		}
	}

	while( true )
	{
		Cell* cell = FindLowestCellInPlay( cellArray, width, height );
		if( !cell )
			break;

		if( CanCellEscape( cellArray, width, height, cell ) )
		{
			cell->inPlay = false;
			cell->canEscapeFromHere = true;
		}
		else
		{
			cell->level++;
			totalVolume++;
		}
	}

	delete[] cellArray;

	return totalVolume;
}

/*struct Cell
{
	int r, c;
	enum Type { GAS, LIQUID, SOLID };
	Type type;
	bool queued;
	Cell* nextCell;
};

void AssertFatal( bool condition )
{
	if( !condition )
	{
		fprintf( stderr, "I suck.\n" );
		getchar();
		exit(-1);
	}
}

int CalcOffset( int width, int r, int c )
{
	return( r * width + c );
}

Cell* FindUnretainableCellOnPerimeter( Cell* cellArray, unsigned short width, unsigned short height )
{
	for( int r = 0; r < height; r++ )
	{
		Cell* cell = &cellArray[ CalcOffset( width, r, 0 ) ];
		if( cell->type == Cell::LIQUID )
			return cell;

		cell = &cellArray[ CalcOffset( width, r, width - 1 ) ];
		if( cell->type == Cell::LIQUID )
			return cell;
	}

	for( int c = 0; c < width; c++ )
	{
		Cell* cell = &cellArray[ CalcOffset( width, 0, c ) ];
		if( cell->type == Cell::LIQUID )
			return cell;

		cell = &cellArray[ CalcOffset( width, height - 1, c ) ];
		if( cell->type == Cell::LIQUID )
			return cell;
	}

	return nullptr;
}

// We can assume that the given cell is adjacent to a gas.
void QueueIfUnretainable( Cell*& cellQueueHead, Cell* cell )
{
	if( !cell->queued && cell->type == Cell::LIQUID )
	{
		cell->nextCell = cellQueueHead;
		cellQueueHead = cell;
		cell->queued = true;
	}
}

unsigned __int64 CalculatePlanarRetainedVolume( const unsigned short* board, unsigned short width, unsigned short height, unsigned short planarZ )
{
	unsigned __int64 netVolume = 0;

	Cell* cellArray = new Cell[ width * height ];

	for( int r = 0; r < height; r++ )
	{
		for( int c = 0; c < width; c++ )
		{
			int offset = CalcOffset( width, r, c );
			Cell* cell = &cellArray[ offset ];
			unsigned short z = board[ offset ];

			cell->type = ( z < planarZ ) ? Cell::LIQUID : Cell::SOLID;
			cell->r = r;
			cell->c = c;
			cell->nextCell = nullptr;
			cell->queued = false;

			if( cell->type == Cell::LIQUID )
				netVolume++;
		}
	}

	while( true )
	{
		Cell* cell = FindUnretainableCellOnPerimeter( cellArray, width, height );
		if( !cell )
			break;

		// Every cell that can be found through a path of adjacency is also unretainable.
		cell->queued = true;
		Cell* cellQueueHead = cell;
		while( cellQueueHead )
		{
			cell = cellQueueHead;
			cellQueueHead = cell->nextCell;

			AssertFatal( cell->type == Cell::LIQUID );

			cell->type = Cell::GAS;
			netVolume--;

			if( cell->r > 0 )
				QueueIfUnretainable( cellQueueHead, &cellArray[ CalcOffset( width, cell->r - 1, cell->c ) ] );

			if( cell->r < height - 1 )
				QueueIfUnretainable( cellQueueHead, &cellArray[ CalcOffset( width, cell->r + 1, cell->c ) ] );

			if( cell->c > 0 )
				QueueIfUnretainable( cellQueueHead, &cellArray[ CalcOffset( width, cell->r, cell->c - 1 ) ] );

			if( cell->c < width - 1 )
				QueueIfUnretainable( cellQueueHead, &cellArray[ CalcOffset( width, cell->r, cell->c + 1 ) ] );
		}
	}

	delete[] cellArray;

	return netVolume;
}

// Calculate the volume of water retained by the given board (depth-field) when baptized in water.
// The following algorithm should work due to the natural constraints on the topology of the board.
unsigned __int64 CalculateVolume( const unsigned short* board, unsigned short width, unsigned short height )
{
	unsigned short maxZ = 0;
	for( int r = 0; r < height; r++ )
	{
		for( int c = 0; c < width; c++ )
		{
			int offset = CalcOffset( width, r, c );
			unsigned short z = board[ offset ];
			if( z > maxZ )
				maxZ = z;
		}
	}

	unsigned __int64 totalVolume = 0;
	for( int z = 1; z <= maxZ; z++ )
	{
		unsigned __int64 retainedVolume = CalculatePlanarRetainedVolume( board, width, height, z );
		totalVolume += retainedVolume;
	}

	return totalVolume;
}*/

int main( int argc, char** argv )
{
#if 0
	const unsigned short width = 3;
	const unsigned short height = 4;

	unsigned short board[ width * height ];

	board[ CalcOffset( width, 0, 0 ) ] = 4;
	board[ CalcOffset( width, 0, 1 ) ] = 3;
	board[ CalcOffset( width, 0, 2 ) ] = 3;

	board[ CalcOffset( width, 1, 0 ) ] = 4;
	board[ CalcOffset( width, 1, 1 ) ] = 2;
	board[ CalcOffset( width, 1, 2 ) ] = 3;

	board[ CalcOffset( width, 2, 0 ) ] = 4;
	board[ CalcOffset( width, 2, 1 ) ] = 1;
	board[ CalcOffset( width, 2, 2 ) ] = 1;

	board[ CalcOffset( width, 3, 0 ) ] = 4;
	board[ CalcOffset( width, 3, 1 ) ] = 4;
	board[ CalcOffset( width, 3, 2 ) ] = 4;

	unsigned __int64 volume = CalculateVolume( board, width, height );
	printf( "Volume = %lld", volume );
	getchar();
#endif

	const unsigned short width = 50;
	const unsigned short height = 50;

	unsigned short board[ width * height ];

	srand(0);

	for( int i = 0; i < 10; i++ )
	{
		for( int r = 0; r < height; r++ )
			for( int c = 0; c < width; c++ )
				board[ CalcOffset( width, r, c ) ] = rand() % 50;	// Not a good way to generate a random number.

		unsigned __int64 volume = CalculateVolume( board, width, height );
		printf( "Volume = %lld\n", volume );
	}

	printf( "Done!\n" );
	getchar();

	return 0;
}

// driver.cpp
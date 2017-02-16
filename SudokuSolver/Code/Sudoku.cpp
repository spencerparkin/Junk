// Sudoku.cpp

#include "Sudoku.h"
#include <stdio.h>

//======================================================================================
Sudoku::Sudoku( void )
{
	Allocate();
}

//======================================================================================
Sudoku::~Sudoku( void )
{
	Deallocate();
}

//======================================================================================
bool Sudoku::AssignNumber( int r, int c, int number )
{
	if( number < 0 || number > 9 )
		return false;

	if( r < 0 || r > 8 || c < 0 || c > 8 )
		return false;

	square[r][c].number = number;
	return true;
}

//======================================================================================
bool Sudoku::AssignSquare( int square[9][9] )
{
	for( int r = 0; r < 9; r++ )
		for( int c = 0; c < 9; c++ )
			if( !AssignNumber( r, c, square[r][c] ) )
				return false;
	return true;
}

//======================================================================================
bool Sudoku::LoadInitialCondition( const char* file )
{
	return false;
}

//======================================================================================
void Sudoku::Print( FILE* file ) const
{
	for( int r = 0; r < 9; r++ )
	{
		for( int c = 0; c < 9; c++ )
		{
			SubSquare* subSquare = &square[r][c];

			if( subSquare->number > 0 )
				fprintf( file, "%d", subSquare->number );
			else
				fprintf( file, " " );

			fprintf( file, " " );
		}

		fprintf( file, "\n" );
	}
}

//======================================================================================
bool Sudoku::Solve( void )
{
	Sudoku* candidacySquare = MakeCopy();
	candidacySquare->DetermineAllCandidates();
	return SolveRecursive( candidacySquare, 1 );
}

//======================================================================================
bool Sudoku::SolveRecursive( Sudoku* candidacySquare, int depth )
{
	DetermineAllCandidates();

	while( !IsSolved() )
	{
		SubSquare* subSquare = FindUniquelyDeterminedAndUnknownSubSquare();
		if( subSquare )
			subSquare->number = subSquare->candidates.front();
		else
		{
			SubSquareList subSquareList;
			FindAllUnknownSubSquaresInDeterminacyOrder( subSquareList );
			if( subSquareList.size() == 0 )
			{
				// We get here if the puzzle can't be solved along the chosen path.
				break;
			}

			bool solutionPossible = false;
			for( SubSquareList::iterator subSquareIter = subSquareList.begin(); subSquareIter != subSquareList.end(); subSquareIter++ )
			{
				subSquare = *subSquareIter;
				NumberList* candidates = &subSquare->candidates;

				for( NumberList::iterator candidateIter = candidates->begin(); candidateIter != candidates->end(); candidateIter++ )
				{
					int number = *candidateIter;

					// See the comment below this one.
					if( !candidacySquare->IsCandidate( subSquare->r, subSquare->c, number ) )
						continue;

					Sudoku* sudokuCopy = MakeCopy();
					sudokuCopy->square[ subSquare->r ][ subSquare->c ].number = number;

					solutionPossible = sudokuCopy->SolveRecursive( candidacySquare, depth + 1 );
					delete sudokuCopy;
					if( solutionPossible )
					{
						subSquare->number = number;
						break;
					}

					// Peter's realization, if I undertand it correctly, (which thing is quite debatable at this point), is
					// that while trying each candidate here, we GLOBALLY eliminate it, (eliminate it from all future search
					// paths that we try in the remainder of the entire algorithm), in the case that we cannot find a solution
					// with this candidate anywhere beyond this point in the search path.
					//
					// This is not immediately obvious to me at all, and so my implementation of Peter's optimization
					// may not be correct, and therefore, this algorithm may not be correct, even if it does happen
					// to find a solution.  Or it could be correct, but I just don't fully understand yet why it is.
					//
					// On the premise that the initial condition of the puzzle we're given does have a solution (a valid completion),
					// then for any one trial choice we make along our search for a solution, if that choice leads to an unsolvable puzzle,
					// then that choice must be one that is not made in any solution of the puzzle.  Thinking recursively, however,
					// the initial condition changes with each sub-problem, and each sub-problem may not be a puzzle with a valid completion,
					// in which case, the premise is false, and we could be globally eliminating candidates that shouldn't be eliminated.  :(  Sigh...I'm dumb.
					// Challenging the first statment made here, while a choice may have doomed us to an unsolvable puzzle after one point in our searching, why must
					// this choice necessarily also similarly doom us if made at any other point in our searching?
					candidacySquare->EliminateCandidate( subSquare->r, subSquare->c, number );
				}

				if( solutionPossible )
					break;
			}

			if( !solutionPossible )
				break;
		}

		DetermineAllCandidates();
	}

	return IsSolved();
}

//======================================================================================
bool Sudoku::IsCandidate( int r, int c, int number ) const
{
	NumberList::iterator iter;
	return FindCandidate( r, c, number, iter );
}

//======================================================================================
bool Sudoku::FindCandidate( int r, int c, int number, NumberList::iterator& iter ) const
{
	SubSquare* subSquare = &square[r][c];
	NumberList* candidates = &subSquare->candidates;
	for( iter = candidates->begin(); iter != candidates->end(); iter++ )
		if( *iter == number )
			return true;
	return false;
}

//======================================================================================
void Sudoku::EliminateCandidate( int r, int c, int number ) const
{
	NumberList::iterator iter;
	if( FindCandidate( r, c, number, iter ) )
		square[r][c].candidates.erase( iter );
}

//======================================================================================
// This routine is only correct if DetermineAllCandidates is called prior to it.
Sudoku::SubSquare* Sudoku::FindUniquelyDeterminedAndUnknownSubSquare( void )
{
	SubSquareList subSquareList;
	FindDeterminedAndUnknownSubSquares( subSquareList, 1 );
	if( subSquareList.size() == 0 )
		return 0;
	return subSquareList.front();
}

//======================================================================================
void Sudoku::FindAllUnknownSubSquaresInDeterminacyOrder( SubSquareList& subSquareList )
{
	for( int candidacyCount = 1; candidacyCount <= 9; candidacyCount++ )
	{
		SubSquareList subSquareListForCount;
		FindDeterminedAndUnknownSubSquares( subSquareListForCount, candidacyCount );
		if( subSquareListForCount.size() > 0 )
			subSquareList.insert( subSquareList.end(), subSquareListForCount.begin(), subSquareListForCount.end() );
	}
}

//======================================================================================
// This routine is only correct if DetermineAllCandidates is called prior to it.
void Sudoku::FindDeterminedAndUnknownSubSquares( SubSquareList& subSquareList, int candidacyCount )
{
	subSquareList.clear();

	for( int r = 0; r < 9; r++ )
	{
		for( int c = 0; c < 9; c++ )
		{
			SubSquare* subSquare = &square[r][c];
			if( subSquare->number )
				continue;

			if( subSquare->candidates.size() == candidacyCount )
				subSquareList.push_back( subSquare );
		}
	}
}

//======================================================================================
void Sudoku::DetermineAllCandidates( void ) const
{
	for( int r = 0; r < 9; r++ )
		for( int c = 0; c < 9; c++ )
			DetermineCandidates( r, c );
}

//======================================================================================
void Sudoku::DetermineCandidates( int r, int c ) const
{
	const SubSquare* subSquare = &square[r][c];
	NumberList* candidates = &subSquare->candidates;
	candidates->clear();
	for( int number = 1; number <= 9; number++ )
		if( !NumberInUnit( r, c, number ) )
			candidates->push_back( number );
}

//======================================================================================
bool Sudoku::NumberInRow( int r, int number ) const
{
	for( int c = 0; c < 9; c++ )
		if( square[r][c].number == number )
			return true;
	return false;
}

//======================================================================================
bool Sudoku::NumberInCol( int c, int number ) const
{
	for( int r = 0; r < 9; r++ )
		if( square[r][c].number == number )
			return true;
	return false;
}

//======================================================================================
bool Sudoku::NumberInBox( int r, int c, int number ) const
{
	r -= r % 3;
	c -= c % 3;
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
			if( square[ r + i ][ c + j ].number == number )
				return true;
	return false;
}

//======================================================================================
bool Sudoku::NumberInUnit( int r, int c, int number ) const
{
	if( NumberInRow( r, number ) )
		return true;
	if( NumberInCol( c, number ) )
		return true;
	if( NumberInBox( r, c, number ) )
		return true;
	return false;
}

//======================================================================================
bool Sudoku::IsSolved( void ) const
{
	// If there are any unknown squares, then the puzzle is not yet solved.
	for( int r = 0; r < 9; r++ )
		for( int c = 0; c < 9; c++ )
			if( !square[r][c].number )
				return false;

	// If all squares are filled in, then all that remains is to check the
	// validity of each unit (row, column or box).
	return IsValid();
}

//======================================================================================
bool Sudoku::RowValid( int r ) const
{
	NumberMap numberMap;
	for( int c = 0; c < 9; c++ )
	{
		int number = square[r][c].number;
		if( number )
		{
			NumberMap::iterator iter = numberMap.find( number );
			if( iter != numberMap.end() )
				return false;

			numberMap[ number ] = 1;
		}
	}

	return true;
}

//======================================================================================
bool Sudoku::ColValid( int c ) const
{
	NumberMap numberMap;
	for( int r = 0; r < 9; r++ )
	{
		int number = square[r][c].number;
		if( number )
		{
			NumberMap::iterator iter = numberMap.find( number );
			if( iter != numberMap.end() )
				return false;

			numberMap[ number ] = 1;
		}
	}

	return true;
}

//======================================================================================
bool Sudoku::BoxValid( int r, int c ) const
{
	r -= r % 3;
	c -= c % 3;

	NumberMap numberMap;
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			int number = square[ r + i ][ c + j ].number;
			if( number )
			{
				NumberMap::iterator iter = numberMap.find( number );
				if( iter != numberMap.end() )
					return false;

				numberMap[ number ] = 1;
			}
		}
	}

	return true;
}

//======================================================================================
bool Sudoku::UnitValid( int r, int c ) const
{
	if( !RowValid( r ) )
		return false;
	if( !ColValid( r ) )
		return false;
	if( !BoxValid( r, c ) )
		return false;
	return true;
}

//======================================================================================
bool Sudoku::IsValid( void ) const
{
	for( int r = 0; r < 9; r++ )
		for( int c = 0; c < 9; c++ )
			if( !UnitValid( r, c ) )
				return false;
	return true;
}

//======================================================================================
Sudoku* Sudoku::MakeCopy( void ) const
{
	Sudoku* copy = new Sudoku();
	
	for( int r = 0; r < 9; r++ )
		for( int c = 0; c < 9; c++ )
			copy->square[r][c] = square[r][c];

	return copy;
}

//======================================================================================
void Sudoku::Allocate( void )
{
	square = new SubSquare*[9];
	for( int r = 0; r < 9; r++ )
	{
		square[r] = new SubSquare[9];
		for( int c = 0; c < 9; c++ )
		{
			SubSquare* subSquare = &square[r][c];
			subSquare->number = 0;
			subSquare->r = r;
			subSquare->c = c;
		}
	}
}

//======================================================================================
void Sudoku::Deallocate( void )
{
	for( int r = 0; r < 9; r++ )
		delete[] square[r];
	delete[] square;
}

// Sudoku.cpp
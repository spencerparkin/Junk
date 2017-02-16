// Sudoku.h

#include <list>
#include <map>

//======================================================================================
// A completed sudoku puzzle is a special case of 9x9 Latin square where each of the
// 9 sub-squares of size 3x3 each contain the numbers 1 through 9.  Each of these
// 3x3 sub-squares are often referred to as a box.  The row, column and box associated
// with any one sub-square is called a unit.
//
// There are two fundamental questions that accompany any given validly incomplete
// Sudoku puzzle.  Does there exist a solution to the puzzle?  And if so, is it unique?
// Ideally the solution algorithm we come up with here will fail if and only if there
// does not exist a solution to the puzzle.  Uniqueness of a found solution, however,
// may be harder to determine.
class Sudoku
{
public:

	typedef std::list< int > NumberList;
	typedef std::map< int, int > NumberMap;

	struct SubSquare
	{
		int number;							// We let zero indicate an unknown number.
		mutable NumberList candidates;		// For each sub-square, we keep a list of possible candidates for the square.
		int r, c;							// Let each sub-square know about its own position in the square.
	};

	typedef std::list< SubSquare* > SubSquareList;

	Sudoku( void );
	~Sudoku( void );

	bool LoadInitialCondition( const char* file );
	void Print( FILE* file ) const;
	bool Solve( void );
	bool IsSolved( void ) const;
	Sudoku* MakeCopy( void ) const;

	bool RowValid( int r ) const;
	bool ColValid( int c ) const;
	bool BoxValid( int r, int c ) const;
	bool UnitValid( int r, int c ) const;
	bool IsValid( void ) const;

	bool NumberInRow( int r, int number ) const;
	bool NumberInCol( int c, int number ) const;
	bool NumberInBox( int r, int c, int number ) const;
	bool NumberInUnit( int r, int c, int number ) const;

	SubSquare* FindUniquelyDeterminedAndUnknownSubSquare( void );
	void FindAllUnknownSubSquaresInDeterminacyOrder( SubSquareList& subSquareList );
	void FindDeterminedAndUnknownSubSquares( SubSquareList& subSquareList, int candidacyCount );

	void DetermineAllCandidates( void ) const;
	void DetermineCandidates( int r, int c ) const;

	bool AssignNumber( int r, int c, int number );
	bool AssignSquare( int square[9][9] );

private:

	bool IsCandidate( int r, int c, int number ) const;
	void EliminateCandidate( int r, int c, int number ) const;
	bool FindCandidate( int r, int c, int number, NumberList::iterator& iter ) const;

	bool SolveRecursive( Sudoku* candidacySquare, int depth );

	void Allocate( void );
	void Deallocate( void );

	SubSquare** square;
};

// Sudoku.h
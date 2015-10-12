// Geometry.cpp

#include "Geometry.h"

using namespace Geometer;

GLuint Geometry::newSelectionID = 1;

Geometry::Geometry( void )
{
	selectionID = newSelectionID++;
}

/*virtual*/ Geometry::~Geometry( void )
{
	// TODO: Get Lua state from the app and clear our reference in the stack at our registery-index.
}

const GACompute::Operand* Geometry::OperandTree( void ) const
{
	return 0;
}

GACompute::Operand* Geometry::OperandTree( void )
{
	return 0;
}

GLuint Geometry::SelectionID( void ) const
{
	return selectionID;
}

// Geometry.cpp
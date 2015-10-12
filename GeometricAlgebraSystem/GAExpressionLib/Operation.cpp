// Operation.cpp

#include "Operation.h"
#include <string.h>

using namespace GAExpressionLib;

Operation::Operation( void )
{
}

/*virtual*/ Operation::~Operation( void )
{
}

/*virtual*/ Operand* Operation::Clone( void ) const
{
	Operand* operand = Allocate();
	Operation* operation = dynamic_cast< Operation* >( operand );
	if( !operation )
	{
		delete operand;
		return nullptr;
	}

	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		operation->operandList.InsertAfter()->data = node->data->Clone();

	return operation;
}

/*virtual*/ void Operation::Print( char* buffer, int bufferSize, PrintFormat format ) const
{
	buffer[0] = '\0';

	if( operandList.Count() == 0 )
		PrintIdentity( buffer, bufferSize, format );

	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		const Operand* operand = node->data;
		const Operation* operation = dynamic_cast< const Operation* >( operand );

		bool parenthesisNeeded = false;
		if( TakesPrecedenceOver( operation ) )
			parenthesisNeeded = true;

		if( parenthesisNeeded )
		{
			switch( format )
			{
				case PRINT_SORT:
				case PRINT_NORMAL:
				{
					strcat_s( buffer, bufferSize, "(" );
					break;
				}
				case PRINT_LATEX:
				{
					strcat_s( buffer, bufferSize, "\\left(" );
					break;
				}
			}
		}

		char tempBuffer[ 5*1024 ];
		operand->Print( tempBuffer, sizeof( tempBuffer ), format );
		strcat_s( buffer, bufferSize, tempBuffer );

		if( parenthesisNeeded )
		{
			switch( format )
			{
				case PRINT_SORT:
				case PRINT_NORMAL:
				{
					strcat_s( buffer, bufferSize, ")" );
					break;
				}
				case PRINT_LATEX:
				{
					strcat_s( buffer, bufferSize, "\\left)" );
					break;
				}
			}
		}

		if( node->Next() )
		{
			PrintBinaryOperator( tempBuffer, sizeof( tempBuffer ), format );
			strcat_s( buffer, bufferSize, tempBuffer );
		}
	}
}

/*virtual*/ bool Operation::IsScalar( void ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		if( !node->data->IsScalar() )
			return false;		// In this case, we really just don't know the answer.

	// The scalars are closed under all operations.
	return true;
}

/*virtual*/ bool Operation::Sort( void )
{
	int adjacentSwapCount = SortByPrint();
	if( adjacentSwapCount > 0 )
		return true;

	return false;
}

int Operation::SortByPrint( void )
{
	return operandList.Sort( ( OperandList::SortCompareFunc )&Operation::CompareByOperandPrint );
}

/*static*/ int Operation::CompareByOperandPrint( const Operand* leftOperand, const Operand* rightOperand )
{
	char leftBuffer[ 5*1024 ];
	char rightBuffer[ 5*1024 ];

	leftOperand->Print( leftBuffer, sizeof( leftBuffer ), PRINT_SORT );
	rightOperand->Print( rightBuffer, sizeof( rightBuffer ), PRINT_SORT );

	return strcmp( leftBuffer, rightBuffer );
}

// Operation.cpp
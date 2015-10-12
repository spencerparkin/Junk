// GradeSelector.cpp

#include "GradeSelector.h"
#include "Expander.h"
#include "Addition.h"
#include "OuterProduct.h"
#include "Vector.h"
#include <string.h>
#include <stdio.h>

using namespace GAExpressionLib;

GradeSelector::GradeSelector( void )
{
	type = TYPE_INCLUSIVE;
	grade = -1;
	operand = nullptr;
}

/*virtual*/ GradeSelector::~GradeSelector( void )
{
	delete operand;
}

/*virtual*/ bool GradeSelector::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const GradeSelector* >( operand ) )
		return true;

	return false;
}

/*virtual*/ Operand* GradeSelector::Clone( void ) const
{
	GradeSelector* gradeSelector = new GradeSelector();
	gradeSelector->type = type;
	gradeSelector->grade = grade;

	if( operand )
		gradeSelector->operand = operand->Clone();

	return gradeSelector;
}

/*virtual*/ void GradeSelector::Print( char* buffer, int bufferSize, PrintFormat format ) const
{
	buffer[0] = '\0';

	switch( format )
	{
		case PRINT_NORMAL:
		{
			strcat_s( buffer, bufferSize, "<" );
			break;
		}
		case PRINT_LATEX:
		{
			strcat_s( buffer, bufferSize, "\\left\\langle" );
			break;
		}
	}

	char tempBuffer[ 5*1024 ];
	if( operand )
		operand->Print( tempBuffer, sizeof( tempBuffer ), format );
	else
		strcpy_s( tempBuffer, sizeof( tempBuffer ), "?" );
	strcat_s( buffer, bufferSize, tempBuffer );

	switch( format )
	{
		case PRINT_NORMAL:
		{
			if( type == TYPE_INCLUSIVE )
				sprintf_s( tempBuffer, sizeof( tempBuffer ), ">_{%d}", grade );
			else if( type == TYPE_EXCLUSIVE )
				sprintf_s( tempBuffer, sizeof( tempBuffer ), ">_{!%d}", grade );
			else
				strcpy_s( tempBuffer, sizeof( tempBuffer ), ">_{?}" );
			break;
		}
		case PRINT_LATEX:
		{
			if( type == TYPE_INCLUSIVE )
				sprintf_s( tempBuffer, sizeof( tempBuffer ), "\\right\\rangle_{%d}", grade );
			else if( type == TYPE_EXCLUSIVE )
				sprintf_s( tempBuffer, sizeof( tempBuffer ), "\\right\\rangle_{!%d}", grade );
			else
				strcpy_s( tempBuffer, sizeof( tempBuffer ), "\\right\\rangle_{?}" );
			break;
		}
	}

	strcat_s( buffer, bufferSize, tempBuffer );
}

bool GradeSelector::Select( void )
{
	if( !operand )
		return false;

	Expander expander;
	expander.expansionTarget = Expander::SUM_OF_BLADES;
	expander.ManipulateTree( &operand );

	Addition* addition = dynamic_cast< Addition* >( operand );
	if( !addition )
		return false;

	Addition::OperandList::Node* node = addition->operandList.Head();
	while( node )
	{
		Addition::OperandList::Node* nextNode = node->Next();

		Operand* nestedOperand = node->data;
		int count = -1;

		OuterProduct* outerProduct = dynamic_cast< OuterProduct* >( nestedOperand );
		Vector* vector = dynamic_cast< Vector* >( nestedOperand );

		if( outerProduct && outerProduct->IsHomogeneousOfVectors() )
			count = outerProduct->operandList.Count();
		else if( vector )
			count = 1;
		else if( nestedOperand->IsScalar() )
			count = 0;

		if( ( count == grade && type == TYPE_EXCLUSIVE ) || ( count != grade && type == TYPE_INCLUSIVE ) )
			addition->operandList.Remove( node );

		node = nextNode;
	}

	return true;
}

// GradeSelector.cpp
// InnerProduct.cpp

#include "GACompute.h"

using namespace GACompute;

InnerProduct::InnerProduct( void )
{
}

/*virtual*/ InnerProduct::~InnerProduct( void )
{
}

/*virtual*/ Operand* InnerProduct::Create( void ) const
{
	return new InnerProduct();
}

/*virtual*/ Operand* InnerProduct::CreateVacuousCase( void ) const
{
	NumericScalar* numericScalar = new NumericScalar();
	numericScalar->SetReal( 1.0 );
	return numericScalar;
}

/*virtual*/ bool InnerProduct::TakesPrecedenceOver( const Operation* operation ) const
{
	if( dynamic_cast< const Sum* >( operation ) ||
		dynamic_cast< const InnerProduct* >( operation ) )
	{
		return true;
	}

	return false;
}

/*virtual*/ bool InnerProduct::CanDistributeOver( const Operation* operation ) const
{
	if( dynamic_cast< const Sum* >( operation ) )
		return true;
	return false;
}

/*virtual*/ const char* InnerProduct::OperatorSymbol( PrintFormat printFormat ) const
{
	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			return ".";
		}
		case PRINT_LATEX:
		{
			return "\\cdot ";
		}
	}

	return "?";
}

/*virtual*/ void InnerProduct::PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "(IP-Empty)" );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "\\left(\\mbox{IP-Empty}\\right)" );
			break;
		}
	}
}

/*virtual*/ Operand* InnerProduct::Simplify( const SimplificationContext& simplificationContext )
{
	if( IsAdditiveIdentity() )
	{
		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( 0.0 );
		return numericScalar;
	}

	Operand* operand = Distribute();
	if( operand )
		return operand;

	operand = Operation::Simplify( simplificationContext );
	if( operand )
		return operand;

	operand = MakeNonDegenerate();
	if( operand )
		return operand;

	if( MarryScalars() )
		return this;

	if( operandList->Count() != 2 )
	{
		Error* error = new Error();
		error->Format( "An inner product expected exactly 2 operands at this point, but has %d of them.", operandList->Count() );
		throw error;
	}

	OperandList::Node* leftNode = operandList->Head();
	OperandList::Node* rightNode = operandList->Tail();

	Operand* leftOperand = leftNode->data;
	Operand* rightOperand = rightNode->data;

	Vector* leftVector = dynamic_cast< Vector* >( leftOperand );
	Vector* rightVector = dynamic_cast< Vector* >( rightOperand );

	bool leftOperandIsBlade = leftOperand->IsBlade();
	bool rightOperandIsBlade = rightOperand->IsBlade();
	
	bool leftOperandIsScalar = leftOperand->IsScalar();
	bool rightOperandIsScalar = rightOperand->IsScalar();

	if( leftVector && rightVector )
	{
		GeometricProduct* geometricProduct = GenerateScalarProduct( leftVector, rightVector );
		return geometricProduct;
	}
	else if( leftVector && rightOperandIsBlade )
	{
		OuterProduct* rightBlade = ( OuterProduct* )rightOperand;
		Sum* sum = GenerateAlternatingSeries( leftVector, rightBlade, VECTOR_DOT_BLADE );
		return sum;
	}
	else if( leftOperandIsBlade && rightVector )
	{
		OuterProduct* leftBlade = ( OuterProduct* )leftOperand;
		Sum* sum = GenerateAlternatingSeries( rightVector, leftBlade, BLADE_DOT_VECTOR );
		return sum;
	}
	else if( leftOperandIsBlade && rightOperandIsBlade )
	{
		OuterProduct* leftBlade = ( OuterProduct* )leftOperand;
		OuterProduct* rightBlade = ( OuterProduct* )rightOperand;

		if( leftBlade->operandList->Count() <= rightBlade->operandList->Count() )
		{
			OperandList::Node* node = leftBlade->operandList->Tail();
			leftBlade->operandList->Remove( node, false );
			InnerProduct* innerProduct = new InnerProduct();
			innerProduct->operandList->InsertAfter( 0, node );
			node = operandList->Tail();
			operandList->Remove( node, false );
			innerProduct->operandList->InsertAfter( 0, node );
			operandList->InsertAfter()->data = innerProduct;
			return this;
		}
		else
		{
			OperandList::Node* node = rightBlade->operandList->Head();
			rightBlade->operandList->Remove( node, false );
			InnerProduct* innerProduct = new InnerProduct();
			innerProduct->operandList->InsertBefore( 0, node );
			node = operandList->Head();
			operandList->Remove( node, false );
			innerProduct->operandList->InsertBefore( 0, node );
			operandList->InsertBefore()->data = innerProduct;
			return this;
		}
	}
	else if( leftOperandIsScalar && rightOperandIsBlade )
	{
		Operand* leftScalar = leftNode->data;
		OuterProduct* rightBlade = ( OuterProduct* )rightNode->data;

		leftNode->data = 0;
		rightNode->data = 0;
		
		Vector* vector = ( Vector* )rightBlade->operandList->Head()->data;
		vector->MarryWithScalar( leftScalar );
		return rightBlade;
	}
	else if( leftOperandIsBlade && rightOperandIsScalar )
	{
		OuterProduct* leftBlade = ( OuterProduct* )leftNode->data;
		Operand* rightScalar = rightNode->data;

		leftNode->data = 0;
		rightNode->data = 0;

		Vector* vector = ( Vector* )leftBlade->operandList->Head()->data;
		vector->MarryWithScalar( rightScalar );
		return leftBlade;
	}
	else if( leftOperandIsScalar && rightOperandIsScalar )
	{
		operandList->Remove( leftNode, false );
		operandList->Remove( rightNode, false );

		GeometricProduct* geometricProduct = new GeometricProduct();
		geometricProduct->operandList->InsertAfter( 0, leftNode );
		geometricProduct->operandList->InsertAfter( 0, rightNode );
		return geometricProduct;
	}

	Error* error = new Error();
	error->Format( "An inner product could not simplify." );
	throw error;

	return 0;
}

GeometricProduct* InnerProduct::GenerateScalarProduct( const Vector* vectorA, const Vector* vectorB )
{
	GeometricProduct* geometricProduct = new GeometricProduct();

	InnerProductScalar* innerProductScalar = new InnerProductScalar();
	innerProductScalar->SetNames( vectorA->GetName(), vectorB->GetName() );
	geometricProduct->operandList->InsertAfter()->data = innerProductScalar;

	if( vectorA->GetScalar() )
		geometricProduct->operandList->InsertBefore()->data = vectorA->GetScalar()->Clone();

	if( vectorB->GetScalar() )
		geometricProduct->operandList->InsertBefore()->data = vectorB->GetScalar()->Clone();

	return geometricProduct;
}

Sum* InnerProduct::GenerateAlternatingSeries( const Vector* vector, OuterProduct* blade, DotOrder dotOrder )
{
	if( blade->operandList->Count() <= 1 )
	{
		Error* error = new Error();
		error->Format( "Degenerate blade encountered while trying to generate an alternating series from an inner product." );
		throw error;
	}

	int count = 0;
	switch( dotOrder )
	{
		case VECTOR_DOT_BLADE:
		{
			count = 0;
			break;
		}
		case BLADE_DOT_VECTOR:
		{
			if( blade->operandList->Count() % 2 == 0 )
				count = 1;
			break;
		}
	}

	Sum* sum = new Sum();

	for( OperandList::Node* node = blade->operandList->Head(); node; node = node->Next() )
	{
		OperandList::Node* prevNode = node->Prev();
		blade->operandList->Remove( node, false );
		OuterProduct* outerProduct = ( OuterProduct* )blade->Clone();
		Vector* otherVector = ( Vector* )node->data;

		GeometricProduct* geometricProduct = GenerateScalarProduct( vector, otherVector );
		outerProduct->operandList->InsertBefore()->data = geometricProduct;

		if( count++ % 2 == 1 )
		{
			NumericScalar* numericScalar = new NumericScalar();
			numericScalar->SetReal( -1.0 );
			outerProduct->operandList->InsertBefore()->data = numericScalar;
		}

		if( prevNode )
			blade->operandList->InsertAfter( prevNode, node );
		else
			blade->operandList->InsertBefore( 0, node );

		sum->operandList->InsertAfter()->data = outerProduct;
	}

	return sum;
}

/*virtual*/ bool InnerProduct::IsAdditiveIdentity( void ) const
{
	return AnyOperandAdditiveIdentity();
}

// InnerProduct.cpp
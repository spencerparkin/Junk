// Expander.cpp

#include "Expander.h"
#include "Associator.h"
#include "Distributor.h"
#include "Compressor.h"
#include "InnerProduct.h"
#include "OuterProduct.h"
#include "GeometricProduct.h"
#include "Addition.h"
#include "Vector.h"
#include "NumericReal.h"
#include "Distributor.h"
#include "Compressor.h"
#include "GradeSelector.h"
#include <stdio.h>

using namespace GAExpressionLib;

Expander::Expander( void )
{
	expansionTarget = SUM_OF_BLADES;
}

/*virtual*/ Expander::~Expander( void )
{
}

/*virtual*/ bool Expander::ManipulateTree( Operand** operand )
{
	bool manipulatedTree = false;

	Associator associator;
	Distributor distributor;
	Compressor compressor;

	while( true )
	{
		int count;

		do
		{
			count = 0;

			if( associator.ManipulateTree( operand ) )
				count++;

			if( distributor.ManipulateTree( operand ) )
				count++;

			if( compressor.ManipulateTree( operand ) )
				count++;

			if( count > 0 )
				manipulatedTree = true;
		}
		while( count > 0 );

		if( ManipulateRecursively( operand ) )
		{
			manipulatedTree = true;
			continue;
		}

		break;
	}

	return manipulatedTree;
}

// a1^a2^...^aN = a1*(a2^...^aN) - a1.(a2^...^aN).
Operand* Expander::SplitBlade( const OuterProduct* blade )
{
	if( blade->operandList.Count() <= 1 )
		return nullptr;

	GeometricProduct* geometricProduct = new GeometricProduct();
	InnerProduct* innerProduct = new InnerProduct();
	OuterProduct* outerProduct = new OuterProduct();
	
	NumericReal* numericReal = new NumericReal();
	numericReal->real = -1.0;
	innerProduct->operandList.InsertAfter()->data = numericReal;

	for( const OuterProduct::OperandList::Node* node = blade->operandList.Head(); node; node = node->Next() )
	{
		if( node != blade->operandList.Head() )
			outerProduct->operandList.InsertAfter()->data = node->data->Clone();
		else
		{
			geometricProduct->operandList.InsertAfter()->data = node->data->Clone();
			innerProduct->operandList.InsertAfter()->data = node->data->Clone();
		}	
	}

	geometricProduct->operandList.InsertAfter()->data = outerProduct;
	innerProduct->operandList.InsertAfter()->data = outerProduct->Clone();

	Addition* addition = new Addition();
	addition->operandList.InsertAfter()->data = geometricProduct;
	addition->operandList.InsertAfter()->data = innerProduct;

	return addition;
}

// a1*a2*...*aN = a1.(a2*...*aN) + a1^(a2*...*aN) if N even.
// a1*a2*...*aN = a1.(a2*...*aN) + a1^<a2*...*aN>_{>0} if N odd.
Operand* Expander::SplitVersor( const GeometricProduct* versor )
{
	if( versor->operandList.Count() <= 1 )
		return nullptr;

	GeometricProduct* geometricProduct = new GeometricProduct();
	InnerProduct* innerProduct = new InnerProduct();
	OuterProduct* outerProduct = new OuterProduct();

	for( const GeometricProduct::OperandList::Node* node = versor->operandList.Head(); node; node = node->Next() )
	{
		if( node != versor->operandList.Head() )
			geometricProduct->operandList.InsertAfter()->data = node->data->Clone();
		else
		{
			outerProduct->operandList.InsertAfter()->data = node->data->Clone();
			innerProduct->operandList.InsertAfter()->data = node->data->Clone();
		}
	}

	innerProduct->operandList.InsertAfter()->data = geometricProduct;

	if( versor->operandList.Count() % 2 == 0 )
		outerProduct->operandList.InsertAfter()->data = geometricProduct->Clone();
	else
	{
		GradeSelector* gradeSelector = new GradeSelector();
		gradeSelector->type = GradeSelector::TYPE_EXCLUSIVE;
		gradeSelector->grade = 0;
		gradeSelector->operand = geometricProduct->Clone();

		outerProduct->operandList.InsertAfter()->data = gradeSelector;
	}
	
	Addition* addition = new Addition();
	addition->operandList.InsertAfter()->data = innerProduct;
	addition->operandList.InsertAfter()->data = outerProduct;
	return addition;
}

/*virtual*/ bool Expander::Manipulate( ManipulationData& data )
{
	GradeSelector* gradeSelector = dynamic_cast< GradeSelector* >( data.oldOperand );
	if( gradeSelector )
	{
		( void )gradeSelector->Select();
		data.newOperand = gradeSelector->operand;
		gradeSelector->operand = nullptr;
		return true;
	}

	GeometricProduct* geometricProduct = dynamic_cast< GeometricProduct* >( data.oldOperand );
	if( geometricProduct )
	{
		switch( expansionTarget )
		{
			case SUM_OF_BLADES:
			{
				if( geometricProduct->IsHomogeneousOfVectors() )
				{
					data.newOperand = SplitVersor( geometricProduct );
					if( data.newOperand )
						return true;
				}
				else if( !geometricProduct->HasAScalar() )
				{
					bool split = false;

					Operation::OperandList::Node* node = geometricProduct->operandList.Head();
					while( node )
					{
						OuterProduct* outerProduct = dynamic_cast< OuterProduct* >( node->data );
						if( outerProduct && outerProduct->IsHomogeneousOfVectors() && outerProduct->operandList.Count() > 1 )
						{
							node->data = SplitBlade( outerProduct );
							split = true;
						}

						node = node->Next();
					}

					return split;
				}

				break;
			}
		}
	}

	OuterProduct* outerProduct = dynamic_cast< OuterProduct* >( data.oldOperand );
	if( outerProduct )
	{
		switch( expansionTarget )
		{
			case SUM_OF_VERSORS:
			{
				if( outerProduct->IsHomogeneousOfVectors() )
				{
					data.newOperand = SplitBlade( outerProduct );
					if( data.newOperand )
						return true;
				}
				else if( !outerProduct->HasAScalar() )
				{
					bool split = false;

					Operation::OperandList::Node* node = outerProduct->operandList.Head();
					while( node )
					{
						GeometricProduct* geometricProduct = dynamic_cast< GeometricProduct* >( node->data );
						if( geometricProduct && geometricProduct->IsHomogeneousOfVectors() && geometricProduct->operandList.Count() > 1 )
						{
							node->data = SplitVersor( geometricProduct );
							split = true;
						}

						node = node->Next();
					}

					return split;
				}

				break;
			}
		}
	}

	InnerProduct* innerProduct = dynamic_cast< InnerProduct* >( data.oldOperand );
	if( innerProduct && innerProduct->operandList.Count() == 2 )
	{
		const Vector* leftVector = dynamic_cast< const Vector* >( innerProduct->operandList.Head()->data );
		const Vector* rightVector = dynamic_cast< const Vector* >( innerProduct->operandList.Tail()->data );

		if( leftVector && rightVector )
		{
			// We should never have to handle this case, because the compressor should already handle it.
			return false;
		}

		const OuterProduct* leftBlade = dynamic_cast< const OuterProduct* >( innerProduct->operandList.Head()->data );
		const OuterProduct* rightBlade = dynamic_cast< const OuterProduct* >( innerProduct->operandList.Tail()->data );

		if( leftVector && rightBlade && rightBlade->IsHomogeneousOfVectors() && rightBlade->operandList.Count() > 1 )
		{
			data.newOperand = GenerateInnerProductExpansion( leftVector, rightBlade, false );
			return true;
		}

		if( leftBlade && leftBlade->IsHomogeneousOfVectors() && leftBlade->operandList.Count() > 1 && rightVector )
		{
			data.newOperand = GenerateInnerProductExpansion( rightVector, leftBlade, true );
			return true;
		}

		if( leftBlade && leftBlade->IsHomogeneousOfVectors() && rightBlade && rightBlade->IsHomogeneousOfVectors() )
		{
			if( leftBlade->operandList.Count() > 1 && rightBlade->operandList.Count() > 1 )
			{
				InnerProduct* supInnerProduct = new InnerProduct();
				InnerProduct* subInnerProduct = new InnerProduct();

				OuterProduct* leftBladeClone = ( OuterProduct* )leftBlade->Clone();
				OuterProduct* rightBladeClone = ( OuterProduct* )rightBlade->Clone();

				if( leftBlade->operandList.Count() <= rightBlade->operandList.Count() )
				{
					OuterProduct::OperandList::Node* node = leftBladeClone->operandList.Tail();
					leftBladeClone->operandList.Remove( node, false );
					subInnerProduct->operandList.InsertAfter( nullptr, node );
					subInnerProduct->operandList.InsertAfter()->data = rightBladeClone;
					supInnerProduct->operandList.InsertAfter()->data = leftBladeClone;
					supInnerProduct->operandList.InsertAfter()->data = subInnerProduct;
				}
				else
				{
					OuterProduct::OperandList::Node* node = rightBladeClone->operandList.Head();
					rightBladeClone->operandList.Remove( node, false );
					subInnerProduct->operandList.InsertAfter()->data = leftBladeClone;
					subInnerProduct->operandList.InsertAfter( nullptr, node );
					supInnerProduct->operandList.InsertAfter()->data = subInnerProduct;
					supInnerProduct->operandList.InsertAfter()->data = rightBladeClone;
				}

				data.newOperand = supInnerProduct;
				return true;
			}
		}

		if( expansionTarget == SUM_OF_VERSORS )
		{
			GeometricProduct* leftVersor = dynamic_cast< GeometricProduct* >( innerProduct->operandList.Head()->data );
			GeometricProduct* rightVersor = dynamic_cast< GeometricProduct* >( innerProduct->operandList.Tail()->data );

			if( ( leftVector && rightVersor && rightVersor->IsHomogeneousOfVectors() && rightVersor->operandList.Count() > 1 ) ||
				( leftVersor && leftVersor->IsHomogeneousOfVectors() && leftVersor->operandList.Count() > 1 && rightVector ) )
			{
				// STPTODO: This is quite slow.  Is there a better way?  Can we find identities to handle these cases?
				//          For a long time I've investigated a possible identity that is very similar to the inner product
				//          expansion used above, but I either can't prove it or, more likely, it's not really true.

				data.newOperand = data.oldOperand;
				data.oldOperand = nullptr;

				Expander expander;
				expander.expansionTarget = SUM_OF_BLADES;
				expander.ManipulateTree( &data.newOperand );

				return true;
			}
		}
	}

	return false;
}

Addition* Expander::GenerateInnerProductExpansion( const Vector* vector, const OuterProduct* blade, bool commute )
{
	Addition* addition = new Addition();

	for( int i = 0; i < blade->operandList.Count(); i++ )
	{
		Operand* operand = nullptr;
		OuterProduct* outerProduct = new OuterProduct();

		int j = 0;
		const OuterProduct::OperandList::Node* node = blade->operandList.Head();
		while( node )
		{
			if( j == i )
				operand = ( Vector* )node->data->Clone();
			else
				outerProduct->operandList.InsertAfter()->data = node->data->Clone();

			node = node->Next();
			j++;
		}

		InnerProduct* innerProduct = new InnerProduct();
		innerProduct->operandList.InsertAfter()->data = vector->Clone();
		innerProduct->operandList.InsertAfter()->data = operand;

		j = i;
		if( commute )
			j += blade->operandList.Count() + 1;

		if( j % 2 == 1 )
		{
			NumericReal* numericReal = new NumericReal();
			numericReal->real = -1.0;
			innerProduct->operandList.InsertAfter()->data = numericReal;
		}

		GeometricProduct* geometricProduct = new GeometricProduct();
		geometricProduct->operandList.InsertAfter()->data = innerProduct;
		geometricProduct->operandList.InsertAfter()->data = outerProduct;

		addition->operandList.InsertAfter()->data = geometricProduct;
	}

	return addition;
}

// Expander.cpp
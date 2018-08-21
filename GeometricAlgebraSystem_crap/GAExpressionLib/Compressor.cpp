// Compressor.cpp

#include "Compressor.h"
#include "GeometricProduct.h"
#include "InnerProduct.h"
#include "OuterProduct.h"
#include "Addition.h"
#include "SymbolicReal.h"
#include "NumericReal.h"
#include "Vector.h"
#include "BilinearForm.h"
#include <string.h>

using namespace GAExpressionLib;

Compressor::Compressor( void )
{
}

/*virtual*/ Compressor::~Compressor( void )
{
}

/*virtual*/ bool Compressor::Manipulate( ManipulationData& data )
{
	bool manipulatedTree = false;

	InnerProduct* innerProduct = dynamic_cast< InnerProduct* >( data.oldOperand );
	if( innerProduct && innerProduct->operandList.Count() == 2 && innerProduct->IsHomogeneousOfVectors() )
	{
		ScaledSymbol* scaledSymbol = dynamic_cast< ScaledSymbol* >( innerProduct->operandList.Head()->data );
		if( scaledSymbol->scalar )
		{
			InnerProduct* nestedInnerProduct = dynamic_cast< InnerProduct* >( scaledSymbol->scalar );
			if( nestedInnerProduct && nestedInnerProduct->operandList.Count() == 2 && nestedInnerProduct->IsHomogeneousOfVectors() )
			{
				char innerProductBuffer[ 1024 ];
				char nestedInnerProductBuffer[ 1024 ];

				innerProduct->Print( innerProductBuffer, sizeof( innerProductBuffer ), Operand::PRINT_SORT );
				nestedInnerProduct->Print( nestedInnerProductBuffer, sizeof( nestedInnerProductBuffer ), Operand::PRINT_SORT );

				if( strcmp( nestedInnerProductBuffer, innerProductBuffer ) > 0 )
				{
					data.oldOperand = nullptr;
					data.newOperand = nestedInnerProduct;

					scaledSymbol->scalar = nullptr;
					scaledSymbol = dynamic_cast< ScaledSymbol* >( nestedInnerProduct->operandList.Head()->data );
					
					if( !scaledSymbol->scalar )
						scaledSymbol->scalar = innerProduct;
					else
					{
						GeometricProduct* geometricProduct = new GeometricProduct();
						geometricProduct->operandList.InsertAfter()->data = scaledSymbol->scalar;
						geometricProduct->operandList.InsertAfter()->data = innerProduct;

						scaledSymbol->scalar = geometricProduct;
					}

					return true;
				}
			}
		}
	}

	Operation* operation = dynamic_cast< Operation* >( data.oldOperand );
	if( operation )
	{
		if( operation->operandList.Count() == 0 )
		{
			data.newOperand = operation->Identity();
			manipulatedTree = true;
		}
		else if( operation->operandList.Count() == 1 )
		{
			Operation::OperandList::Node* node = operation->operandList.Head();
			data.newOperand = node->data;
			node->data = nullptr;
			manipulatedTree = true;
		}
		else
		{
			if( operation->Sort() )
				manipulatedTree = true;

			if( Compress( operation ) )
				manipulatedTree = true;
		}
	}

	ScaledSymbol* scaledSymbol = dynamic_cast< ScaledSymbol* >( data.oldOperand );
	if( scaledSymbol && scaledSymbol->scalar )
	{
		NumericReal* numericReal = dynamic_cast< NumericReal* >( scaledSymbol->scalar );
		if( numericReal )
		{
			if( numericReal->real == 0.0 )
			{
				data.newOperand = numericReal;
				scaledSymbol->scalar = nullptr;
				manipulatedTree = true;
			}
			else if( numericReal->real == 1.0 )
			{
				delete scaledSymbol->scalar;
				scaledSymbol->scalar = nullptr;
				manipulatedTree = true;
			}
		}
	}

	return manipulatedTree;
}

bool Compressor::Compress( Operation* operation )
{
	bool compressed = false;

	pairQueue.RemoveAll();

	if( operation->Commutes() )
	{
		for( Operation::OperandList::Node* leftNode = operation->operandList.Head(); leftNode; leftNode = leftNode->Next() )
			for( Operation::OperandList::Node* rightNode = leftNode->Next(); rightNode; rightNode = rightNode->Next() )
				pairQueue.InsertAfter()->data = Pair( leftNode, rightNode );
	}
	else
	{
		for( Operation::OperandList::Node* leftNode = operation->operandList.Head(); leftNode; leftNode = leftNode->Next() )
		{
			Operation::OperandList::Node* rightNode = leftNode->Next();
			if( rightNode )
				pairQueue.InsertAfter()->data = Pair( leftNode, rightNode );
		}
	}

	while( pairQueue.Count() > 0 )
	{
		PairQueue::Node* node = pairQueue.Head();
		Pair pair = node->data;
		pairQueue.Remove( node );

		Operand* resultOperand = nullptr;
		if( Combine( pair.leftNode->data, pair.rightNode->data, operation, &resultOperand ) )
		{
			compressed = true;

			Operation::OperandList::Node* newNode = nullptr;
			if( resultOperand )
			{
				newNode = operation->operandList.InsertAfter( pair.leftNode );
				newNode->data = resultOperand;
			}

			CullQueuedPairsContaining( pair.leftNode );
			CullQueuedPairsContaining( pair.rightNode );

			operation->operandList.Remove( pair.leftNode );
			operation->operandList.Remove( pair.rightNode );

			if( newNode )
			{
				if( operation->Commutes() )
				{
					for( Operation::OperandList::Node* node = operation->operandList.Head(); node; node = node->Next() )
						if( node != newNode )
							pairQueue.InsertAfter()->data = Pair( newNode, node );
				}
				else
				{
					if( newNode->Prev() )
						pairQueue.InsertAfter()->data = Pair( newNode->Prev(), newNode );
					if( newNode->Next() )
						pairQueue.InsertAfter()->data = Pair( newNode, newNode->Next() );
				}
			}
		}
	}

	return compressed;
}

void Compressor::CullQueuedPairsContaining( Operation::OperandList::Node* node )
{
	PairQueue::Node* pairNode = pairQueue.Head();
	while( pairNode )
	{
		PairQueue::Node* nextPairNode = pairNode->Next();

		if( pairNode->data.leftNode == node || pairNode->data.rightNode == node )
			pairQueue.Remove( pairNode );

		pairNode = nextPairNode;
	}
}

/*virtual*/ bool Compressor::Combine( const Operand* leftOperand, const Operand* rightOperand, const Operation* operation, Operand** resultOperand )
{
	return operation->Combine( leftOperand, rightOperand, resultOperand );
}

// Compressor.cpp
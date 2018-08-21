// BilinearForm.cpp

#include "BilinearForm.h"
#include "Vector.h"
#include "GeometricProduct.h"

namespace GAExpressionLib
{
	GA_EXPRESSION_LIB_API BilinearForm* bilinearForm = nullptr;
}

using namespace GAExpressionLib;

BilinearForm::BilinearForm( void )
{
}

/*virtual*/ BilinearForm::~BilinearForm( void )
{
}

Operand* BilinearForm::Evaluate( const Vector* leftVector, const Vector* rightVector ) const
{
	Operand* resultOperand = EvaluateInnerProduct( leftVector->name, rightVector->name );
	
	if( resultOperand && ( leftVector->scalar || rightVector->scalar ) )
	{
		GeometricProduct* geometricProduct = new GeometricProduct();
		geometricProduct->operandList.InsertAfter()->data = resultOperand;

		if( leftVector->scalar )
			geometricProduct->operandList.InsertAfter()->data = leftVector->scalar->Clone();

		if( rightVector->scalar )
			geometricProduct->operandList.InsertAfter()->data = rightVector->scalar->Clone();

		resultOperand = geometricProduct;
	}

	return resultOperand;
}

// BilinearForm.cpp
// BilinearForm.h

#pragma once

#include "GAExpLibApi.h"

namespace GAExpressionLib
{
	class BilinearForm;
	class Operand;
	class Vector;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::BilinearForm
{
public:

	BilinearForm( void );
	virtual ~BilinearForm( void );

	Operand* Evaluate( const Vector* leftVector, const Vector* rightVector ) const;

	virtual Operand* EvaluateInnerProduct( const char* leftVectorName, const char* rightVectorName ) const = 0;
};

namespace GAExpressionLib
{
	GA_EXPRESSION_LIB_API extern BilinearForm* bilinearForm;
}

// BilinearForm.h
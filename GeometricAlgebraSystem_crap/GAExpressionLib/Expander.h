// Expander.h

#pragma once

#include "Manipulator.h"
#include "Operation.h"

namespace GAExpressionLib
{
	class Expander;
	class OuterProduct;
	class GeometricProduct;
	class Addition;
	class Vector;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Expander : public GAExpressionLib::Manipulator
{
public:

	Expander( void );
	virtual ~Expander( void );

	virtual bool ManipulateTree( Operand** operand ) override;
	virtual bool Manipulate( ManipulationData& data ) override;

	enum ExpansionTarget { SUM_OF_BLADES, SUM_OF_VERSORS };
	ExpansionTarget expansionTarget;

private:

	Operand* SplitBlade( const OuterProduct* blade );
	Operand* SplitVersor( const GeometricProduct* versor );

	Addition* GenerateInnerProductExpansion( const Vector* vector, const OuterProduct* blade, bool commute );
};

// Expander.h
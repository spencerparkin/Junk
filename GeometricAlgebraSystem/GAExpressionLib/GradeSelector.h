// GradeSelector.h

#pragma once

#include "Operand.h"

namespace GAExpressionLib
{
	class GradeSelector;
	class Addition;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::GradeSelector : public GAExpressionLib::Operand
{
public:

	GradeSelector( void );
	virtual ~GradeSelector( void );

	// Overrides:
	virtual bool IsSameTypeAs( const Operand* operand ) const override;
	virtual Operand* Clone( void ) const override;
	virtual void Print( char* buffer, int bufferSize, PrintFormat format ) const override;

	bool Select( void );

	enum Type { TYPE_INCLUSIVE, TYPE_EXCLUSIVE };
	Type type;
	int grade;
	Operand* operand;
};

// GradeSelector.h
// Operation.h

#pragma once

class GACOMPUTE_API GACompute::Operation : public GACompute::Operand
{
public:

	Operation( void );
	virtual ~Operation( void );

	// Operand overrides:
	virtual void Print( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual Operand* Clone( void ) const override;
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsScalar( void ) const override;
	virtual void SortKey( char* buffer, int bufferSize ) const override;

	// Optional overrides:
	virtual Operand* Create( void ) const;
	virtual Operand* CreateVacuousCase( void ) const;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const;
	virtual bool CanDistributeOver( const Operation* operation ) const;
	virtual bool CanAssociateWith( const Operation* operation ) const;
	virtual int Sort( void );
	virtual int SortCompareOperands( const Operand* operandA, const Operand* operandB ) const;
	
	// Mandatory overrides:
	virtual void PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const = 0;
	virtual const char* OperatorSymbol( PrintFormat printFormat ) const = 0;
	//virtual bool IsProduct( void ) const = 0;
	//virtual bool IsSum( void ) const = 0;

	struct OperandCounts
	{
		int bladeCount;
		int versorCount;
		int scalarCount;
	};

	// Inherited functionality:
	Operand* Distribute( void );
	bool Associate( void );
	bool MarryScalars( void );
	GeometricProduct* StripScalars( void );
	Operand* MakeNonDegenerate( void );
	Vector* FindLeadingVector( void );
	bool AllOperandsAdditiveIdentity( void ) const;
	bool AnyOperandAdditiveIdentity( void ) const;
	static int SortCompare( const Operand* operandA, const Operand* operandB, void* data );
	void CountOperands( OperandCounts& operandCounts ) const;

	typedef List< Operand* > OperandList;
	OperandList* operandList;
};

// Operation.h
// BilinearForm.h

#pragma once

#include <lua.hpp>
#include "GACompute.h"

class BilinearFormTable : public GACompute::BilinearForm
{
public:

	BilinearFormTable( void );
	virtual ~BilinearFormTable( void );

	virtual bool Evaluate( const char* nameA, const char* nameB, double& result ) const override;

	virtual int TranslateName( const char* name ) const = 0;
	virtual double LookupTable( int i, int j ) const = 0;
};

class BilinearFormE3GA : public BilinearFormTable
{
public:

	BilinearFormE3GA( void );
	virtual ~BilinearFormE3GA( void );

	virtual int TranslateName( const char* name ) const override;
	virtual double LookupTable( int i, int j ) const override;
};

class BilinearFormC3GA : public BilinearFormTable
{
public:

	BilinearFormC3GA( void );
	virtual ~BilinearFormC3GA( void );

	virtual int TranslateName( const char* name ) const override;
	virtual double LookupTable( int i, int j ) const override;
};

class BilinearFormLua : public GACompute::BilinearForm
{
public:

	BilinearFormLua( lua_State* L );
	virtual ~BilinearFormLua( void );

	virtual bool Evaluate( const char* nameA, const char* nameB, double& result ) const override;

	static int registryKey;

	lua_State* L;
};

// BilinearForm.h
// BilinearForm.cpp

#include "BilinearForm.h"

BilinearFormTable::BilinearFormTable( void )
{
}

/*virtual*/ BilinearFormTable::~BilinearFormTable( void )
{
}

/*virtual*/ bool BilinearFormTable::Evaluate( const char* nameA, const char* nameB, double& result ) const
{
	int i = TranslateName( nameA );
	if( i == -1 )
		return false;

	int j = TranslateName( nameB );
	if( j == -1 )
		return false;

	result = LookupTable( i, j );
	return true;
}

BilinearFormE3GA::BilinearFormE3GA( void )
{
}

/*virtual*/ BilinearFormE3GA::~BilinearFormE3GA( void )
{
}

/*virtual*/ int BilinearFormE3GA::TranslateName( const char* name ) const
{
	if( 0 == strcmp( name, "e0" ) )
		return 0;
	if( 0 == strcmp( name, "e1" ) )
		return 1;
	if( 0 == strcmp( name, "e2" ) )
		return 2;
	if( 0 == strcmp( name, "e3" ) )
		return 3;
	return -1;
}

/*virtual*/ double BilinearFormE3GA::LookupTable( int i, int j ) const
{
	static double table[4][4] =
	{
		{ +1.0,  0.0,  0.0,  0.0 },
		{  0.0, +1.0,  0.0,  0.0 },
		{  0.0,  0.0, +1.0,  0.0 },
		{  0.0,  0.0,  0.0, +1.0 },
	};

	return table[i][j];
}

BilinearFormC3GA::BilinearFormC3GA( void )
{
}

/*virtual*/ BilinearFormC3GA::~BilinearFormC3GA( void )
{
}

/*virtual*/ int BilinearFormC3GA::TranslateName( const char* name ) const
{
	if( 0 == strcmp( name, "no" ) )
		return 0;
	if( 0 == strcmp( name, "e1" ) )
		return 1;
	if( 0 == strcmp( name, "e2" ) )
		return 2;
	if( 0 == strcmp( name, "e3" ) )
		return 3;
	if( 0 == strcmp( name, "ni" ) )
		return 4;
	return -1;
}

/*virtual*/ double BilinearFormC3GA::LookupTable( int i, int j ) const
{
	static double table[5][5] =
	{
		{  0.0,  0.0,  0.0,  0.0, -1.0 },
		{  0.0, +1.0,  0.0,  0.0,  0.0 },
		{  0.0,  0.0, +1.0,  0.0,  0.0 },
		{  0.0,  0.0,  0.0, +1.0,  0.0 },
		{ -1.0,  0.0,  0.0,  0.0,  0.0 },
	};

	return table[i][j];
}

int BilinearFormLua::registryKey = 0;

BilinearFormLua::BilinearFormLua( lua_State* L )
{
	this->L = L;
}

/*virtual*/ BilinearFormLua::~BilinearFormLua( void )
{
}

/*virtual*/ bool BilinearFormLua::Evaluate( const char* nameA, const char* nameB, double& result ) const
{
	bool known = false;

	try
	{
		lua_pushlightuserdata( L, ( void* )&BilinearFormLua::registryKey );
		lua_gettable( L, LUA_REGISTRYINDEX );

		if( !lua_isfunction( L, -1 ) )
			throw 1;

		lua_pushstring( L, nameA );
		lua_pushstring( L, nameB );

		int ret = lua_pcall( L, 2, 1, 0 );
		if( ret != 0 )
		{
			GACompute::Error* error = new GACompute::Error();
			error->Format( "Lua Error: %s", lua_tostring( L, -1 ) );
			GACompute::Error::Handle( error );
			throw 1;
		}

		if( !lua_isnumber( L, -1 ) )
			throw 1;

		result = lua_tonumber( L, -1 );
		known = true;
		throw 1;
	}
	catch( int popCount )
	{
		lua_pop( L, popCount );
	}

	return known;
}

// BilinearForm.cpp
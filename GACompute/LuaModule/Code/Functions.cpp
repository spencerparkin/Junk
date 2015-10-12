// Functions.cpp

#include "Functions.h"
#include "UserData.h"
#include "GACompute.h"
#include "BilinearForm.h"

int l_version( lua_State* L )
{
	lua_newtable( L );
	lua_pushinteger( L, GACOMPUTELUA_VERSION_MAJOR );
	lua_setfield( L, -2, "major" );
	lua_pushinteger( L, GACOMPUTELUA_VERSION_MINOR );
	lua_setfield( L, -2, "minor" );
	return 1;
}

int l_newscalar( lua_State* L )
{
	UserData* userData = l_newuserdata( L );
	if( !userData )
		return luaL_error( L, "Failed to create new user data." );

	if( lua_isnumber( L, -2 ) )
	{
		double realNumber = lua_tonumber( L, -2 );
		GACompute::NumericScalar* numericScalar = new GACompute::NumericScalar();
		numericScalar->SetReal( realNumber );
		userData->operand = numericScalar;
	}
	else if( lua_isstring( L, -2 ) )
	{
		const char* name = lua_tostring( L, -2 );
		GACompute::SymbolicScalar* symbolicScalar = new GACompute::SymbolicScalar();
		symbolicScalar->SetName( name );
		userData->operand = symbolicScalar;
	}
	else
	{
		lua_pop( L, 1 );
		lua_pushnil( L );
	}

	return 1;
}

int l_newvector( lua_State* L )
{
	UserData* userData = l_newuserdata( L );
	if( !userData )
		return luaL_error( L, "Failed to create new user data." );

	if( lua_isstring( L, -2 ) )
	{
		const char* name = lua_tostring( L, -2 );
		GACompute::Vector* vector = new GACompute::Vector();
		vector->SetName( name );
		userData->operand = vector;
	}
	else
	{
		lua_pop( L, 1 );
		lua_pushnil( L );
	}

	return 1;
}

int l_bilinear_form( lua_State* L )
{
	if( lua_isstring( L, -1 ) )
	{
		const char* string = lua_tostring( L, -1 );
		if( 0 == _stricmp( string, "e3ga" ) )
		{
			GACompute::BilinearForm::Set( new BilinearFormE3GA() );
			lua_pushboolean( L, 1 );
			return 1;
		}
		else if( 0 == _stricmp( string, "c3ga" ) )
		{
			GACompute::BilinearForm::Set( new BilinearFormC3GA() );
			lua_pushboolean( L, 1 );
			return 1;
		}
	}
	else if( lua_isfunction( L, -1 ) )
	{
		lua_pushlightuserdata( L, ( void* )&BilinearFormLua::registryKey );
		lua_pushvalue( L, -2 );
		lua_settable( L, LUA_REGISTRYINDEX );

		// The bilinear form cannot out-live the life of the module, so we can
		// feel safe about giving it a pointer to the Lua state.
		GACompute::BilinearForm::Set( new BilinearFormLua(L) );
		lua_pushboolean( L, 1 );
		return 1;
	}
	else if( lua_isnil( L, -1 ) )
	{
		GACompute::BilinearForm::Set(0);
		lua_pushboolean( L, 1 );
		return 1;
	}
	
	lua_pushboolean( L, 0 );
	return 1;
}

// Functions.cpp
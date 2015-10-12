// BilinearFormLua.cpp

#include "BilinearFormLua.h"
#include "Expression.h"

BilinearFormLua* bilinearFormLua = nullptr;

int BilinearFormLua::registryKey = 0;

BilinearFormLua::BilinearFormLua( lua_State* L )
{
	this->L = L;
}

/*virtual*/ BilinearFormLua::~BilinearFormLua( void )
{
}

/*virtual*/ GAExpressionLib::Operand* BilinearFormLua::EvaluateInnerProduct( const char* leftVectorName, const char* rightVectorName ) const
{
	lua_pushlightuserdata( L, ( void* )&registryKey );
	lua_gettable( L, LUA_REGISTRYINDEX );

	if( lua_isnil( L, -1 ) || !lua_isfunction( L, -1 ) )
	{
		lua_pop( L, 1 );
		return nullptr;
	}

	lua_pushstring( L, leftVectorName );
	lua_pushstring( L, rightVectorName );

	int result = lua_pcall( L, 2, 1, 0 );
	if( result != 0 )
	{
		const char* error = lua_tostring( L, -1 );

		lua_pop( L, 1 );
		return nullptr;
	}

	GAExpressionLib::Operand* operand = l_grab_real( L, -1 );

	lua_pop( L, 1 );
	return operand;
}

// BilinearFormLua.cpp
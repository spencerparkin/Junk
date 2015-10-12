// LuaIdioms.cpp

#include "LuaIdioms.h"

int l_getornewtable( lua_State* L, const char* key, bool global /*= false*/ )
{
	while( true )
	{
		if( global )
			lua_getglobal( L, key );
		else
		{
			lua_pushstring( L, key );
			lua_gettable( L, -2 );
		}

		if( lua_istable( L, -1 ) )
			break;

		lua_pop( L, 1 );
		lua_newtable( L );

		if( global )
			lua_setglobal( L, key );
		else
			lua_setfield( L, -2, key );
	}

	return 1;
}

LuaStackPopper::LuaStackPopper( lua_State* L )
{
	this->L = L;
	stackTop = lua_gettop(L);
}

LuaStackPopper::~LuaStackPopper( void )
{
	while( stackTop < lua_gettop(L) )
		lua_pop( L, 1 );
}

// LuaIdioms.cpp
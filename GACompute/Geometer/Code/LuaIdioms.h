// LuaIdioms.h

#pragma once

#include <lua.hpp>

int l_getornewtable( lua_State* L, const char* key, bool global = false );

// When lua does a PCall, it will clean up the stack after the call,
// but calls into lua that are not in a PCall will want to make sure
// that they don't leak data on the stack.
class LuaStackPopper
{
public:

	LuaStackPopper( lua_State* L );
	~LuaStackPopper( void );

	lua_State* L;
	int stackTop;
};

// LuaIdioms.h
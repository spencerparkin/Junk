// GAExpLuaApi.h

#pragma once

#if defined GA_EXPRESSION_LUA_API_EXPORT
#	define GA_EXPRESSION_LUA_API		__declspec( dllexport )
#elif defined GA_EXPRESSION_LUA_API_IMPORT
#	define GA_EXPRESSION_LUA_API		__declspec( dllimport )
#else
#	define GA_EXPRESSION_LUA_API
#endif

#include <lua.hpp>

extern "C"
{
	extern GA_EXPRESSION_LUA_API int luaopen_GAExpressionLua( lua_State* L );
}

int luaclose_GAExpressionLua( lua_State* L );

int l_new_scalar( lua_State* L );
int l_new_vector( lua_State* L );
int l_bilinear_form( lua_State* L );

// STPTODO: We might expose functions here to which Lua is none-the-wiser.

// GAExpLuaApi.h
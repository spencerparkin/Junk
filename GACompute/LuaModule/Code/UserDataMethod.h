// UserDataMethod.h

#pragma once

#include <lua.hpp>

int l_userdata_clone( lua_State* L );
int l_userdata_latex( lua_State* L );
int l_userdata_simplify( lua_State* L );
int l_userdata_simplify_debug( lua_State* L );
int l_userdata_reverse( lua_State* L );

// UserDataMethod.h
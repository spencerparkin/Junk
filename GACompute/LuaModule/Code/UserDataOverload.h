// UserDataOverload.h

#pragma once

#include <lua.hpp>

int l_userdata_add( lua_State* L );
int l_userdata_sub( lua_State* L );
int l_userdata_mul( lua_State* L );
int l_userdata_mod( lua_State* L );
int l_userdata_concat( lua_State* L );
int l_userdata_pow( lua_State* L );
int l_userdata_div( lua_State* L );
int l_userdata_index( lua_State* L );
int l_userdata_newindex( lua_State* L );
int l_userdata_len( lua_State* L );
int l_userdata_unm( lua_State* L );
int l_userdata_eq( lua_State* L );
int l_userdata_tostring( lua_State* L );

// UserDataOverload.h
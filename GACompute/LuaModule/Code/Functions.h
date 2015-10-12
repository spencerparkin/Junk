// BilinearForm.h

#pragma once

#include <lua.hpp>

#define GACOMPUTELUA_VERSION_MAJOR		0
#define GACOMPUTELUA_VERSION_MINOR		1

int l_version( lua_State* L );
int l_newscalar( lua_State* L );
int l_newvector( lua_State* L );
int l_bilinear_form( lua_State* L );

// BilinearForm.h
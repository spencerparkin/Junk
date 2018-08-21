// LuaInterpreter.h

#pragma once

#include <lua.hpp>
#include <string.h>

void l_execute( lua_State* L, const char* buf );

// LuaInterpreter.h
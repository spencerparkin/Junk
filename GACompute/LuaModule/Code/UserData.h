// UserData.h

#pragma once

#include <lua.hpp>
#include "GACompute.h"

struct UserData
{
	char magicKey[ 16 ];
	GACompute::Operand* operand;
};

UserData* l_newuserdata( lua_State* L );
UserData* l_grabuserdata( lua_State* L, int idx );
int l_deleteuserdata( lua_State* L );

// UserData.h
// Expression.h

#pragma once

#include <lua.hpp>
#include <GAExpLibApi.h>
#include <Operand.h>
#include <Manipulator.h>

struct Expression
{
	char magicKey[ 16 ];
	GAExpressionLib::Operand* operand;
};

Expression* l_new_exp( lua_State* L );
Expression* l_grab_exp( lua_State* L, int idx );
int l_del_exp( lua_State* L );
GAExpressionLib::Operand* l_grab_arg( lua_State* L, int idx );
GAExpressionLib::Operand* l_grab_real( lua_State* L, int idx );
int l_exp_manipulate( lua_State* L, int idx, GAExpressionLib::Manipulator* manipulator );

// Overloads:
int l_exp_add( lua_State* L );
int l_exp_sub( lua_State* L );
int l_exp_mul( lua_State* L );
int l_exp_mod( lua_State* L );
int l_exp_cat( lua_State* L );
int l_exp_pow( lua_State* L );
int l_exp_div( lua_State* L );
int l_exp_index( lua_State* L );
int l_exp_newindex( lua_State* L );
int l_exp_len( lua_State* L );
int l_exp_unm( lua_State* L );
int l_exp_eq( lua_State* L );
int l_exp_tostring( lua_State* L );

// Methods:
int l_exp_clone( lua_State* L );
int l_exp_associate( lua_State* L );
int l_exp_distribute( lua_State* L );
int l_exp_compress( lua_State* L );
int l_exp_expand( lua_State* L );
int l_exp_reverse( lua_State* L );

// Expression.h
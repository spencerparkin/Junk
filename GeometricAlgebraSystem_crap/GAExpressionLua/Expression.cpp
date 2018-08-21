// Expression.cpp

#include "Expression.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Operands:
#include <NumericReal.h>
#include <SymbolicReal.h>

// Operations:
#include <Addition.h>
#include <GeometricProduct.h>
#include <InnerProduct.h>
#include <OuterProduct.h>

// Manipulators:
#include <Distributor.h>
#include <Associator.h>
#include <Compressor.h>
#include <Expander.h>

static const char* metaKey = "gaexplua__metatable";
static const char* magicKey = "gaexplua__magic";

Expression* l_new_exp( lua_State* L )
{
	Expression* exp = ( Expression* )lua_newuserdata( L, sizeof( Expression ) );
	if( !exp )
		return nullptr;
	
	exp->operand = nullptr;
	strcpy_s( exp->magicKey, sizeof( exp->magicKey ), magicKey );

	luaL_getmetatable( L, metaKey );
	if( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		luaL_newmetatable( L, metaKey );

		lua_pushcfunction( L, &l_del_exp );
		lua_setfield( L, -2, "__gc" );

		lua_pushcfunction( L, &l_exp_add );
		lua_setfield( L, -2, "__add" );

		lua_pushcfunction( L, &l_exp_sub );
		lua_setfield( L, -2, "__sub" );

		lua_pushcfunction( L, &l_exp_mul );
		lua_setfield( L, -2, "__mul" );

		lua_pushcfunction( L, &l_exp_mod );
		lua_setfield( L, -2, "__mod" );

		lua_pushcfunction( L, &l_exp_cat );
		lua_setfield( L, -2, "__concat" );

		lua_pushcfunction( L, &l_exp_pow );
		lua_setfield( L, -2, "__pow" );

		lua_pushcfunction( L, &l_exp_div );
		lua_setfield( L, -2, "__div" );

		lua_pushcfunction( L, &l_exp_index );
		lua_setfield( L, -2, "__index" );

		lua_pushcfunction( L, &l_exp_newindex );
		lua_setfield( L, -2, "__newindex" );

		lua_pushcfunction( L, &l_exp_len );
		lua_setfield( L, -2, "__len" );

		lua_pushcfunction( L, &l_exp_unm );
		lua_setfield( L, -2, "__unm" );

		lua_pushcfunction( L, &l_exp_eq );
		lua_setfield( L, -2, "__eq" );

		lua_pushcfunction( L, &l_exp_tostring );
		lua_setfield( L, -2, "__tostring" );
	}

	lua_setmetatable( L, -2 );
	
	return exp;
}

Expression* l_grab_exp( lua_State* L, int idx )
{
	Expression* exp = nullptr;

	if( lua_isuserdata( L, idx ) )
	{
		exp = ( Expression* )lua_touserdata( L, idx );
		if( exp )
		{
			int len = strlen( magicKey );
			char buf[ 16 ];
			strncpy_s( buf, sizeof( buf ), exp->magicKey, len );
			buf[ len ] = '\0';
			if( 0 != strcmp( buf, magicKey ) )
				exp = nullptr;
		}
	}

	return exp;
}

int l_del_exp( lua_State* L )
{
	Expression* exp = l_grab_exp( L, -1 );
	if( exp )
	{
		delete exp->operand;
		exp->operand = nullptr;
	}

	return 0;
}

GAExpressionLib::Operand* l_grab_real( lua_State* L, int idx )
{
	using namespace GAExpressionLib;

	Operand* operand = nullptr;

	if( lua_isnumber( L, idx ) )
	{
		NumericReal* numericReal = new NumericReal();
		numericReal->real = lua_tonumber( L, idx );
		operand = numericReal;
	}
	else if( lua_isstring( L, idx ) )
	{
		SymbolicReal* symbolicReal = new SymbolicReal();
		symbolicReal->name = _strdup( lua_tostring( L, idx ) );
		operand = symbolicReal;
	}

	return operand;
}

GAExpressionLib::Operand* l_grab_arg( lua_State* L, int idx )
{
	using namespace GAExpressionLib;

	Expression* exp = l_grab_exp( L, idx );

	Operand* operand = nullptr;

	if( exp && exp->operand )
		operand = exp->operand->Clone();
	else
		operand = l_grab_real( L, idx );

	return operand;
}

int l_exp_add( lua_State* L )
{
	using namespace GAExpressionLib;

	Operand* leftOperand = l_grab_arg( L, -2 );
	Operand* rightOperand = l_grab_arg( L, -1 );

	if( !( leftOperand && rightOperand ) )
	{
		delete leftOperand;
		delete rightOperand;
		return 0;
	}

	Addition* addition = new Addition();
	addition->operandList.InsertAfter()->data = leftOperand;
	addition->operandList.InsertAfter()->data = rightOperand;

	Expression* exp = l_new_exp( L );
	exp->operand = addition;

	return 1;
}

int l_exp_sub( lua_State* L )
{
	using namespace GAExpressionLib;

	Operand* leftOperand = l_grab_arg( L, -2 );
	Operand* rightOperand = l_grab_arg( L, -1 );

	if( !( leftOperand && rightOperand ) )
	{
		delete leftOperand;
		delete rightOperand;
		return 0;
	}

	NumericReal* numericReal = new NumericReal();
	numericReal->real = -1.0;

	GeometricProduct* geometricProduct = new GeometricProduct();
	geometricProduct->operandList.InsertAfter()->data = numericReal;
	geometricProduct->operandList.InsertAfter()->data = rightOperand;

	Addition* addition = new Addition();
	addition->operandList.InsertAfter()->data = leftOperand;
	addition->operandList.InsertAfter()->data = geometricProduct;

	Expression* exp = l_new_exp( L );
	exp->operand = addition;

	return 1;
}

int l_exp_mul( lua_State* L )
{
	using namespace GAExpressionLib;

	Operand* leftOperand = l_grab_arg( L, -2 );
	Operand* rightOperand = l_grab_arg( L, -1 );

	if( !( leftOperand && rightOperand ) )
	{
		delete leftOperand;
		delete rightOperand;
		return 0;
	}

	GeometricProduct* geometricProduct = new GeometricProduct();
	geometricProduct->operandList.InsertAfter()->data = leftOperand;
	geometricProduct->operandList.InsertAfter()->data = rightOperand;

	Expression* exp = l_new_exp( L );
	exp->operand = geometricProduct;

	return 1;
}

int l_exp_mod( lua_State* L )
{
	return 0;
}

int l_exp_cat( lua_State* L )
{
	using namespace GAExpressionLib;

	Operand* leftOperand = l_grab_arg( L, -2 );
	Operand* rightOperand = l_grab_arg( L, -1 );

	if( !( leftOperand && rightOperand ) )
	{
		delete leftOperand;
		delete rightOperand;
		return 0;
	}

	InnerProduct* innerProduct = new InnerProduct();
	innerProduct->operandList.InsertAfter()->data = leftOperand;
	innerProduct->operandList.InsertAfter()->data = rightOperand;

	Expression* exp = l_new_exp( L );
	exp->operand = innerProduct;

	return 1;
}

int l_exp_pow( lua_State* L )
{
	using namespace GAExpressionLib;

	Operand* leftOperand = l_grab_arg( L, -2 );
	Operand* rightOperand = l_grab_arg( L, -1 );

	if( !( leftOperand && rightOperand ) )
	{
		delete leftOperand;
		delete rightOperand;
		return 0;
	}

	OuterProduct* outerProduct = new OuterProduct();
	outerProduct->operandList.InsertAfter()->data = leftOperand;
	outerProduct->operandList.InsertAfter()->data = rightOperand;

	Expression* exp = l_new_exp( L );
	exp->operand = outerProduct;

	return 1;
}

int l_exp_div( lua_State* L )
{
	return 0;
}

int l_exp_index( lua_State* L )
{
	if( lua_isstring( L, -1 ) )
	{
		const char* method = lua_tostring( L, -1 );

		if( 0 == strcmp( method, "clone" ) )
		{
			lua_pushcfunction( L, &l_exp_clone );
			return 1;
		}

		if( 0 == strcmp( method, "associate" ) )
		{
			lua_pushcfunction( L, &l_exp_associate );
			return 1;
		}

		if( 0 == strcmp( method, "distribute" ) )
		{
			lua_pushcfunction( L, &l_exp_distribute );
			return 1;
		}

		if( 0 == strcmp( method, "compress" ) )
		{
			lua_pushcfunction( L, &l_exp_compress );
			return 1;
		}

		if( 0 == strcmp( method, "expand" ) )
		{
			lua_pushcfunction( L, &l_exp_expand );
			return 1;
		}

		if( 0 == strcmp( method, "reverse" ) )
		{
			lua_pushcfunction( L, &l_exp_reverse );
			return 1;
		}
	}

	return 0;
}

int l_exp_newindex( lua_State* L )
{
	return 0;
}

int l_exp_len( lua_State* L )
{
	return 0;
}

int l_exp_unm( lua_State* L )
{
	return 0;
}

int l_exp_eq( lua_State* L )
{
	return 0;
}

int l_exp_tostring( lua_State* L )
{
	using namespace GAExpressionLib;

	Expression* exp = l_grab_exp( L, -1 );
	if( !( exp && exp->operand ) )
		lua_pushstring( L, "???" );
	else
	{
		char buffer[ 5*1024 ];
		exp->operand->Print( buffer, sizeof( buffer ), Operand::PRINT_NORMAL );
		lua_pushstring( L, buffer );
	}
	
	return 1;
}

int l_exp_clone( lua_State* L )
{
	using namespace GAExpressionLib;

	Expression* exp = l_grab_exp( L, -1 );
	if( !( exp && exp->operand ) )
		lua_pushnil( L );
	else
	{
		Expression* clone = l_new_exp( L );
		clone->operand = exp->operand->Clone();
	}

	return 1;
}

int l_exp_manipulate( lua_State* L, int idx, GAExpressionLib::Manipulator* manipulator )
{
	Expression* exp = l_grab_exp( L, idx );
	
	if( exp && exp->operand )
		manipulator->ManipulateTree( &exp->operand );

	return 1;
}

int l_exp_associate( lua_State* L )
{
	GAExpressionLib::Associator associator;
	return l_exp_manipulate( L, -1, &associator );
}

int l_exp_distribute( lua_State* L )
{
	GAExpressionLib::Distributor distributor;
	return l_exp_manipulate( L, -1, &distributor );
}

int l_exp_compress( lua_State* L )
{
	GAExpressionLib::Compressor compressor;
	return l_exp_manipulate( L, -1, &compressor );
}

int l_exp_expand( lua_State* L )
{
	if( !lua_isnumber( L, -1 ) )
		return 0;

	GAExpressionLib::Expander expander;
	expander.expansionTarget = ( GAExpressionLib::Expander::ExpansionTarget )lua_tointeger( L, -1 );
	return l_exp_manipulate( L, -2, &expander );
}

int l_exp_reverse( lua_State* L )
{
	return 0;
}

// Expression.cpp
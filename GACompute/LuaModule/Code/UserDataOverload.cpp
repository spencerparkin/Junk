// UserDataOverload.cpp

#include "UserDataOverload.h"
#include "UserDataMethod.h"
#include "UserData.h"

enum
{
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_MOD,
	OP_CAT,
	OP_POW,
	OP_DIV,
};

int l_userdata_op( lua_State* L, int op )
{
	GACompute::Operand* leftOperand = 0;
	GACompute::Operand* rightOperand = 0;

	const UserData* leftUserData = l_grabuserdata( L, -2 );
	const UserData* rightUserData = l_grabuserdata( L, -1 );

	if( leftUserData && leftUserData->operand )
		leftOperand = leftUserData->operand->Clone();
	else if( lua_isnumber( L, -2 ) )
	{
		GACompute::NumericScalar* numericScalar = new GACompute::NumericScalar();
		numericScalar->SetReal( lua_tonumber( L, -2 ) );
		leftOperand = numericScalar;
	}

	if( rightUserData && rightUserData->operand )
		rightOperand = rightUserData->operand->Clone();
	else if( lua_isnumber( L, -1 ) )
	{
		GACompute::NumericScalar* numericScalar = new GACompute::NumericScalar();
		numericScalar->SetReal( lua_tonumber( L, -1 ) );
		rightOperand = numericScalar;
	}

	AutoDelete< GACompute::Operand > autoDeleteLeftOperand( leftOperand );
	AutoDelete< GACompute::Operand > autoDeleteRightOperand( rightOperand );

	if( !( leftOperand && rightOperand ) )
		return 0;

	UserData* userData = l_newuserdata( L );
	if( !userData )
		return 0;

	switch( op )
	{
		case OP_ADD:
		{
			GACompute::Sum* sum = new GACompute::Sum();
			sum->operandList->InsertAfter()->data = leftOperand;
			sum->operandList->InsertAfter()->data = rightOperand;

			userData->operand = sum;

			autoDeleteLeftOperand.Release();
			autoDeleteRightOperand.Release();
			break;
		}
		case OP_SUB:
		{
			GACompute::NumericScalar* numericScalar = new GACompute::NumericScalar();
			numericScalar->SetReal( -1.0 );

			GACompute::GeometricProduct* geometricProduct = new GACompute::GeometricProduct();
			geometricProduct->operandList->InsertAfter()->data = numericScalar;
			geometricProduct->operandList->InsertAfter()->data = rightOperand;

			GACompute::Sum* sum = new GACompute::Sum();
			sum->operandList->InsertAfter()->data = leftOperand;
			sum->operandList->InsertAfter()->data = geometricProduct;

			userData->operand = sum;

			autoDeleteLeftOperand.Release();
			autoDeleteRightOperand.Release();
			break;
		}
		case OP_MUL:
		{
			GACompute::GeometricProduct* geometricProduct = new GACompute::GeometricProduct();
			geometricProduct->operandList->InsertAfter()->data = leftOperand;
			geometricProduct->operandList->InsertAfter()->data = rightOperand;

			userData->operand = geometricProduct;

			autoDeleteLeftOperand.Release();
			autoDeleteRightOperand.Release();
			break;
		}
		case OP_MOD:
		{
			//...
			break;
		}
		case OP_CAT:
		{
			GACompute::InnerProduct* innerProduct = new GACompute::InnerProduct();
			innerProduct->operandList->InsertAfter()->data = leftOperand;
			innerProduct->operandList->InsertAfter()->data = rightOperand;

			userData->operand = innerProduct;

			autoDeleteLeftOperand.Release();
			autoDeleteRightOperand.Release();
			break;
		}
		case OP_POW:
		{
			GACompute::OuterProduct* outerProduct = new GACompute::OuterProduct();
			outerProduct->operandList->InsertAfter()->data = leftOperand;
			outerProduct->operandList->InsertAfter()->data = rightOperand;

			userData->operand = outerProduct;

			autoDeleteLeftOperand.Release();
			autoDeleteRightOperand.Release();
			break;
		}
		case OP_DIV:
		{
			//...
			break;
		}
	}

	if( !userData->operand )
	{
		lua_pop( L, 1 );
		lua_pushnil( L );
	}

	return 1;
}

int l_userdata_add( lua_State* L )
{
	return l_userdata_op( L, OP_ADD );
}

int l_userdata_sub( lua_State* L )
{
	return l_userdata_op( L, OP_SUB );
}

int l_userdata_mul( lua_State* L )
{
	return l_userdata_op( L, OP_MUL );
}

int l_userdata_mod( lua_State* L )
{
	return l_userdata_op( L, OP_MOD );
}

int l_userdata_concat( lua_State* L )
{
	return l_userdata_op( L, OP_CAT );
}

int l_userdata_pow( lua_State* L )
{
	return l_userdata_op( L, OP_POW );
}

int l_userdata_div( lua_State* L )
{
	return l_userdata_op( L, OP_DIV );
}

int l_userdata_index( lua_State* L )
{
	if( lua_isstring( L, -1 ) )
	{
		const char* method = lua_tostring( L, -1 );

		if( 0 == strcmp( method, "clone" ) )
		{
			lua_pushcfunction( L, &l_userdata_clone );
			return 1;
		}

		if( 0 == strcmp( method, "latex" ) )
		{
			lua_pushcfunction( L, &l_userdata_latex );
			return 1;
		}

		if( 0 == strcmp( method, "simplify" ) )
		{
			lua_pushcfunction( L, &l_userdata_simplify );
			return 1;
		}

		if( 0 == strcmp( method, "simplify_debug" ) )
		{
			lua_pushcfunction( L, &l_userdata_simplify_debug );
			return 1;
		}

		if( 0 == strcmp( method, "reverse" ) )
		{
			lua_pushcfunction( L, &l_userdata_reverse );
			return 1;
		}
	}
	else if( lua_isnumber( L, -1 ) )
	{
		//...
	}

	return 0;
}

int l_userdata_newindex( lua_State* L )
{
	return 0;
}

int l_userdata_len( lua_State* L )
{
	return 0;
}

int l_userdata_unm( lua_State* L )
{
	return 0;
}

int l_userdata_eq( lua_State* L )
{
	return 0;
}

int l_userdata_tostring( lua_State* L )
{
	UserData* userData = l_grabuserdata( L, -1 );
	if( !userData )
		return luaL_error( L, "Expected user-data." );

	if( !userData->operand )
		lua_pushstring( L, "null" );
	else
	{
		char buffer[ 2048 ];
		userData->operand->Print( buffer, sizeof( buffer ), GACompute::Operand::PRINT_NORMAL );
		lua_pushstring( L, buffer );
	}

	return 1;
}

// UserDataOverload.cpp
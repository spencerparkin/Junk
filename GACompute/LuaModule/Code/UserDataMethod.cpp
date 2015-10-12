// UserDataMethod.cpp

#include "UserDataMethod.h"
#include "UserData.h"

int l_userdata_clone( lua_State* L )
{
	UserData* userData = l_grabuserdata( L, -1 );
	if( !userData || !userData->operand )
	{
		lua_pushnil( L );
		return 1;
	}

	UserData* userDataClone = l_newuserdata( L );
	if( !userDataClone )
	{
		lua_pushnil( L );
		return 1;
	}

	userDataClone->operand = userData->operand->Clone();
	return 1;
}

int l_userdata_latex( lua_State* L )
{
	UserData* userData = l_grabuserdata( L, -1 );
	if( !userData || !userData->operand )
	{
		lua_pushstring( L, "latex{\\mbox{null}}latex" );
		return 1;
	}

	char printBuffer[ 2048 ];
	userData->operand->Print( printBuffer, sizeof( printBuffer ), GACompute::Operand::PRINT_LATEX );

	char latexBuffer[ 2048 ];
	strcpy_s( latexBuffer, sizeof( latexBuffer ), "latex{" );
	strcat_s( latexBuffer, sizeof( latexBuffer ), printBuffer );
	strcat_s( latexBuffer, sizeof( latexBuffer ), "}latex" );
	lua_pushstring( L, latexBuffer );
	return 1;
}

int l_userdata_simplify( lua_State* L )
{
	UserData* userData = l_grabuserdata( L, -1 );
	if( !userData || !userData->operand )
		return 0;

	GACompute::Operand::SimplificationContext simplificationContext;
	simplificationContext.target = GACompute::Operand::SimplificationContext::SUM_OF_BLADES;
	simplificationContext.operation = 0;

	if( !GACompute::Operand::CompletelySimplify( userData->operand, simplificationContext ) )
	{
		//...
		return 0;
	}

	return 0;
}

int l_userdata_simplify_debug( lua_State* L )
{
	UserData* userData = l_grabuserdata( L, -1 );
	if( !userData || !userData->operand )
		return 0;

	GACompute::Operand::SimplificationContext simplificationContext;
	simplificationContext.target = GACompute::Operand::SimplificationContext::SUM_OF_BLADES;
	simplificationContext.operation = 0;

	bool result = GACompute::Operand::Simplify( userData->operand, simplificationContext );
	lua_pushboolean( L, ( result ? 1 : 0 ) );
	return 1;
}

int l_userdata_reverse( lua_State* L )
{
	// TODO: Simplify as sum of versors, then reverse!  :)
	//       If a result can be found about reversing blades, though, we may forgo that
	//       and just call the reverse algorithm itself with that knowledge.
	//       Some method should be given, though, that lets the user simplify as a sum of versors.

	return 0;
}

// UserDatamethod.cpp
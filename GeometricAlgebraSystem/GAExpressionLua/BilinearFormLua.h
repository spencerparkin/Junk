// BilinaerFormLua.h

#pragma once

#include "BilinearForm.h"
#include "Vector.h"
#include <lua.hpp>

class BilinearFormLua : public GAExpressionLib::BilinearForm
{
public:

	BilinearFormLua( lua_State* L );
	virtual ~BilinearFormLua( void );

	virtual GAExpressionLib::Operand* EvaluateInnerProduct( const char* leftVectorName, const char* rightVectorName ) const override;

	lua_State* L;

	static int registryKey;
};

extern BilinearFormLua* bilinearFormLua;

// BilinaerFormLua.h
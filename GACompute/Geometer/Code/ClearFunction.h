// ClearFunction.h

#ifndef __ClearFunction_h__
#define __ClearFunction_h__

#include "Function.h"

namespace Geometer
{
	class ClearFunction;
}

class Geometer::ClearFunction : public Geometer::Function
{
public:

	wxDECLARE_DYNAMIC_CLASS( ClearFunction );

	ClearFunction( void );
	virtual ~ClearFunction( void );

	virtual int Call( lua_State* L ) override;
	virtual wxString LuaFunctionName( void ) override;
};

#endif //__ClearFunction_h__

// ClearFunction.h
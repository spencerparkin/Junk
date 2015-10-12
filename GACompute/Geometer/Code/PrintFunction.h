// PrintFunction.h

#ifndef __PrintFunction_h__
#define __PrintFunction_h__

#include "Function.h"

namespace Geometer
{
	class PrintFunction;
}

class Geometer::PrintFunction : public Geometer::Function
{
public:

	wxDECLARE_DYNAMIC_CLASS( PrintFunction );

	PrintFunction( void );
	virtual ~PrintFunction( void );

	virtual int Call( lua_State* L ) override;
	virtual wxString LuaFunctionName( void ) override;
};

#endif //__PrintFunction_h__

// PrintFunction.h
// Exception.h

#pragma once

#include <wx/string.h>

class Exception
{
public:

	Exception( const wxString& error );
	virtual ~Exception( void );

	void Log( void );

	wxString error;
};

// Exception.h

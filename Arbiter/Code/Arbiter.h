// Arbiter.h

#pragma once

#include <wx/notebook.h>
#include <wx/xml/xml.h>

class Arbiter
{
public:

	Arbiter( void );
	virtual ~Arbiter( void );

	virtual void AddPageToNoteBook( wxNotebook* noteBook ) = 0;
	virtual bool CanHandleRequestType( const wxString& requestType ) = 0;
	virtual void HandleRequest( const wxXmlNode* xmlRequestNode, wxXmlNode* xmlResponseNode, const wxString& clientName ) = 0;
	virtual void DroppedClient( const wxString& clientName ) = 0;
	virtual void UpdateView( void ) = 0;
};

// Arbiter.h

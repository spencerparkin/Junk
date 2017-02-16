// Client.h

#pragma once

#include <wx/socket.h>
#include <wx/sckstrm.h>
#include <wx/xml/xml.h>

class Arbiter;

class Client
{
	friend class Thread;

public:

	Client( wxSocketBase* connectedSocket );
	virtual ~Client( void );

	bool Run( void );
	const wxString& GetName( void ) const { return name; }

	static const wxXmlNode* FindChildNode( const wxXmlNode* xmlParentNode, const wxString& name );
	static wxString GetContent( const wxXmlNode* xmlNode );

private:

	bool HandleRequest( const wxXmlDocument& xmlDocInput );
	Arbiter* FindArbiterForRequest( const wxString& requestType );

	wxString name;
	wxSocketBase* connectedSocket;
	wxSocketInputStream* socketInputStream;
};

// Client.h
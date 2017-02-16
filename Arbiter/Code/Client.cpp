// Client.cpp

#include "Client.h"
#include "Exception.h"
#include "Application.h"
#include "Server.h"
#include "Arbiter.h"
#include <wx/time.h>
#include <wx/utils.h>

Client::Client( wxSocketBase* connectedSocket )
{
	this->connectedSocket = connectedSocket;
	socketInputStream = new wxSocketInputStream( *connectedSocket );
	name = "<no name>";
}

/*virtual*/ Client::~Client( void )
{
	delete socketInputStream;
	delete connectedSocket;
}

bool Client::Run( void )
{
	if( !connectedSocket->WaitForRead(0) )
		return true;

	if( !connectedSocket->IsConnected() )
		return false;

	// I may need to write my own socket stream class, but let's try this...
	wxXmlDocument xmlDocInput;
	if( !xmlDocInput.Load( *socketInputStream ) )
		return true;

	try
	{
		if( !HandleRequest( xmlDocInput ) )
			return false;
	}
	catch( Exception* exception )
	{
		exception->Log();
		delete exception;
	}

	return true;
}

bool Client::HandleRequest( const wxXmlDocument& xmlDocInput )
{
	wxXmlDocument xmlDocOutput;
	wxXmlNode* xmlResponseNode = new wxXmlNode( wxXML_ELEMENT_NODE, "ArbiterResponse" );
	xmlDocOutput.SetRoot( xmlResponseNode );

	const wxXmlNode* xmlRequestNode = xmlDocInput.GetRoot();
	if( xmlRequestNode->GetName() != "ArbiterRequest" )
		xmlResponseNode->AddAttribute( "error", "Unrecognized XML packet: " + xmlRequestNode->GetName() );
	else
	{
		// We always pass the request ID through to the response.
		wxString requestId = xmlRequestNode->GetAttribute( "id" );
		xmlResponseNode->AddAttribute( "id", requestId );

		const wxXmlNode* xmlTypeNode = FindChildNode( xmlRequestNode, "Type" );
		if( !xmlTypeNode )
			xmlResponseNode->AddAttribute( "error", "No request type given." );
		else
		{
			wxString requestType = GetContent( xmlTypeNode );
			if( requestType == "IdentifyClient" )
				name = GetContent( FindChildNode( xmlRequestNode, "ClientName" ) );
			else
			{
				Arbiter* arbiter = FindArbiterForRequest( requestType );
				if( !arbiter )
					xmlResponseNode->AddAttribute( "error", "Unrecognized request type: " + requestType );
				else
					arbiter->HandleRequest( xmlRequestNode, xmlResponseNode, name );
			}
		}

		wxGetApp().GetServer()->updateView = true;
	}

	wxSocketOutputStream socketOutputStream( *connectedSocket );
	if( !xmlDocOutput.Save( socketOutputStream ) )
	{
	}

	return true;
}

Arbiter* Client::FindArbiterForRequest( const wxString& requestType )
{
	ArbiterList& arbiterList = wxGetApp().GetArbiterList();
	for( ArbiterList::iterator iter = arbiterList.begin(); iter != arbiterList.end(); iter++ )
	{
		Arbiter* arbiter = *iter;
		if( arbiter->CanHandleRequestType( requestType ) )
			return arbiter;
	}

	return nullptr;
}

/*static*/ const wxXmlNode* Client::FindChildNode( const wxXmlNode* xmlParentNode, const wxString& name )
{
	const wxXmlNode* xmlChildNode = xmlParentNode->GetChildren();
	while( xmlChildNode && xmlChildNode->GetName() != name )
		xmlChildNode = xmlChildNode->GetNext();

	if( !xmlChildNode )
		throw new Exception( "Failed to find XML node with name: " + name );

	return xmlChildNode;
}

/*static*/ wxString Client::GetContent( const wxXmlNode* xmlNode )
{
	wxString content;

	if( !xmlNode )
		throw new Exception( "Can't get content of null XML node." );

	if( xmlNode->GetType() == wxXML_TEXT_NODE )
		content = xmlNode->GetContent();
	else if( xmlNode->GetType() == wxXML_ELEMENT_NODE )
		content = GetContent( xmlNode->GetChildren() );

	return content;
}

// Client.cpp
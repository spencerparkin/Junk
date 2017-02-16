// Client.cpp

#include "Client.h"
#include "App.h"
#include "Frame.h"
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>

Client::Client( wxSocketBase* connectedSocket )
{
	this->connectedSocket = connectedSocket;
	socketInputStream = new wxSocketInputStream( *connectedSocket );
	socketOutputStream = new wxSocketOutputStream( *connectedSocket );
}

/*virtual*/ Client::~Client( void )
{
	delete socketInputStream;
	delete socketOutputStream;
	delete connectedSocket;
}

bool Client::IsConnected( void )
{
	return connectedSocket->IsConnected();
}

bool Client::Run( void )
{
	Frame* frame = wxGetApp().GetFrame();

	if( !connectedSocket->WaitForRead(0) )
		return true;

	if( !IsConnected() )
	{
		frame->AddLogMessage( wxString::Format( "Client 0x%016x no longer connected.", this ) );
		return false;
	}

	wxStringOutputStream stringOutputStream;
	socketInputStream->Read( stringOutputStream );

	wxString stringPacket = stringOutputStream.GetString();

	rapidjson::Document inputDoc;
	inputDoc.Parse( ( const char* )stringPacket.c_str() );
	if( !inputDoc.IsObject() )
		return false;

	if( !inputDoc.HasMember( "packetType" ) )
	{
		frame->AddLogMessage( "No packet type given." );
		return false;
	}

	wxString packetType = inputDoc[ "packetType" ].GetString();

	Handler* handler = nullptr;
	if( packetType == "Query" )
		handler = new QueryPackageHandler();
	else if( packetType == "Redeploy" )
		handler = new RedeployPackageHandler();

	if( !handler )
	{
		frame->AddLogMessage( "Can't handle packet type: " + packetType );
		return false;
	}

	rapidjson::Document outputDoc;
	if( !handler->Handle( inputDoc, outputDoc ) )
	{
		frame->AddLogMessage( "Failed to handle packet." );
		return false;
	}

	rapidjson::StringBuffer buffer;
	rapidjson::Writer< rapidjson::StringBuffer > writer( buffer );
	if( !outputDoc.Accept( writer ) )
	{
		frame->AddLogMessage( "Failed to write json packet string." );
		return false;
	}

	stringPacket = writer.GetString();

	wxStringInputStream stringInputStream( stringPacket );
	socketOutputStream->Write( stringInputStream );

	return true;
}

/*virtual*/ bool Client::QueryPackageHandler::Handle( const rapidjson::Document& inputDoc, rapidjson::Document& outputDoc )
{
	Frame* frame = wxGetApp().GetFrame();

	if( !inputDoc.HasMember( "packageName" ) )
	{
		frame->AddLogMessage( "No package name given." );
		return false;
	}

	if( !inputDoc.HasMember( "currentVersion" ) )
	{
		frame->AddLogMessage( "No current version given." );
		return false;
	}

	wxString packageName = inputDoc[ "packageName" ].GetString();
	int currentVersion = inputDoc[ "version" ].GetInt();

	wxString packageServerHost = "http://utwarhammer:9000/";

	wxHTTP http;
	http.SetHeader( "content-type", "blah" );

	if( !http.Connect( packageServerHost ) )
	{
		frame->AddLogMessage( "Failed to connect to package server." );
		return false;
	}

	wxInputStream* httpInputStream = http.GetInputStream( "packageVersion?package=" + packageName + wxString::Format( "&version=%d", currentVersion ) );



	wxDELETE( httpInputStream );

	http.Close();

	return true;
}

/*virtual*/ bool Client::RedeployPackageHandler::Handle( const rapidjson::Document& inputDoc, rapidjson::Document& outputDoc )
{
	Frame* frame = wxGetApp().GetFrame();

	return false;
}

// Client.cpp
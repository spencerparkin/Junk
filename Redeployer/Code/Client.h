// Client.h

#pragma once

#include <wx/socket.h>
#include <wx/sckstrm.h>
#include <rapidjson/document.h>

class Client
{
public:

	Client( wxSocketBase* connectedSocket );
	virtual ~Client( void );

	bool IsConnected( void );
	bool Run( void );

private:

	class Handler
	{
	public:
		virtual bool Handle( const rapidjson::Document& inputDoc, rapidjson::Document& outputDoc ) = 0;
	};

	class QueryPackageHandler : public Handler
	{
	public:
		virtual bool Handle( const rapidjson::Document& inputDoc, rapidjson::Document& outputDoc ) override;
	};

	class RedeployPackageHandler : public Handler
	{
	public:
		virtual bool Handle( const rapidjson::Document& inputDoc, rapidjson::Document& outputDoc ) override;
	};

	wxSocketBase* connectedSocket;
	wxSocketInputStream* socketInputStream;
	wxSocketOutputStream* socketOutputStream;
};

// Client.h

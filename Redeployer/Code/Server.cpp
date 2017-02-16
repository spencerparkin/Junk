// Server.cpp

#include "Server.h"
#include "Client.h"
#include "App.h"
#include "Frame.h"

Server::Server( void )
{
	socketServer = nullptr;
	port = 9876;
}

/*virtual*/ Server::~Server( void )
{
	delete socketServer;

	while( clientList.size() > 0 )
	{
		ClientList::iterator iter = clientList.begin();
		Client* client = *iter;
		delete client;
		clientList.erase( iter );
	}
}

bool Server::Run( void )
{
	Frame* frame = wxGetApp().GetFrame();

	if( !socketServer )
	{
		wxIPV4address address;
		address.LocalHost();
		address.Service( port );

		socketServer = new wxSocketServer( address, wxSOCKET_NOWAIT );
		if( socketServer->Initialize() )
			frame->AddLogMessage( wxString::Format( "Server running on port %d.", port ) );
		else
		{
			frame->AddLogMessage( wxString::Format( "Failed to start server on port %d.", port ) );

			delete socketServer;
			socketServer = nullptr;
			return false;
		}
	}

	wxSocketBase* connectedSocket = socketServer->Accept( false );
	if( connectedSocket )
	{
		Client* client = new Client( connectedSocket );
		clientList.push_back( client );

		frame->AddLogMessage( wxString::Format( "Connected with client: 0x%016x.", client ) );
	}

	ClientList::iterator iter = clientList.begin();
	while( iter != clientList.end() )
	{
		ClientList::iterator nextIter = iter;
		nextIter++;

		Client* client = *iter;
		if( !client->IsConnected() )
		{
			frame->AddLogMessage( wxString::Format( "Dropped client: 0x%016x.", client ) );

			delete client;
			clientList.erase( iter );
		}
		else if( !client->Run() )
			frame->AddLogMessage( wxString::Format( "Run failure for client: 0x%016x.", client ) );

		iter = nextIter;
	}

	return true;
}

// Server.cpp
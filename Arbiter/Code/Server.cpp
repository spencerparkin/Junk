// Server.cpp

#include "Server.h"
#include "Client.h"
#include "Application.h"
#include "Frame.h"
#include "Arbiter.h"
#include <wx/dataview.h>

Server::Server( void )
{
	socketServer = nullptr;
	updateView = false;
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
	if( !socketServer )
	{
		wxIPV4address address;
		address.LocalHost();
		address.Service( 9000 );

		socketServer = new wxSocketServer( address, wxSOCKET_NOWAIT );
		if( !socketServer->Initialize() )
		{
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
	}

	// Note that one of the main features of this entire program is
	// that one and only one client can be served at any given time!
	ClientList::iterator iter = clientList.begin();
	while( iter != clientList.end() )
	{
		ClientList::iterator nextIter = iter;
		nextIter++;

		Client* client = *iter;
		if( !client->Run() )
		{
			ArbiterList& arbiterList = wxGetApp().GetArbiterList();
			for( ArbiterList::iterator arbIter = arbiterList.begin(); arbIter != arbiterList.end(); arbIter++ )
			{
				Arbiter* arbiter = *arbIter;
				arbiter->DroppedClient( client->GetName() );
			}

			delete client;
			clientList.erase( iter );
			updateView = true;
		}

		iter = nextIter;
	}

	if( updateView )
	{
		ArbiterList& arbiterList = wxGetApp().GetArbiterList();
		for( ArbiterList::iterator arbIter = arbiterList.begin(); arbIter != arbiterList.end(); arbIter++ )
		{
			Arbiter* arbiter = *arbIter;
			arbiter->UpdateView();
		}

		wxGetApp().GetFrame()->GetClientDataView()->GetModel()->Cleared();

		updateView = false;
	}

	return true;
}

// Server.cpp
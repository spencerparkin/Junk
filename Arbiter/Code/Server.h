// Server.h

#pragma once

#include <list>
#include <wx/socket.h>

class Client;

typedef std::list< Client* > ClientList;

class Server
{
public:

	Server( void );
	virtual ~Server( void );

	bool Run( void );

	const ClientList& GetClientList( void ) { return clientList; }

	volatile bool updateView;

private:

	ClientList clientList;

	wxSocketServer* socketServer;
};

// Server.h
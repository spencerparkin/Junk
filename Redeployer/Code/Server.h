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

private:

	ClientList clientList;

	wxSocketServer* socketServer;
	unsigned short port;
};

// Server.h

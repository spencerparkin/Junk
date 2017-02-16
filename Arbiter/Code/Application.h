// Application.h

#pragma once

#include <wx/app.h>
#include <list>

class Frame;
class Server;
class Arbiter;

typedef std::list< Arbiter* > ArbiterList;

class Application : public wxApp
{
public:

	Application( void );
	virtual ~Application( void );

	virtual bool OnInit( void ) override;

	Frame* GetFrame( void ) { return frame; }
	Server* GetServer( void ) { return server; }
	ArbiterList& GetArbiterList( void ) { return arbiterList; }

private:

	Frame* frame;
	Server* server;
	ArbiterList arbiterList;
};

wxDECLARE_APP( Application );

// Application.h
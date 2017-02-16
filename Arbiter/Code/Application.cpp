// Application.cpp

#include "Application.h"
#include "Frame.h"
#include "Server.h"
#include "MutexArbiter.h"

wxIMPLEMENT_APP( Application );

Application::Application( void )
{
	frame = nullptr;

	server = new Server();
}

/*virtual*/ Application::~Application( void )
{
	while( arbiterList.size() > 0 )
	{
		ArbiterList::iterator iter = arbiterList.begin();
		Arbiter* arbiter = *iter;
		delete arbiter;
		arbiterList.erase( iter );
	}

	delete server;
}

/*virtual*/ bool Application::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	arbiterList.push_back( new MutexArbiter() );

	frame = new Frame();
	frame->Show();

	return true;
}

// Application.cpp
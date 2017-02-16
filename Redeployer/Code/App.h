// App.h

#pragma once

#include <wx/app.h>

class Frame;

class App : public wxApp
{
public:

	App( void );
	virtual ~App( void );

	virtual bool OnInit( void ) override;

	Frame* GetFrame( void ) { return frame; }

private:

	Frame* frame;
};

wxDECLARE_APP( App );

// App.h

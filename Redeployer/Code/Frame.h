// Frame.h

#pragma once

#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

class TaskBarIcon;
class Server;

class Frame : public wxFrame
{
public:

	Frame( wxWindow* parent );
	virtual ~Frame( void );

	void AddLogMessage( const wxString& logMessage );

private:

	void OnExit( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void OnTimer( wxTimerEvent& event );
	void OnHide( wxCommandEvent& event );

	wxTextCtrl* textCtrl;
	TaskBarIcon* taskBarIcon;
	Server* server;

	enum
	{
		ID_Exit = wxID_HIGHEST,
		ID_Timer,
		ID_Help,
		ID_Hide,
		ID_About,
	};

	wxTimer timer;
};

// Frame.h

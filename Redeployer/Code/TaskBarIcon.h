// TaskBarIcon.h

#pragma once

#include <wx/taskbar.h>
#include <wx/menu.h>
#include <wx/string.h>

class TaskBarIcon : public wxTaskBarIcon
{
public:

	TaskBarIcon( void );
	virtual ~TaskBarIcon( void );

	virtual wxMenu* CreatePopupMenu( void ) override;

	enum
	{
		ID_Exit = wxID_HIGHEST,
		ID_ToggleFrame,
	};

	void OnExit( wxCommandEvent& event );
	void OnToggleFrame( wxCommandEvent& event );
	void OnUpdateUI( wxUpdateUIEvent& event );
};

// TaskBarIcon.h

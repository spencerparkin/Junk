// Frame.h

#ifndef __Frame_h__
#define __Frame_h__

#include <wx/frame.h>
#include <wx/aui/aui.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/timer.h>

#include "Interface.h"

namespace Geometer
{
	class Frame;
}

class Geometer::Frame : public wxFrame
{
public:

	Frame( wxWindow* parent );
	virtual ~Frame( void );

	void OnAbout( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnIdle( wxIdleEvent& event );
	void OnToggleInterface( wxCommandEvent& event );
	void OnUpdateMenuItemUI( wxUpdateUIEvent& event );
	void OnTimer( wxTimerEvent& event );

private:

	void SaveFrameConfig( void );
	void RestoreFrameConfig( void );
	
	enum
	{
		ID_About = wxID_HIGHEST,
		ID_Exit,
		ID_InterfacePanelStart,		// This must be the last enum.
		ID_Timer,
	};

	void PopulateInterfaceMenu( void );

	bool AddInterfaceMenuItem( const Interface::RegistryEntry& entry );
	bool RemoveInterfaceMenuItem( const Interface::RegistryEntry& entry );

	bool CreateInterface( const Interface::RegistryEntry* entry );
	bool DestroyInterface( Interface* iface );

	int FindUnusedInterfaceMenuItemId( void );
	bool IsInterfaceMenuItemIdUsed( int menuItemId );

	Interface* FindInterfaceByMenuItemId( int menuItemId );
	const Interface::RegistryEntry* FindInterfaceRegistryEntryByMenuItemId( int menuItemId );
	const Interface::RegistryEntry* FindInterfaceRegistryEntryByClassName( const wxString& className );
	wxWindow* FindCenterPane( void );

	wxMenu* interfaceMenu;

	wxAuiManager* auiManager;

	wxTimer timer;
};

#endif //__Frame_h__

// Frame.h
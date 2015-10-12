// Frame.cpp

#include <wx/config.h>
#include <wx/tokenzr.h>
#include <wx/aboutdlg.h>
#include <wx/icon.h>

#include "Frame.h"

using namespace Geometer;

Frame::Frame( wxWindow* parent ) : wxFrame( parent, wxID_ANY, "Geometer" ), timer( this, ID_Timer )
{
	wxIcon logoIcon( wxICON( LuaLogoGAIcon ) );
	SetIcon( logoIcon );

	auiManager = new wxAuiManager( this, wxAUI_MGR_LIVE_RESIZE | wxAUI_MGR_DEFAULT );
	
	wxMenu* fileMenu = new wxMenu();
	wxMenuItem* exitMenuItem = new wxMenuItem( fileMenu, ID_Exit, "Exit", "Quit the application." );
	fileMenu->Append( exitMenuItem );

	interfaceMenu = new wxMenu();
	PopulateInterfaceMenu();

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About", "Show the about-box." );
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( fileMenu, "File" );
	menuBar->Append( interfaceMenu, "Interface" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	// STPTODO: There should be an option on the file menu to open and run a script.

	Bind( wxEVT_MENU, &Frame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &Frame::OnAbout, this, ID_About );
	Bind( wxEVT_IDLE, &Frame::OnIdle, this );
	Bind( wxEVT_TIMER, &Frame::OnTimer, this, ID_Timer );

	RestoreFrameConfig();

	auiManager->Update();

	timer.Start( 100 );
}

/*virtual*/ Frame::~Frame( void )
{
	auiManager->UnInit();
	delete auiManager;
}

void Frame::SaveFrameConfig( void )
{
	wxConfig* config = new wxConfig( "GeometerFrameConfig" );

	wxSize size = GetSize();
	config->Write( "FrameWidth", size.x );
	config->Write( "FrameHeight", size.y );

	wxPoint pos = GetPosition();
	config->Write( "FramePosX", pos.x );
	config->Write( "FramePosY", pos.y );

	wxString interfaceDockablePanes, interfaceCenterPane;
	wxAuiPaneInfoArray& paneInfoArray = auiManager->GetAllPanes();
	for( unsigned int i = 0; i < paneInfoArray.GetCount(); i++ )
	{
		Interface* iface = wxDynamicCast( paneInfoArray[i].window, Interface );
		if( iface )
		{
			wxClassInfo* classInfo = iface->GetClassInfo();
			wxString className = classInfo->GetClassName();
			if( paneInfoArray[i].dock_direction == wxAUI_DOCK_CENTER )
				interfaceCenterPane = className;
			else
				interfaceDockablePanes += className + "|";
		}
	}

	config->Write( "FrameInterfaceDockablePanes", interfaceDockablePanes );
	config->Write( "FrameInterfaceCenterPane", interfaceCenterPane );

	wxString perspective = auiManager->SavePerspective();
	config->Write( "FramePerspective", perspective );

	delete config;
}

void Frame::RestoreFrameConfig( void )
{
	wxConfig* config = new wxConfig( "GeometerFrameConfig" );

	wxSize size;
	config->Read( "FrameWidth", &size.x, 700 );
	config->Read( "FrameHeight", &size.y, 500 );
	SetSize( size );

	wxPoint pos;
	config->Read( "FramePosX", &pos.x, 100 );
	config->Read( "FramePosY", &pos.y, 100 );
	SetPosition( pos );

	wxString interfaceCenterPane;
	if( config->Read( "FrameInterfaceCenterPane", &interfaceCenterPane ) )
	{
		const Interface::RegistryEntry* entry = FindInterfaceRegistryEntryByClassName( interfaceCenterPane );
		if( entry )
			CreateInterface( entry );
	}

	wxString interfaceDockablePanes;
	if( config->Read( "FrameInterfaceDockablePanes", &interfaceDockablePanes ) )
	{
		wxStringTokenizer tokenizer( interfaceDockablePanes, "|" );
		while( tokenizer.HasMoreTokens() )
		{
			wxString className = tokenizer.GetNextToken();
			const Interface::RegistryEntry* entry = FindInterfaceRegistryEntryByClassName( className );
			if( entry )
				CreateInterface( entry );
		}
	}

	wxString perspective;
	if( config->Read( "FramePerspective", &perspective ) )
		auiManager->LoadPerspective( perspective );

	delete config;
}

void Frame::PopulateInterfaceMenu( void )
{
	const Interface::Registry& interfaceRegistry = Interface::GetRegistry();
	Interface::Registry::const_iterator iter = interfaceRegistry.begin();
	while( iter != interfaceRegistry.end() )
	{
		const Interface::RegistryEntry& entry = iter->second;
		AddInterfaceMenuItem( entry );
		iter++;
	}
}

bool Frame::AddInterfaceMenuItem( const Interface::RegistryEntry& entry )
{
	if( entry.interfaceMenuItemId != -1 )
		return false;

	entry.interfaceMenuItemId = FindUnusedInterfaceMenuItemId();

	wxMenuItem* menuItem = new wxMenuItem(
									interfaceMenu,
									entry.interfaceMenuItemId,
									entry.interfaceMenuItemName,
									entry.interfaceMenuItemHelp,
									wxITEM_CHECK );

	interfaceMenu->Append( menuItem );

	Bind( wxEVT_MENU, &Frame::OnToggleInterface, this, entry.interfaceMenuItemId );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, entry.interfaceMenuItemId );

	return true;
}

bool Frame::RemoveInterfaceMenuItem( const Interface::RegistryEntry& entry )
{
	wxMenuItem* menuItem = interfaceMenu->FindItem( entry.interfaceMenuItemId );
	if( !menuItem )
		return false;

	Unbind( wxEVT_MENU, &Frame::OnToggleInterface, this, entry.interfaceMenuItemId );
	Unbind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, entry.interfaceMenuItemId );

	interfaceMenu->Remove( menuItem );

	return true;
}

int Frame::FindUnusedInterfaceMenuItemId( void )
{
	int menuItemId = ID_InterfacePanelStart;
	while( IsInterfaceMenuItemIdUsed( menuItemId ) )
		menuItemId++;
	return menuItemId;
}

bool Frame::IsInterfaceMenuItemIdUsed( int menuItemId )
{
	const wxMenuItemList& itemsList = interfaceMenu->GetMenuItems();
	for( unsigned int i = 0; i < itemsList.size(); i++ )
	{
		const wxMenuItem* menuItem = itemsList[i];
		if( menuItemId == menuItem->GetId() )
			return true;
	}
	return false;
}

void Frame::OnAbout( wxCommandEvent& event )
{
	wxAboutDialogInfo aboutDlgInfo;

	aboutDlgInfo.SetName( "Geometer" );
	aboutDlgInfo.SetVersion( "1.0" );
	aboutDlgInfo.SetCopyright( "(C) 2015" );
	aboutDlgInfo.SetDescription(
				"A wxWidgets interface to Lua with GA supporting modules.\n"
				"Programmed by Spencer T. Parkin.\n"
				"Released under the MIT Licence.\n"
				"Report bugs to spencerparkin@outlook.com." );
	//aboutDlgInfo.AddDeveloper( "Spencer T. Parkin" );
	//aboutDlgInfo.SetLicence( "MIT Licence" );
	
#if 0
	aboutDlgInfo.SetIcon( GetIcon() );
#else
	wxBitmap logoBitmap( wxBITMAP( LuaLogoGABmp ) );
	wxIcon logoIcon;
	logoIcon.CopyFromBitmap( logoBitmap );
	aboutDlgInfo.SetIcon( logoIcon );
#endif

	wxAboutBox( aboutDlgInfo );
}

void Frame::OnExit( wxCommandEvent& event )
{
	SaveFrameConfig();

	Close( true );
}

void Frame::OnTimer( wxTimerEvent& event )
{
	static bool inHere = false;

	if( inHere )
		return;

	inHere = true;

	wxAuiPaneInfoArray& paneInfoArray = auiManager->GetAllPanes();
	for( unsigned int i = 0; i < paneInfoArray.GetCount(); i++ )
	{
		Interface* iface = wxDynamicCast( paneInfoArray[i].window, Interface );
		if( iface && iface->GetUpdateKey() != Interface::GetMasterUpdateKey() )
		{
			if( iface->UpdateControls() )
				iface->SetUpdateKey( Interface::GetMasterUpdateKey() );
		}
	}

	inHere = false;
}

void Frame::OnIdle( wxIdleEvent& event )
{
	// A wxTimerEvent is being used now instead of this idle-loop, because,
	// for whatever reason, requesting more idle events causes our CPU-usage
	// to go through the roof.
	//event.RequestMore();
}

void Frame::OnToggleInterface( wxCommandEvent& event )
{
	Interface* iface = FindInterfaceByMenuItemId( event.GetId() );
	if( iface )
		DestroyInterface( iface );
	else
	{
		const Interface::RegistryEntry* entry = FindInterfaceRegistryEntryByMenuItemId( event.GetId() );
		wxASSERT( entry != 0 );
		if( entry )
			CreateInterface( entry );
	}

	auiManager->Update();
}

// One possible weakness of our design here is that at most one
// instance of an interface can ever be instantiated.  It might be
// useful to have more than one instance of an interface in the overall UI.
bool Frame::CreateInterface( const Interface::RegistryEntry* entry )
{
	wxObjectConstructorFn constructorFunc = entry->classInfo->GetConstructor();
	Interface* iface = ( Interface* )constructorFunc();
	iface->Create( this );
	iface->CreateControls();

	wxAuiPaneInfo paneInfo;
	paneInfo.DestroyOnClose();
	paneInfo.BestSize( 200, 200 );
	paneInfo.Caption( entry->interfaceMenuItemName );
	paneInfo.Name( entry->classInfo->GetClassName() );
	if( !FindCenterPane() )
		paneInfo.CenterPane();
	else
		paneInfo.Dockable();

	auiManager->AddPane( iface, paneInfo );
	return true;
}

bool Frame::DestroyInterface( Interface* iface )
{
	auiManager->DetachPane( iface );
	delete iface;
	return true;
}

void Frame::OnUpdateMenuItemUI( wxUpdateUIEvent& event )
{
	if( event.GetId() >= ID_InterfacePanelStart )
	{
		Interface* iface = FindInterfaceByMenuItemId( event.GetId() );
		event.Check( iface ? true : false );
	}
}

Interface* Frame::FindInterfaceByMenuItemId( int menuItemId )
{
	Interface* foundInterface = 0;
	wxAuiPaneInfoArray& paneInfoArray = auiManager->GetAllPanes();
	for( unsigned int i = 0; i < paneInfoArray.GetCount() && !foundInterface; i++ )
	{
		Interface* iface = wxDynamicCast( paneInfoArray[i].window, Interface );
		wxClassInfo* classInfo = iface->GetClassInfo();
		wxString className = classInfo->GetClassName();
		const Interface::RegistryEntry* entry = FindInterfaceRegistryEntryByClassName( className );
		if( entry && entry->interfaceMenuItemId == menuItemId )
			foundInterface = iface;
	}
	return foundInterface;
}

const Interface::RegistryEntry* Frame::FindInterfaceRegistryEntryByMenuItemId( int menuItemId )
{
	const Interface::Registry& interfaceRegistry = Interface::GetRegistry();
	Interface::Registry::const_iterator iter = interfaceRegistry.begin();
	while( iter != interfaceRegistry.end() )
	{
		const Interface::RegistryEntry& entry = iter->second;
		if( entry.interfaceMenuItemId == menuItemId )
			return &entry;
		iter++;
	}
	return 0;
}

const Interface::RegistryEntry* Frame::FindInterfaceRegistryEntryByClassName( const wxString& className )
{
	const Interface::Registry& interfaceRegistry = Interface::GetRegistry();
	Interface::Registry::const_iterator iter = interfaceRegistry.find( className );
	if( iter == interfaceRegistry.end() )
		return 0;
	const Interface::RegistryEntry& entry = iter->second;
	return &entry;
}

wxWindow* Frame::FindCenterPane( void )
{
	wxAuiPaneInfoArray& paneInfoArray = auiManager->GetAllPanes();
	for( unsigned int i = 0; i < paneInfoArray.GetCount(); i++ )
	{
		wxAuiPaneInfo& paneInfo = paneInfoArray[i];
		if( paneInfo.dock_direction == wxAUI_DOCK_CENTER )
			return paneInfo.window;
	}
	return 0;
}

// Frame.cpp
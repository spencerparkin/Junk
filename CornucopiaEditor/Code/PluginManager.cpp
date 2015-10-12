// PluginManager.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
PluginManager::PluginManager( void )
{
}

//=================================================================================
PluginManager::~PluginManager( void )
{
	UnloadAllPlugins();
}

//=================================================================================
void PluginManager::LoadAllPlugins( void )
{
	// We look for plugins in the paths given by the following environment variable.
	char pluginPathsBuffer[ 512 ];
	if( !GetEnvironmentVariableA( "CORNUCOPIA_PLUGIN_PATH", pluginPathsBuffer, sizeof( pluginPathsBuffer ) ) )
		return;

	// Make a class to handle loading a given plugin file.
	class PluginLoader : public wxDirTraverser
	{
	public:
		PluginLoader( PluginManager* pluginManager ) { this->pluginManager = pluginManager; }
		virtual ~PluginLoader( void ) {}

		virtual wxDirTraverseResult OnFile( const wxString& fileName ) override
		{
			Cornucopia::Path libraryPath;
			libraryPath.SetPath( ( const char* )fileName.c_str() );
			pluginManager->LoadPlugin( libraryPath );
			return wxDIR_CONTINUE;
		}

		virtual wxDirTraverseResult OnDir( const wxString& dirName ) override
		{
			return wxDIR_CONTINUE;
		}

		virtual wxDirTraverseResult OnOpenError( const wxString& openErrorName ) override
		{
			return wxDIR_IGNORE;
		}

	private:
		PluginManager* pluginManager;
	};

	// Go try to load plugins in all the paths given in the found environment variable.
	boost::char_separator< char > separator( ";" );
	typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;
	std::string pluginPaths = pluginPathsBuffer;
	Tokenizer tokenizer( pluginPaths, separator );
	for( Tokenizer::iterator iter = tokenizer.begin(); iter != tokenizer.end(); iter++ )
	{
		std::string path = *iter;
		PluginLoader pluginLoader( this );
		wxDir dir( ( const char* )path.c_str() );
		dir.Traverse( pluginLoader, wxT( "*.cornucopiaplugin" ), wxDIR_FILES );
	}
}

//=================================================================================
void PluginManager::UnloadAllPlugins( void )
{
	while( pluginList.size() > 0 )
	{
		PluginList::iterator iter = pluginList.begin();
		UnloadPlugin( iter );
	}
}

//=================================================================================
// When the application exposes an entry-point for plugins, it does so
// by issuing an event of which each plugin, in turn, is given a chance
// to handle.  The order in which we give plugins a chance is left undefined.
// Processing stops at the first plugin to handle the event, which may mean
// that some plugins never get a chance to handle it.  In most cases, an
// event should be handled by all plugins.  To achieve this, all plugins
// should handle the event while pretending that they didn't so that the
// next plugin to be given a chance to handle it may do so too.  For the cases
// where just one plugin should handle the event, we may have a situation where
// two or more plugins are incompatible with one another.
bool PluginManager::CallAllPlugins( PluginEvent& event )
{
	for( PluginList::iterator iter = pluginList.begin(); iter != pluginList.end(); iter++ )
	{
		Plugin* plugin = *iter;
		if( CallPlugin( plugin, event ) )
			return true;
	}
	return false;
}

//=================================================================================
bool PluginManager::CallPlugin( Plugin* plugin, PluginEvent& event )
{
	CornucopiaEditor::Plugin* eventHandler = plugin->GetEventHandler();
	event.SetAssistant( &assistant );
	event.SetLibraryPath( ( const char* )plugin->GetLibraryPath() );
	return eventHandler->SafelyProcessEvent( event );
}

//=================================================================================
bool PluginManager::ProcessAllPlugins( PluginProcessor* pluginProcessor )
{
	for( PluginList::iterator iter = pluginList.begin(); iter != pluginList.end(); iter++ )
	{
		Plugin* plugin = *iter;
		if( !pluginProcessor->ProcessPlugin( plugin->GetEventHandler(), plugin->GetLibraryPath() ) )
			return false;
	}
	return true;
}

//=================================================================================
bool PluginManager::LoadPlugin( const Cornucopia::Path& libraryPath )
{
	bool success = false;
	Plugin* plugin = 0;

	do
	{
		// Make sure that the plugin is not already loaded.
		if( FindPlugin( libraryPath ) )
		{
			wxMessageBox( wxString::Format( wxT( "The plugin \"%s\" is already loaded." ), ( const char* )libraryPath ), wxT( "Error" ), wxOK | wxCENTRE );
			break;
		}

		// Create a new plugin object to represent the plugin in our list.
		plugin = new ( std::nothrow ) Plugin();
		if( !plugin )
			break;

		// Attempt to load the plugin library.
		if( !plugin->Load( libraryPath ) )
			break;

		// Attempt to initialize the plugin.
		CornucopiaEditor::Plugin* eventHandler = plugin->GetEventHandler();
		if( !eventHandler->Initialize( &assistant ) )
		{
			wxMessageBox( wxString::Format( wxT( "The plugin \"%s\" failed to initialize." ), ( const char* )libraryPath ), wxT( "Error" ), wxOK | wxCENTRE );
			break;
		}

		// Send the first event to the plugin.
		PluginEvent event;
		event.SetEventObject(0);
		event.SetEventType( CORNUCOPIA_PLUGIN_INITIALIZE );
		( void )CallPlugin( plugin, event );

		// We made it threw the gauntlet!
		success = true;
	}
	while( false );

	// Discard or keep the plugin.
	if( !success )
		delete plugin;
	else
		pluginList.push_back( plugin );

	return success;
}

//=================================================================================
bool PluginManager::UnloadPlugin( const Cornucopia::Path& libraryPath )
{
	PluginList::iterator iter;
	if( !FindPlugin( libraryPath, &iter ) )
		return false;

	return UnloadPlugin( iter );
}

//=================================================================================
bool PluginManager::UnloadPluginLater( const Cornucopia::Path& libraryPath )
{
	if( !FindPlugin( libraryPath ) )
		return false;

	pendingUnloadList.push_back( ( const char* )libraryPath );
	return true;
}

//=================================================================================
bool PluginManager::UnloadPlugin( PluginList::iterator& iter )
{
	// Remove the plugin from our list.
	Plugin* plugin = *iter;
	pluginList.erase( iter );

	// Send the last event to the plugin.
	PluginEvent event;
	event.SetEventObject(0);
	event.SetEventType( CORNUCOPIA_PLUGIN_FINALIZE );
	( void )CallPlugin( plugin, event );

	// We can now dispose of the plugin.
	plugin->GetEventHandler()->Finalize( &assistant );
	plugin->Unload();
	delete plugin;
	return true;
}

//=================================================================================
void PluginManager::DoIdleProcessing( void )
{
	bool unloadedOneOrMorePlugins = false;
	while( pendingUnloadList.Count() > 0 )
	{
		wxArrayString::iterator iter = pendingUnloadList.begin();
		Cornucopia::Path libraryPath( ( const char* )( *iter ).c_str() );
		if( UnloadPlugin( libraryPath ) )
			unloadedOneOrMorePlugins = true;
		pendingUnloadList.erase( iter );
	}

	if( unloadedOneOrMorePlugins )
	{
		PluginPanel* pluginPanel = wxGetApp().GetFrame()->GetPluginPanel();
		if( pluginPanel )
			pluginPanel->UpdateList();
	}
}

//=================================================================================
CornucopiaEditor::PluginAssistant* PluginManager::GetAssistant( void )
{
	return &assistant;
}

//=================================================================================
// Do a linear search for a plugin by library path.
bool PluginManager::FindPlugin( const Cornucopia::Path& libraryPath, PluginList::iterator* iter /*= 0*/ )
{
	PluginList::iterator localIter;
	if( !iter )
		iter = &localIter;

	for( *iter = pluginList.begin(); *iter != pluginList.end(); ( *iter )++ )
	{
		Plugin* plugin = **iter;
		if( plugin->GetLibraryPath() == libraryPath )
			break;
	}

	if( *iter == pluginList.end() )
		return false;

	return true;
}

//=================================================================================
PluginManager::Plugin::Plugin( void )
{
	libraryHandle = NULL;
	eventHandler = 0;
}

//=================================================================================
PluginManager::Plugin::~Plugin( void )
{
	Unload();
}

//=================================================================================
bool PluginManager::Plugin::Load( const std::string& libraryPath )
{
	bool success = false;
	libraryHandle = NULL;

	do
	{
		libraryHandle = LoadLibraryA( ( const char* )libraryPath.c_str() );
		if( libraryHandle == NULL )
		{
			DWORD error = GetLastError();
			wxMessageBox( wxString::Format( "Failed to load plugin \"%s\" (error: %d).", ( const char* )libraryPath.c_str(), error ), wxT( "Error" ), wxOK | wxCENTRE );
			break;
		}
	
		CreatePluginFunc createPlugin = ( CreatePluginFunc )GetProcAddress( libraryHandle, "CreatePlugin" );
		if( !createPlugin )
		{
			DWORD error = GetLastError();
			wxMessageBox( wxString::Format( "Failed to find process address for function \"CreatePlugin\" in plugin \"%s\" (error: %d).", ( const char* )libraryPath.c_str(), error ), wxT( "Error" ), wxOK | wxCENTRE );
			break;
		}

		eventHandler = createPlugin();
		if( !eventHandler )
		{
			wxMessageBox( wxString::Format( "Failed to create plugin event handler for plugin \"%s\".", ( const char* )libraryPath.c_str() ), wxT( "Error" ), wxOK | wxCENTRE );
			break;
		}

		this->libraryPath = libraryPath;
		success = true;
	}
	while( false );

	if( !success )
		Unload();

	return success;
}

//=================================================================================
bool PluginManager::Plugin::Unload( void )
{
	// We must delete the event handler before unloading the destructor code.
	if( eventHandler )
	{
		delete eventHandler;
		eventHandler = 0;
	}

	if( libraryHandle != NULL )
	{
		FreeLibrary( libraryHandle );
		libraryHandle = NULL;
	}

	libraryPath.SetPath( "" );
	return true;
}

//=================================================================================
CornucopiaEditor::Plugin* PluginManager::Plugin::GetEventHandler( void )
{
	return eventHandler;
}

//=================================================================================
const Cornucopia::Path& PluginManager::Plugin::GetLibraryPath( void ) const
{
	return libraryPath;
}

//=================================================================================
/*virtual*/ wxFrame* PluginManager::PluginAssistant::GetFrame( void )
{
	return wxGetApp().GetFrame();
}

//=================================================================================
/*virtual*/ wxAuiManager* PluginManager::PluginAssistant::GetFrameAuiManager( void )
{
	return wxGetApp().GetFrame()->GetAuiManager();
}

//=================================================================================
/*virtual*/ wxAuiNotebook* PluginManager::PluginAssistant::GetAuiNoteBook( void )
{
	return wxGetApp().GetFrame()->GetNoteBook();
}

//=================================================================================
/*virtual*/ bool PluginManager::PluginAssistant::IsDataViewCtrlForTree( wxDataViewCtrl* dataViewCtrl )
{
	if( wxDynamicCast( dataViewCtrl, DataViewCtrl ) )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ int PluginManager::PluginAssistant::GetUnusedModelColumn( wxDataViewCtrl* dataViewCtrl )
{
	DataViewCtrl* dataViewCtrlForTree = wxDynamicCast( dataViewCtrl, DataViewCtrl );
	if( !dataViewCtrlForTree )
		return -1;
	return dataViewCtrlForTree->GetUnusedModelColumn();
}

//=================================================================================
/*virtual*/ wxWindow* PluginManager::PluginAssistant::GetCurrentPage( void )
{
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	wxWindow* page = noteBook->GetCurrentPage();
	return page;
}

//=================================================================================
/*virtual*/ Cornucopia::Node* PluginManager::PluginAssistant::GetCurrentTree( void )
{
	Cornucopia::Node* root = 0;

	wxWindow* page = GetCurrentPage();
	Panel* panel = wxDynamicCast( page, Panel );
	if( panel )
	{
		Tree* tree = panel->GetTree();
		root = tree->GetRoot();
	}

	return root;
}

//=================================================================================
/*virtual*/ wxString PluginManager::PluginAssistant::GetCurrentFile( void )
{
	wxString filePath;

	wxWindow* page = GetCurrentPage();
	Panel* panel = wxDynamicCast( page, Panel );
	if( panel )
	{
		Tree* tree = panel->GetTree();
		filePath = tree->GetFilePath();
	}

	return filePath;
}

//=================================================================================
/*virtual*/ Cornucopia::Node* PluginManager::PluginAssistant::GetTreeFromPage( wxWindow* page )
{
	Cornucopia::Node* root = 0;

	Panel* panel = wxDynamicCast( page, Panel );
	if( panel )
	{
		Tree* tree = panel->GetTree();
		root = tree->GetRoot();
	}

	return root;
}

//=================================================================================
/*virtual*/ wxString PluginManager::PluginAssistant::GetFileFromPage( wxWindow* page )
{
	wxString filePath = wxEmptyString;

	Panel* panel = wxDynamicCast( page, Panel );
	if( panel )
	{
		Tree* tree = panel->GetTree();
		filePath = tree->GetFilePath();
	}

	return filePath;
}

//=================================================================================
/*virtual*/ bool PluginManager::PluginAssistant::RequestPageReload( wxWindow* page, const wxString& reason )
{
	Panel* panel = wxDynamicCast( page, Panel );
	if( !panel )
		return false;

	Tree* tree = panel->GetTree();
	wxString filePath = tree->GetFilePath();

	wxString prompt = "A plugin wishes to reload the file \"" + filePath + "\" for the following reason.\n\n";
	prompt += reason + "\n\n";
	prompt += "Would you like to reload the file?";

	return wxGetApp().GetFrame()->ReopenTree( panel, prompt );
}

//=================================================================================
// It's important to note that we can't just unload the plugin here and now,
// because the plugin code is still running the call-stack.
/*virtual*/ bool PluginManager::PluginAssistant::UnloadPlugin( const wxString& libraryPath )
{
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	Cornucopia::Path path( ( const char* )libraryPath.c_str() );
	if( pluginManager && pluginManager->UnloadPluginLater( path ) )
		return true;
	return false;
}

// PluginManager.cpp
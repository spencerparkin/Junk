// Application.cpp

#include "CornucopiaEditor.h"

//=================================================================================
namespace CornucopiaEditor
{
	IMPLEMENT_APP( Application )
}

using namespace CornucopiaEditor;

//=================================================================================
Application::Application( void )
{
	ifaceStyle = IFACE_STYLE_DOCKABLE_PANELS;
	config = new wxConfig();
	pluginManager = 0;
	suppressPluginManager = false;
	suppressAppSettings = false;
	suppressSplashScreen = false;
	splashScreen = 0;	// There's no need to delete this if we allocate it.  It gets freed by wxWidgets.
}

//=================================================================================
/*virtual*/ Application::~Application( void )
{
	delete config;
	delete pluginManager;
}

//=================================================================================
Cornucopia::Context& Application::GetContext( void )
{
	return context;
}

//=================================================================================
Frame* Application::GetFrame( void )
{
	return frame;
}

//=================================================================================
Application::InterfaceStyle Application::GetInterfaceStyle( void )
{
	return ifaceStyle;
}

//=================================================================================
void Application::SetInterfaceStyle( InterfaceStyle ifaceStyle )
{
	this->ifaceStyle = ifaceStyle;
}

//=================================================================================
PluginManager* Application::GetPluginManager( void )
{
	return pluginManager;
}

//=================================================================================
ResourceManager* Application::GetResourceManager( void )
{
	return &resourceManager;
}

//=================================================================================
/*virtual*/ bool Application::OnCmdLineParsed( wxCmdLineParser& parser )
{
	if( wxCMD_SWITCH_NOT_FOUND != parser.FoundSwitch( wxT( "spm" ) ) )
		suppressPluginManager = true;

	if( wxCMD_SWITCH_NOT_FOUND != parser.FoundSwitch( wxT( "sas" ) ) )
		suppressAppSettings = true;

	if( wxCMD_SWITCH_NOT_FOUND != parser.FoundSwitch( wxT( "sss" ) ) )
		suppressSplashScreen = true;

	wxString file;
	if( parser.Found( wxT( "file" ), &file ) )
		cmdLineFiles.Add( file );
	if( parser.Found( wxT( "file2" ), &file ) )
		cmdLineFiles.Add( file );
	if( parser.Found( wxT( "file3" ), &file ) )
		cmdLineFiles.Add( file );

	// TODO: FileReferenceNode base should be passed through on cmd-line.

	return true;
}

//=================================================================================
/*virtual*/ void Application::OnInitCmdLine( wxCmdLineParser& parser )
{
	parser.AddSwitch( wxT( "spm" ), wxT( "suppressPluginManager" ) );
	parser.AddSwitch( wxT( "sas" ), wxT( "suppressAppSettings" ) );
	parser.AddSwitch( wxT( "sss" ), wxT( "suppressSplashScreen" ) );

	// This is lame.  I wish that the "file" option could just simply be given multiple times.
	parser.AddOption( wxT( "file" ), wxEmptyString, wxT( "Open the given file upon application start-up." ), wxCMD_LINE_VAL_STRING );
	parser.AddOption( wxT( "file2" ), wxEmptyString, wxT( "Open the given file upon application start-up too." ), wxCMD_LINE_VAL_STRING );
	parser.AddOption( wxT( "file3" ), wxEmptyString, wxT( "Open the given file upon application start-up too." ), wxCMD_LINE_VAL_STRING );
}

//=================================================================================
/*virtual*/ bool Application::OnInit( void )
{
	// Among other things, this also does the command-line parsing and processing.
	if( !wxApp::OnInit() )
		return false;

	// The resource manager helps us location and load our application resources.
	// If this fails, we won't be able to load our resources.
	if( !resourceManager.Initialize( argc, argv ) )
	{
		wxMessageBox( wxT( "Cornucopia Editor's resource manager failed to initialize, probably because it couldn't find its resource file: CornucopiaEditor.zip" ), wxT( "Error" ), wxOK | wxCENTRE );
		return false;
	}
	
	// The rest of initialization may take a while, so throw up our splash screen.
	wxBitmap bitmap;
	if( !suppressSplashScreen && resourceManager.LoadBitmap( bitmap, "Art/splashScreen.bmp", wxBITMAP_TYPE_BMP ) )
		splashScreen = new wxSplashScreen( bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 3000, 0, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE | wxSTAY_ON_TOP );

	// Make sure we can create all built-in Cornucopia nodes.
	if( !context.RegisterAllBuiltinNodeCreators() )
		return false;

	// Setting this as the reading disposition allows us to load Cornucopia
	// trees, even if the underlying definition of the node has changed.
	context.SetNodeReadingDisposition( Cornucopia::Context::NONEXISTENCE_OF_FIELD_IS_DEFAULTED );

	// Create and register the help provider for our application.
	// At this time, there is no need to derive our own help provider class.
	wxHelpProvider::Set( new wxHelpControllerHelpProvider() );

	// Register our meta-data creator function.
	context.SetMetaDataCreatorFunc( EditorMetaData::Create );

	// Grab this info from the registry.
	AppSettings appSettings;
	RestoreAppSettings( appSettings );

	// Apply some, but not all settings from the restored app settings structure.
	if( !suppressAppSettings )
		ApplyAppSettings( appSettings );

	// wxWidgets should delete the frame for us on exit.
	if( suppressAppSettings )
		frame = new Frame( wxDefaultPosition, wxDefaultSize );
	else
		frame = new Frame( appSettings.framePos, appSettings.frameSize );
	SetTopWindow( frame );
	if( !suppressAppSettings )
		frame->SetPerspective( appSettings.framePerspective );
	frame->Show();

	// Go load all the files given on the cmd-line.
	for( int index = 0; index < ( signed )cmdLineFiles.size(); index++ )
	{
		wxString file = cmdLineFiles[ index ];
		frame->OpenTree( file );
	}

	// Lastly, go load any plugins that we find.
	if( !suppressPluginManager )
	{
		pluginManager = new PluginManager();
		pluginManager->LoadAllPlugins();
	}

	// Returning true here tells wxWidgets to proceed into the message pump.
	// Returning false would cause the application to exit immediately.
	return true;
}

//=================================================================================
/*virtual*/ int Application::OnExit( void )
{
	return wxApp::OnExit();
}

//=================================================================================
void Application::PopulateAppSettings( AppSettings& appSettings )
{
	appSettings.framePos = frame->GetPosition();
	appSettings.frameSize = frame->GetSize();
	appSettings.framePerspective = frame->GetPerspective();
	appSettings.ifaceStyle = ifaceStyle;
	appSettings.shownColumnFlags = DataViewCtrl::shownColumnFlags;
}

//=================================================================================
void Application::ApplyAppSettings( AppSettings& appSettings )
{
	ifaceStyle = InterfaceStyle( appSettings.ifaceStyle );
	DataViewCtrl::shownColumnFlags = appSettings.shownColumnFlags;
}

//=================================================================================
void Application::SaveAppSettings( const AppSettings& appSettings )
{
	config->Write( wxT( "framePosX" ), appSettings.framePos.x );
	config->Write( wxT( "framePosY" ), appSettings.framePos.y );
	config->Write( wxT( "frameSizeX" ), appSettings.frameSize.x );
	config->Write( wxT( "frameSizeY" ), appSettings.frameSize.y );
	config->Write( wxT( "framePerspective" ), appSettings.framePerspective );
	config->Write( wxT( "ifaceStyle" ), appSettings.ifaceStyle );
	config->Write( wxT( "shownColumnFlags" ), appSettings.shownColumnFlags );
}

//=================================================================================
void Application::RestoreAppSettings( AppSettings& appSettings )
{
	config->Read( wxT( "framePosX" ), &appSettings.framePos.x, 50 );
	config->Read( wxT( "framePosY" ), &appSettings.framePos.y, 50 );
	config->Read( wxT( "frameSizeX" ), &appSettings.frameSize.x, 800 );
	config->Read( wxT( "frameSizeY" ), &appSettings.frameSize.y, 800 );
	config->Read( wxT( "framePerspective" ), &appSettings.framePerspective, wxT( "" ) );
	config->Read( wxT( "ifaceStyle" ), &appSettings.ifaceStyle, IFACE_STYLE_DOCKABLE_PANELS );
	config->Read( wxT( "shownColumnFlags" ), &appSettings.shownColumnFlags, DataViewCtrl::shownColumnFlags );

	// TODO: We should probably make sure that the restored frame parameters
	//       actually make sense with the current display here.
}

//=================================================================================
void Application::CornucopiaErrorMessageBox( const wxString& errorMessage )
{
	wxString shortErrorMessage = errorMessage + wxT( "\n\n" );
	if( errorMessage.IsEmpty() )
		shortErrorMessage = wxT( "An internal error has occurred within the core Cornucopia library." );

	std::list< std::string > errorStringList;
	context.GetErrorStringList( errorStringList );

	wxString extendedErrorMessage = wxT( "" );
	for( std::list< std::string >::iterator iter = errorStringList.begin(); iter != errorStringList.end(); iter++ )
	{
		std::string errorLine = *iter;
		extendedErrorMessage += wxString( ( const char* )errorLine.c_str() ) + wxT( "\n" );
	}

	wxMessageDialog messageDialog( frame, shortErrorMessage, wxT( "Error" ), wxOK | wxCENTRE | wxICON_ERROR );
	messageDialog.SetExtendedMessage( extendedErrorMessage );
	messageDialog.ShowModal();
}

//=================================================================================
/*static*/ bool Application::CopyToClipBoard( Cornucopia::Node* node )
{
	if( !wxTheClipboard->Open() )
		return false;
	
	DataObject* dataObject = new DataObject();
	dataObject->SetNode( node );
	wxTheClipboard->SetData( dataObject );
	wxTheClipboard->Close();
	return true;
}

//=================================================================================
/*static*/ Cornucopia::Node* Application::CreateFromClipBoard( void )
{
	Cornucopia::Node* node = 0;

	if( wxTheClipboard->Open() )
	{
		DataObject dataObject;
		wxTheClipboard->GetData( dataObject );
		node = dataObject.GetNode();
		wxTheClipboard->Close();
	}

	return node;
}

// Application.cpp
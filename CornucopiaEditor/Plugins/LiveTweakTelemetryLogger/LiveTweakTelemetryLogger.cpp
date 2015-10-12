// LiveTweakTelemetryLogger.cpp

#include "LiveTweakTelemetryLogger.h"

//=================================================================================
extern "C" __declspec( dllexport ) CornucopiaEditor::Plugin* CreatePlugin( void )
{
	return new LiveTweakTelemetryLoggerPlugin();
}

//=================================================================================
LiveTweakTelemetryLoggerPlugin::LiveTweakTelemetryLoggerPlugin( void )
{
	frame = 0;
}

//=================================================================================
/*virtual*/ LiveTweakTelemetryLoggerPlugin::~LiveTweakTelemetryLoggerPlugin()
{
}

//=================================================================================
/*virtual*/ bool LiveTweakTelemetryLoggerPlugin::Initialize( CornucopiaEditor::PluginAssistant* assistant )
{
	frame = new Frame( wxDefaultPosition, wxSize( 500, 800 ) );
	frame->Show();

	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_INITIALIZE, &LiveTweakTelemetryLoggerPlugin::OnInitialize, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FINALIZE, &LiveTweakTelemetryLoggerPlugin::OnFinalize, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_TWEAK_NODE_CHANGED, &LiveTweakTelemetryLoggerPlugin::OnLiveTweakNodeChanged, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_TWEAK_NODE_SCRUBBING, &LiveTweakTelemetryLoggerPlugin::OnLiveTweakNodeScrubbing, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_TWEAK_FULL_UPDATE, &LiveTweakTelemetryLoggerPlugin::OnLiveTweakFullUpdate, this );

	return true;
}

//=================================================================================
/*virtual*/ void LiveTweakTelemetryLoggerPlugin::Finalize( CornucopiaEditor::PluginAssistant* assistant )
{
	// We don't want to call "Destroy()" here on the frame, because
	// that will queue the frame for deletion later during idle processing,
	// by which time, our library address space has gone out of scope.
	delete frame;
	frame = 0;
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::OnInitialize( CornucopiaEditor::PluginEvent& event )
{
	frame->assistant = event.GetAssistant();
	frame->libraryPath = event.GetLibraryPath();
	frame->startTime = wxGetLocalTimeMillis();
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::OnFinalize( CornucopiaEditor::PluginEvent& event )
{
}

//=================================================================================
/*virtual*/ void LiveTweakTelemetryLoggerPlugin::ProvideInfo( Info& pluginInfo )
{
	pluginInfo.pluginAuthor = "Spencer T. Parkin";
	pluginInfo.pluginVersion = "v1.0";
	pluginInfo.pluginName = "Live-Tweak Telementry Logger";
	pluginInfo.pluginDesc = "Live-tweak telementry is received from the editor and sent to a log window.";
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::OnLiveTweakNodeChanged( CornucopiaEditor::PluginTweakEvent& event )
{
	frame->LogTweakEvent( event );
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::OnLiveTweakNodeScrubbing( CornucopiaEditor::PluginTweakEvent& event )
{
	frame->LogTweakEvent( event );
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::OnLiveTweakFullUpdate( CornucopiaEditor::PluginTweakEvent& event )
{
	frame->LogTweakEvent( event );
}

//=================================================================================
LiveTweakTelemetryLoggerPlugin::Frame::Frame( const wxPoint& pos, const wxSize& size ) : wxFrame( 0, wxID_ANY, wxT( "Live-Tweak Telemetry Logger" ), pos, size )
{
	textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_LEFT );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( textCtrl, 1, wxGROW | wxALL, 0 );
	SetSizer( boxSizer );

	wxMenu* pluginMenu = new wxMenu();
	wxMenuItem* exitMenuItem = new wxMenuItem( pluginMenu, ID_PluginExit, wxT( "Exit" ), wxT( "Exit this plugin." ) );
	pluginMenu->Append( exitMenuItem );

	menuBar = new wxMenuBar();
	menuBar->Append( pluginMenu, wxT( "Plugin" ) );
	SetMenuBar( menuBar );

	statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	Bind( wxEVT_MENU, &Frame::OnPluginExit, this );
}

//=================================================================================
/*virtual*/ LiveTweakTelemetryLoggerPlugin::Frame::~Frame( void )
{
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::Frame::LogTweakEvent( CornucopiaEditor::PluginTweakEvent& event )
{
	wxLongLong currentTime = wxGetLocalTimeMillis();
	wxLongLong deltaTime = currentTime - startTime;
	double milliseconds = deltaTime.ToDouble();
	wxString timeString = wxString::Format( "%f", milliseconds );

	wxString eventType = "Unknown";
	if( event.GetEventType() == CornucopiaEditor::CORNUCOPIA_PLUGIN_TWEAK_NODE_CHANGED )
		eventType = "Node Changed";
	else if( event.GetEventType() == CornucopiaEditor::CORNUCOPIA_PLUGIN_TWEAK_NODE_SCRUBBING )
		eventType = "Node Scrubbing";
	else if( event.GetEventType() == CornucopiaEditor::CORNUCOPIA_PLUGIN_TWEAK_FULL_UPDATE )
		eventType = "Full Update";

	Cornucopia::Node* node = event.GetNode();
	wxString nodeName = node->GetName();
	wxString displayString = node->GetValueToDisplayString();

	wxString logMessage = timeString + " - " + eventType + " - " + nodeName + " - " + displayString + "\n";
	textCtrl->AppendText( logMessage );
}

//=================================================================================
void LiveTweakTelemetryLoggerPlugin::Frame::OnPluginExit( wxCommandEvent& event )
{
	assistant->UnloadPlugin( libraryPath );
}

// LiveTweakTelemetryLogger.cpp
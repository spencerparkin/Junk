// LiveTweakTelemetryLogger.h

#include "Plugin.h"

#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/datetime.h>

//=================================================================================
// This plugin is a diagnostic tool that tells us what live-tweaks are being
// emitted by the Cornucopia Editor and when.
class LiveTweakTelemetryLoggerPlugin : public CornucopiaEditor::Plugin
{
public:

	LiveTweakTelemetryLoggerPlugin( void );
	virtual ~LiveTweakTelemetryLoggerPlugin();

	virtual bool Initialize( CornucopiaEditor::PluginAssistant* assistant ) override;
	virtual void Finalize( CornucopiaEditor::PluginAssistant* assistant ) override;

	virtual void ProvideInfo( Info& pluginInfo ) override;

	void OnInitialize( CornucopiaEditor::PluginEvent& event );
	void OnFinalize( CornucopiaEditor::PluginEvent& event );

	void OnLiveTweakNodeChanged( CornucopiaEditor::PluginTweakEvent& event );
	void OnLiveTweakNodeScrubbing( CornucopiaEditor::PluginTweakEvent& event );
	void OnLiveTweakFullUpdate( CornucopiaEditor::PluginTweakEvent& event );

private:

	//=================================================================================
	// This plugin manifests itself as an extra window with scrolling text in it.
	class Frame : public wxFrame
	{
	public:

		Frame( const wxPoint& pos, const wxSize& size );
		virtual ~Frame( void );

		void LogTweakEvent( CornucopiaEditor::PluginTweakEvent& event );

		CornucopiaEditor::PluginAssistant* assistant;
		wxString libraryPath;
		wxLongLong startTime;

	private:

		enum
		{
			ID_PluginExit = wxID_HIGHEST,
		};

		void OnPluginExit( wxCommandEvent& event );

		wxMenuBar* menuBar;
		wxStatusBar* statusBar;
		wxTextCtrl* textCtrl;
	};

	Frame* frame;
};

// LiveTweakTelemetryLogger.h
// PerforceClient.h

#include "Plugin.h"
#include "PerforceOperation.h"

// TODO: We need to invalidate our file status cache when the entire program loses focus.
//       After regaining focus, it's possible that the status of the file has been changed by some other program.

//=================================================================================
class PerforceClientPlugin: public CornucopiaEditor::Plugin
{
public:

	PerforceClientPlugin( void );
	virtual ~PerforceClientPlugin( void );

	virtual bool Initialize( CornucopiaEditor::PluginAssistant* assistant ) override;
	virtual void Finalize( CornucopiaEditor::PluginAssistant* assistant ) override;

	virtual void ProvideInfo( Info& pluginInfo ) override;

	void OnPageCreated( CornucopiaEditor::PluginPageEvent& event );
	void OnPageChanged( CornucopiaEditor::PluginPageEvent& event );
	void OnPageDestroying( CornucopiaEditor::PluginPageEvent& event );

	void OnFrameActivated( CornucopiaEditor::PluginFrameEvent& event );
	void OnFrameSetFocus( CornucopiaEditor::PluginFrameEvent& event );
	void OnFrameKillFocus( CornucopiaEditor::PluginFrameEvent& event );

	void OnFileCreated( CornucopiaEditor::PluginFileEvent& event );
	void OnResolveNonWritableFile( CornucopiaEditor::PluginFileEvent& event );

	FileStatusOperation::FileStatus* GetFileStatus( const wxString& file );
	void UpdateFileStatus( const wxString& file );
	void InvalidateFileStatus( const wxString& file );
	void InvalidateFileStatusCache( void );

private:

	void BindHandlers( void );
	void UnbindHandlers( void );

	WX_DECLARE_STRING_HASH_MAP( FileStatusOperation::FileStatus, FileStatusMap );
	FileStatusMap* fileStatusMap;

	void UpdateToolBar( wxWindow* page = 0 );

	class ToolBar : public wxAuiToolBar
	{
	public:

		ToolBar( wxWindow* parent, PerforceClientPlugin* plugin );
		virtual ~ToolBar( void );

		void UpdateInterface( wxWindow* page = 0 );
		void UpdateInterface( const wxString& file );

		bool PerformBasicOperationOnCurrentFile( PerforceOperation* perforceOperation, bool doStatusUpdate = true, bool requestReload = false );

	private:

		enum
		{
			ID_Add = wxID_HIGHEST + 1000,	// +1000 is a hack to avoid collision with appliation IDs.  Can we avoid it by using Bind()?
			ID_CheckOut,
			ID_Submit,
			ID_Revert,
			ID_Lock,
			ID_Unlock,
			ID_GetLatest,
		};

		void OnAdd( wxCommandEvent& event );
		void OnCheckOut( wxCommandEvent& event );
		void OnSubmit( wxCommandEvent& event );
		void OnRevert( wxCommandEvent& event );
		void OnLock( wxCommandEvent& event );
		void OnUnlock( wxCommandEvent& event );
		void OnGetLatest( wxCommandEvent& event );

		wxButton* addButton;
		wxButton* checkOutButton;
		wxButton* submitButton;
		wxButton* revertButton;
		wxButton* lockButton;
		wxButton* unlockButton;
		wxButton* getLatestButton;

		wxTextCtrl* fileStatusText;

		PerforceClientPlugin* plugin;

		DECLARE_EVENT_TABLE()
	};

	ToolBar* toolBar;

	CornucopiaEditor::PluginAssistant* assistant;
};

// PerforceClient.h
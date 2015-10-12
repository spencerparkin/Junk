// Frame.h

namespace CornucopiaEditor
{
	//=================================================================================
	class Frame : public wxFrame
	{
		wxDECLARE_DYNAMIC_CLASS( Frame );

	private:

		Frame( void );

	public:

		Frame( const wxPoint& pos, const wxSize& size );
		virtual ~Frame( void );

		void HandleDroppedFiles( const wxArrayString& fileNames );

		bool LastChanceToSavePanel( Panel* panel );

		NoteBook* GetNoteBook( void );

		void UpdateUndoRedoToolBar( void );
		void UpdateFileToolBar( void );
		void UpdateMenuBar( void );
		void UpdateSearchBar( void );

		bool OpenTree( wxString& filePath );
		bool SaveTree( Panel* panel );
		bool ReopenTree( Panel* panel, const wxString& prompt );

		wxString GetPerspective( void );
		void SetPerspective( const wxString& perspective );

		Panel* CreatePanel( Tree* tree, UndoRedo* undoRedo = 0 );

		void ActivateDocumentationPanel( const wxString& initialPage = wxEmptyString );

		void NotifyOfImminentPageClosure( wxWindow* page );

		wxAuiManager* GetAuiManager( void );

		PluginPanel* GetPluginPanel( void );

	private:

		void OnIdle( wxIdleEvent& event );
		void OnSetFocus( wxFocusEvent& event );
		void OnKillFocus( wxFocusEvent& event );
		void OnActivate( wxActivateEvent& event );
		void OnNewTree( wxCommandEvent& event );
		void OnNewClone( wxCommandEvent& event );
		void OnOpenTree( wxCommandEvent& event );
		void OnSaveTree( wxCommandEvent& event );
		void OnUpgradeTrees( wxCommandEvent& event );
		void OnExit( wxCommandEvent& event );
		void OnClose( wxCloseEvent& event );
		void OnHelp( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );
		void OnUndo( wxCommandEvent& event );
		void OnRedo( wxCommandEvent& event );
		void OnSearch( wxCommandEvent& event );
		void OnSearchBoxEnter( wxCommandEvent& event );
		void OnContextSensativeHelp( wxCommandEvent& event );
		void OnWipeTreeCache( wxCommandEvent& event );
		void OnTouchTree( wxCommandEvent& event );
		void OnPluginManager( wxCommandEvent& event );
		void OnCheckUniqueness( wxCommandEvent& event );
		void OnFileWatchEvent( wxFileSystemWatcherEvent& event );

		enum
		{
			ID_NewTree = wxID_HIGHEST,
			ID_NewClone,
			ID_OpenTree,
			ID_SaveTree,
			ID_UpgradeTrees,
			ID_Help,
			ID_Exit,
			ID_About,
			ID_PageChanged,
			ID_Undo,
			ID_Redo,
			ID_Search,
			ID_SearchBox,
			ID_ContextSensativeHelp,
			ID_WipeTreeCache,
			ID_TouchTree,
			ID_PluginManager,
			ID_CheckUniqueness,
		};

		void BuildUserInterface( void );

		void PerformSearch( void );

		wxAuiToolBar* fileToolBar;
		wxAuiToolBar* undoRedoToolBar;
		wxAuiToolBar* searchToolBar;
		wxAuiToolBar* helpToolBar;
		wxComboBox* searchBox;
		wxMenuBar* menuBar;
		wxStatusBar* statusBar;
		NoteBook* noteBook;
		wxButton* contextSensativeHelpButton;

		wxAuiManager auiManager;

		HelpPanel* helpPanel;
		PluginPanel* pluginPanel;

		wxIcon icon;

		DECLARE_EVENT_TABLE()
	};
}

// Frame.h
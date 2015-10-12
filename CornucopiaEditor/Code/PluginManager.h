// PluginManager.h

namespace CornucopiaEditor
{
	//=================================================================================
	class PluginManager
	{
	public:

		PluginManager( void );
		~PluginManager( void );

		void LoadAllPlugins( void );
		void UnloadAllPlugins( void );

		// The order in which the loaded plugins are called is left undefined.
		bool CallAllPlugins( PluginEvent& event );

		bool LoadPlugin( const Cornucopia::Path& libraryPath );
		bool UnloadPlugin( const Cornucopia::Path& libraryPath );
		bool UnloadPluginLater( const Cornucopia::Path& libraryPath );

		CornucopiaEditor::PluginAssistant* GetAssistant( void );

		class PluginProcessor
		{
		public:
			virtual bool ProcessPlugin( CornucopiaEditor::Plugin* plugin, const Cornucopia::Path& libraryPath ) = 0;
		};

		bool ProcessAllPlugins( PluginProcessor* pluginProcessor );

		void DoIdleProcessing( void );

	private:

		//=================================================================================
		class PluginAssistant : public CornucopiaEditor::PluginAssistant
		{
		public:

			virtual wxFrame* GetFrame( void ) override;
			virtual wxAuiManager* GetFrameAuiManager( void ) override;
			virtual wxAuiNotebook* GetAuiNoteBook( void ) override;

			virtual bool IsDataViewCtrlForTree( wxDataViewCtrl* dataViewCtrl ) override;
			virtual int GetUnusedModelColumn( wxDataViewCtrl* dataViewCtrl ) override;
			
			virtual wxWindow* GetCurrentPage( void ) override;
			virtual Cornucopia::Node* GetCurrentTree( void ) override;
			virtual wxString GetCurrentFile( void ) override;

			virtual Cornucopia::Node* GetTreeFromPage( wxWindow* page ) override;
			virtual wxString GetFileFromPage( wxWindow* page ) override;

			virtual bool RequestPageReload( wxWindow* page, const wxString& reason ) override;

			virtual bool UnloadPlugin( const wxString& libraryPath ) override;
		};

		//=================================================================================
		typedef CornucopiaEditor::Plugin* ( *CreatePluginFunc )( void );

		//=================================================================================
		class Plugin
		{
		public:

			Plugin( void );
			~Plugin( void );

			bool Load( const std::string& libraryPath );
			bool Unload( void );

			const Cornucopia::Path& GetLibraryPath( void ) const;
			CornucopiaEditor::Plugin* GetEventHandler( void );

		private:

			Cornucopia::Path libraryPath;
			HMODULE libraryHandle;

			CornucopiaEditor::Plugin* eventHandler;
		};

		typedef std::list< Plugin* > PluginList;

		bool FindPlugin( const Cornucopia::Path& libraryPath, PluginList::iterator* iter = 0 );
		bool UnloadPlugin( PluginList::iterator& iter );
		bool CallPlugin( Plugin* plugin, PluginEvent& event );

		PluginList pluginList;
		PluginAssistant assistant;
		wxArrayString pendingUnloadList;
	};
}

// PluginManager.h
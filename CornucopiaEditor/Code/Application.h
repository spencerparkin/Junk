// Application.h

namespace CornucopiaEditor
{
	//=================================================================================
	class Application : public wxApp
	{
	public:

		Application( void );
		virtual ~Application( void );

		virtual bool OnInit( void ) override;
		virtual int OnExit( void ) override;

		virtual bool OnCmdLineParsed( wxCmdLineParser& parser ) override;
		virtual void OnInitCmdLine( wxCmdLineParser& parser ) override;

		Cornucopia::Context& GetContext( void );

		Frame* GetFrame( void );

		// These are settings we'll save and restore in the registry.
		struct AppSettings
		{
			wxPoint framePos;
			wxSize frameSize;
			wxString framePerspective;
			int ifaceStyle;
			int shownColumnFlags;
		};

		void PopulateAppSettings( AppSettings& appSettings );
		void ApplyAppSettings( AppSettings& appSettings );
		void SaveAppSettings( const AppSettings& appSettings );
		void RestoreAppSettings( AppSettings& appSettings );

		void CornucopiaErrorMessageBox( const wxString& errorMessage = wxT( "" ) );

		static bool CopyToClipBoard( Cornucopia::Node* node );
		static Cornucopia::Node* CreateFromClipBoard( void );

		enum InterfaceStyle
		{
			IFACE_STYLE_DOCKABLE_PANELS,
			IFACE_STYLE_CONTROL_TREE,
			IFACE_STYLE_PROPERTY_GRID,
		};

		InterfaceStyle GetInterfaceStyle( void );
		void SetInterfaceStyle( InterfaceStyle ifaceStyle );

		PluginManager* GetPluginManager( void );
		ResourceManager* GetResourceManager( void );

	private:

		static wxString ConfigKey( const wxString& key );

		Cornucopia::Context context;
		wxConfig* config;
		Frame* frame;
		InterfaceStyle ifaceStyle;
		PluginManager* pluginManager;
		ResourceManager resourceManager;
		bool suppressPluginManager;
		bool suppressAppSettings;
		bool suppressSplashScreen;
		wxArrayString cmdLineFiles;
		wxSplashScreen* splashScreen;
	};

	//=================================================================================
	DECLARE_APP( Application );
}

// Application.h
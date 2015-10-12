// EditorMetaData.h

namespace CornucopiaEditor
{
	//=================================================================================
	// The purpose that Cornucopia Editor has of utilizing a Cornucopia tree's
	// meta data is that of providing user interface persistence.  That is, the
	// user interface layout that was being used during the last save of a file
	// should be restored on a subsequent load of the file.
	class EditorMetaData : public Cornucopia::MetaData
	{
	public:

		EditorMetaData( void );
		virtual ~EditorMetaData( void );

		static Cornucopia::MetaData* Create( void );

		virtual bool ReadFromTable( lua_State* L, Cornucopia::Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Cornucopia::Context& context ) const override;

		virtual bool Copy( const MetaData* metaData, Cornucopia::Context& context ) override;

		virtual void PostCreate( Cornucopia::Node* node ) override;
		virtual void PreDestroy( Cornucopia::Node* node ) override;

		bool GetExpanded( void );
		void SetExpanded( bool expanded );

		NodeControlPanel* GetNodeControlPanel( void ) const;
		void SetNodeControlPanel( NodeControlPanel* nodeControlPanel );

		int GetNodeControlPanelHandle( void ) const;
		void SetNodeControlPanelHandle( int nodeControlPanelHandle );

		wxString GetPaneInfoString( void );
		void SetPaneInfoString( const wxString& paneInfoString );

		Panel* FindAssociatedPanel( void );

		wxString GetFeedback( void ) const;
		void SetFeedback( const wxString& feedback );

		enum ValueDisplayMode
		{
			SHOW_DISPLAY_VALUE,
			SHOW_VALUE_AS_STRING,
		};

		ValueDisplayMode GetValueDisplayMode( void ) const;
		void SetValueDisplayMode( ValueDisplayMode valueDisplayMode );

		void SetBridgeNode( Cornucopia::Node* node );
		Cornucopia::Node* GetBridgeNode( void ) const;

		void SetVisibility( bool visibile );
		bool GetVisibility( void ) const;

		wxPGProperty* GetProperty( void );
		void SetProperty( wxPGProperty* property );

		bool GetShouldActivateNodeControlPanel( void );
		void SetShouldActivateNodeControlPanel( bool shouldActivateNodeControlPanel );

	private:

		bool expanded, visible;
		wxString paneInfoString;

		int nodeControlPanelHandle;
		bool shouldActivateNodeControlPanel;

		Cornucopia::Node* bridgeNode;

		wxPGProperty* property;

		wxString feedback;
		ValueDisplayMode valueDisplayMode;
	};
}

// EditorMetaData.h
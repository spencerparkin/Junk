// DockingPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class DockingPanel : public Panel
	{
		wxDECLARE_DYNAMIC_CLASS( DockingPanel );

	private:

		DockingPanel( void );

	public:

		DockingPanel( wxWindow* parent, Tree* tree, const wxPoint& pos, const wxSize& size );
		virtual ~DockingPanel( void );

		virtual void BuildUserInterface( void ) override;
		virtual void RefreshUserInterface( void ) override;

		virtual void PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree ) override;
		virtual void PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree ) override;

		virtual void PreSave( void ) override;

		virtual void SynchronizeControlsWithNode( Cornucopia::Node* node ) override;

		void ActivateNodeControlPanelForNode( Cornucopia::Node* node, bool updateManager, bool beSilent );
		void ActivateNodeControlPanelsForSubTree( Cornucopia::Node* node, bool beSilent );
		void DeactivateNodeControlPanelForNode( Cornucopia::Node* node, bool updateManager, bool deletePanel = false );
		void DeactivateNodeControlPanelsForSubTree( Cornucopia::Node* node, bool updateManager, bool deletePanel = false );

		bool IsNodeControlPanelActiveForNode( Cornucopia::Node* node );

		wxAuiManager* GetAuiManager( void );
		DataViewCtrl* GetDataViewControl( void );

		virtual void HighlightNodeControlPanel( NodeControlPanel* highlightNodeControlPanel ) override;
		void HighlightNodeItem( Cornucopia::Node* node );

		virtual bool FindNext( const wxString& searchCriteria ) override;

	private:

		void RefreshDataViewControl( void );
		void RefreshTreeControlExpansionState( void );
		void RefreshColumnWidths( void );
		void RestoreNodeControlPanels( void );
		void SaveNodeControlPanelInfo( Cornucopia::Node* node );

		DataViewCtrl* dataViewCtrl;

		wxAuiManager auiManager;

		class DockArt : public wxAuiDefaultDockArt
		{
		public:
			DockArt( void );
			virtual ~DockArt( void );

			virtual void DrawCaption( wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane ) override;
		};
	};
}

// DockingPanel.h
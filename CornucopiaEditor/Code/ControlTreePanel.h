// ControlTreePanel.h

// Using a bordered panel over the static box significantly improves
// the flicker problem, though the flicker problem with scrolling is
// still there is as great of force as it was before.  Also, the bordered
// panel version appears to create a weird toggle button bug, where it's
// possible to push a toggle button, but it doesn't expand or collaps.
// Was this also there when using the static boxes?
//#define CONTROL_TREE_USE_STATIC_BOX

namespace CornucopiaEditor
{
	//=================================================================================
	class ControlTreePanel : public Panel
	{
		wxDECLARE_DYNAMIC_CLASS( ControlTreePanel );

	private:

		ControlTreePanel( void );

	public:
		
		ControlTreePanel( wxWindow* parent, Tree* tree );
		virtual ~ControlTreePanel( void );

		virtual void BuildUserInterface( void ) override;
		void RebuildUserInterface( void );
		virtual void RefreshUserInterface( void ) override;

		virtual void PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree ) override;
		virtual void PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree ) override;

	private:

		void OnSize( wxSizeEvent& event );
		void OnScrollThumbTrack( wxScrollWinEvent& event );
		void OnScrollThumbRelease( wxScrollWinEvent& event );

		void BuildUserInterfaceRecursively( Cornucopia::Node* node, wxWindow* parent );

		class ToggleButton : public wxToggleButton
		{
		public:

			ToggleButton( wxWindow* parent, Cornucopia::Node* node );
			virtual ~ToggleButton( void );

			void OnToggle( wxCommandEvent& event );

			void SetNode( Cornucopia::Node* node );
			Cornucopia::Node* GetNode( void ) const;

			void SetPlaceholderPanel( wxPanel* placeholderPanel );
			wxPanel* GetPlaceholderPanel( void ) const;

		private:

			ControlTreePanel* FindControlTreePanel( void );

			Cornucopia::Node* node;
			wxPanel* placeholderPanel;
		};

		std::list< ToggleButton* > toggleButtonList;

		DECLARE_EVENT_TABLE()
	};
}

// ControlTreePanel.h
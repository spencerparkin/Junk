// Panel.h

namespace CornucopiaEditor
{
	//=================================================================================
	// Though perhaps poorly named, the panel class created here for use by our editor
	// is a panel that is added as a tab to our note-book control.  This panel is
	// repsonsible for providing a user-interface to a single Corncuopia tree.
	class Panel : public wxPanel
	{
		wxDECLARE_DYNAMIC_CLASS( Panel );

	public:

		Panel( void );		// Can't make private, but should never be used.
		Panel( wxWindow* parent, Tree* tree, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
		virtual ~Panel( void );

		Tree* GetTree( bool remove = false );

		// Can't make these pure-virtual because of RTTI mechanism, but they must be implemented.
		virtual void BuildUserInterface( void );
		virtual void RefreshUserInterface( void );

		UndoRedo* GetUndoRedoSystem( void );
		void SetUndoRedoSystem( UndoRedo* undoRedo, bool deleteCurrent = true );

		void TakeSnapShot( void );

		virtual void PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree );		// Given tree is new.  Owned tree should be the old one.
		virtual void PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree );		// Given tree is old.  Owned tree should be the new one.

		virtual void PreSave( void );
		virtual void PostOpen( void );

		virtual void SynchronizeControlsWithNode( Cornucopia::Node* node );

		void SendLiveTweakEvent( Cornucopia::Node* node, bool scrubbing );

		virtual void DoIdleProcessing( void );

		virtual void DisplayContextSensativeHelpForNode( Cornucopia::Node* node );

		virtual void HighlightNodeControlPanel( NodeControlPanel* highlightNodeControlPanel );

		virtual bool FindNext( const wxString& searchCriteria );

		void PatchNodeControlPanels( void );
		static void PatchNodeControlPanelsUsingBridge( Cornucopia::Node* root );

		// A bridge can only be created between two trees that have the same structure.  These are trees with
		// the same branch structure and the same types of nodes at each point in the tree.  The values of
		// nodes, however, can different between the trees.  The bridge is a link between associated nodes of
		// the two trees.  For example, the root of one tree would be associated with the root of the other
		// tree.  Being able to create a bridge like this is important to the Cornucopia editor, because if
		// such a bridge can be created, then the current user interface can be repatched and re-used without
		// the need to regenerate all or any part of it.  Furthermore, if the current user interface can
		// continue without modification or regeneration, certain non-persistent aspects of it, such as scroll
		// positions and canvas camera orientations, can remain as is through the process of exposing a new
		// tree through the interface while disposing of an old tree, which process is most often performed by
		// the undo/redo system.
		bool CreateBridge( Cornucopia::Node* node, Cornucopia::Node* targetNode );

	protected:

		void RefreshNodeControlPanels( void );

		Tree* tree;
		UndoRedo* undoRedo;		// This must be created after the user interface is created.
	};
}

// Panel.h
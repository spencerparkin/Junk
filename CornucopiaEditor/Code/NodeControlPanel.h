// NodeControlPanel.h

namespace CornucopiaEditor
{
	// TODO: We might create a class derivative of NodeControlPanel that interacts with a plugin,
	//       so that a plugin could create it's own node control panel for nodes it registers
	//       with the core Cornucopia library.  That may be more work than it's worth.  I can't
	//       see at the moment why we would need to add a node via plugin instead of just adding
	//       it to the core library and to the editor, but it's just a thought.

	//=================================================================================
	// A node control panel provides an interface to a single Cornucopia node.
	// It may be possible for a node control panel to become orphaned, but
	// we won't leak memory.
	class NodeControlPanel : public wxPanel
	{
		wxDECLARE_DYNAMIC_CLASS( NodeControlPanel );

	public:

		NodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		NodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~NodeControlPanel( void );

		static NodeControlPanel* CreateForNode( wxWindow* parent, Cornucopia::Node* node );

		virtual wxString GetCaption( void );

		// These were supposed to be pure virtual functions, but
		// the dynamic class macro stuff won't allow it.
		virtual bool PushNodeDataToInterface( void ) { return false; }
		virtual bool PullNodeDataFromInterface( void ) { return false; }

		void SynchronizeTreePanelControls( bool updateUndoRedoSystem );

		Cornucopia::Node* GetNode( void );
		void SetNode( Cornucopia::Node* node );		// The caller must take care that the node is of the correct type!  Derived classes may not and should not need to do a dynamic cast.

		// If the node control panel has one or more canvas controls as children,
		// they will call this method with themselves as the argument when it's time to render.
		virtual void CanvasRender( Canvas* canvas, GLenum renderMode );

		// If the node control has one or more canvas controls, and supports selection,
		// this method will be called by the given canvas with hit records that you can process.
		// The canvas class also supports some convenience routines to help you process these records.
		virtual void CanvasSelect( Canvas* canvas, unsigned int* hitBuffer, int hitBufferSize, int hitCount );

		void SetHighlighted( bool highlighted );	// This should never be called directly.  Ask the owning panel to highlight the node control panel.
		bool GetHighlighted( void );

		void Highlight( void );

		int GetHandle( void );

		static NodeControlPanel* DereferenceHandle( int handle );

		// Used in the docking panel interface style, this can be optionally overridden
		// to provide menu options that can be selected to edit the node exposed through
		// this node control panel interface.
		virtual wxMenu* CreateMenu( wxEvtHandler* eventHandler );

		//=================================================================================
		// Provide a way to present a node control panel to the user as a modal dialog
		// for a given node we want the user to edit.
		class Dialog : public wxDialog
		{
		public:

			Dialog( Cornucopia::Node* node, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
			virtual ~Dialog( void );

			virtual wxWindow* GetContentWindow( void ) const override;
			virtual bool IsModal( void ) const override;

			NodeControlPanel* GetNodeControlPanel( void );

		private:

			NodeControlPanel* nodeControlPanel;
		};

	protected:

		virtual void BuildUserInterface( void ) {}

		virtual void OnMouse( wxMouseEvent& event );

		void OnContextSensativeHelp( wxHelpEvent& event );

		Cornucopia::Node* node;		// This needs to be patched appropriately as different trees become exposed through the interface.
		bool highlighted;

		int handle;

		static HandleSystem< NodeControlPanel > handleSystem;

	private:

		DECLARE_EVENT_TABLE()
	};
}

// NodeControlPanel.h
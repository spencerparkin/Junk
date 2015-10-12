// PropertyGridPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	// Provide an interface for editing a Cornucopia tree through the use of
	// the wxPropertyGrid class.  Like the control-tree style interface, here
	// the structure of the tree is not editable, which makes undo-redo simpler.
	class PropertyGridPanel : public Panel
	{
		wxDECLARE_DYNAMIC_CLASS( PropertyGridPanel );

	private:

		PropertyGridPanel( void );

	public:

		PropertyGridPanel( wxWindow* parent, Tree* tree );
		virtual ~PropertyGridPanel( void );

		virtual void BuildUserInterface( void ) override;
		virtual void RefreshUserInterface( void ) override;

		virtual void PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree ) override;
		virtual void PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree ) override;

		virtual void SynchronizeControlsWithNode( Cornucopia::Node* node ) override;

		virtual bool FindNext( const wxString& searchCriteria ) override;

	private:

		// Rebuild the property grid as a function of the tree we edit.  This is used to
		// create the initial property grid tree and to regenerate it in the case that
		// and undo/redo event changes the structure of the tree.
		void RebuildPropertyGrid( void );
		void BuildPropertyGridRecursively( wxPGProperty* parentProperty, Cornucopia::Node* node );

		// This transfers all values in the tree we edit to the property grid's properties.
		// This should be called immediately after rebuilding the property grid, or whenever
		// one or more values in the tree we edit changes unbeknownst to the property grid,
		// such as when an undo or redo occurs.
		void RefreshPropertyGrid( void );

		// Here, when changes are made through the property grid, we transfer the property
		// value to its corresponding node.
		void OnPropertyValueChanged( wxPropertyGridEvent& event );

		// When this happens, bring up a dialog that the user can use to edit the property another way.
		void OnPropertyGridDoubleClick( wxPropertyGridEvent& event );

		// Used to build the property tree, here we allocate and return an appropriate property
		// that can be used to edit the given node.
		wxPGProperty* CreatePropertyForNode( Cornucopia::Node* node );

		wxPGProperty* GetPropertyFromNode( Cornucopia::Node* node );
		Cornucopia::Node* GetPropertyToNode( wxPGProperty* property );

		void ShowDialogForNode( Cornucopia::Node* node );
		NodeControlPanel::Dialog* FindDialogForNode( Cornucopia::Node* node );
		void RefreshAllDialogs( void );
		void DestroyAllDialogs( void );

		void PluginRefreshProperty( wxPGProperty* property );

		wxPropertyGrid* propertyGrid;

		typedef std::list< NodeControlPanel::Dialog* > DialogList;
		DialogList dialogList;
	};
}

// PropertyGridPanel.h
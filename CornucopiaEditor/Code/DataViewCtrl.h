// DataViewCtrl.h

namespace CornucopiaEditor
{
	//=================================================================================
	class DataViewCtrl : public wxDataViewCtrl
	{
		wxDECLARE_DYNAMIC_CLASS( DataViewCtrl );

	public:

		DataViewCtrl( void ) {}
		DataViewCtrl( wxWindow* parent );
		virtual ~DataViewCtrl( void );

		int GetUnusedModelColumn( void );
		bool IsModelColumnUsed( int modelCol );
		wxDataViewColumn* FindModelColumn( int modelCol );
		static wxString ColModelIndexToColName( int modelCol );
		void RebuildColumns( void );

	private:

		void OnItemActivated( wxDataViewEvent& event );
		void OnItemCollapsed( wxDataViewEvent& event );
		void OnItemExpanded( wxDataViewEvent& event );
		void OnStartEditing( wxDataViewEvent& event );
		void OnEditingStarted( wxDataViewEvent& event );
		void OnEditingDone( wxDataViewEvent& event );
		void OnContextMenu( wxDataViewEvent& event );
		void OnSelectionChanged( wxDataViewEvent& event );
		void OnColumnReordered( wxDataViewEvent& event );
		void OnColumnHeaderClick( wxDataViewEvent& event );

		void OnContextMenuCreate( wxCommandEvent& event );
		void OnContextMenuChangeType( wxCommandEvent& event );
		void OnContextMenuCut( wxCommandEvent& event );
		void OnContextMenuDeepCopy( wxCommandEvent& event );
		void OnContextMenuShallowCopy( wxCommandEvent& event );
		void OnContextMenuPasteUnder( wxCommandEvent& event );
		void OnContextMenuPasteOver( wxCommandEvent& event );
		void OnContextMenuDelete( wxCommandEvent& event );

		void OnContextMenuMoveForward( wxCommandEvent& event );
		void OnContextMenuMoveBackward( wxCommandEvent& event );

		void OnContextMenuEditName( wxCommandEvent& event );
		void OnContextMenuEditType( wxCommandEvent& event );
		void OnContextMenuEditValue( wxCommandEvent& event );
		void OnContextMenuEditShortExplanation( wxCommandEvent& event );
		void OnContextMenuEditLongExplanation( wxCommandEvent& event );

		void OnContextMenuToggleColumn( wxCommandEvent& event );
		void OnContextMenuRestoreDefaultValue( wxCommandEvent& event );
		void OnContextMenuToggleVisibility( wxCommandEvent& event );

		void OnActivateNodeControlPanel( wxCommandEvent& event );
		void OnActivateSubTreeControlPanels( wxCommandEvent& event );

		void OnDeactivateNodeControlPanel( wxCommandEvent& event );
		void OnDeactivateSubTreeControlPanels( wxCommandEvent& event );

		void OnFollowFileReference( wxCommandEvent& event );
		void OnContextSensativeHelp( wxHelpEvent& event );

	public:

		static int shownColumnFlags;

		enum
		{
			ID_ContextMenu_Create = wxID_HIGHEST,
			ID_ContextMenu_ChangeType,
			ID_ContextMenu_Cut,
			ID_ContextMenu_DeepCopy,
			ID_ContextMenu_ShallowCopy,
			ID_ContextMenu_PasteUnder,
			ID_ContextMenu_PasteOver,
			ID_ContextMenu_Delete,

			ID_ContextMenu_MoveForward,
			ID_ContextMenu_MoveBackward,

			ID_ContextMenu_EditName,
			ID_ContextMenu_EditType,
			ID_ContextMenu_EditValue,
			ID_ContextMenu_EditShortExplanation,
			ID_ContextMenu_EditLongExplanation,

			ID_ContextMenu_ToggleNameColumn,
			ID_ContextMenu_ToggleTypeColumn,
			ID_ContextMenu_ToggleValueColumn,
			ID_ContextMenu_ToggleFeedbackColumn,

			ID_ContextMenu_ToggleVisibility,

			ID_ContextMenu_ActivateNodeControlPanel,
			ID_ContextMenu_ActivateSubTreeControlPanels,

			ID_ContextMenu_DeactivateNodeControlPanel,
			ID_ContextMenu_DeactivateSubTreeControlPanels,

			ID_ContextMenu_FollowFileReference,
			
			ID_ContextMenu_RestoreDefaultValue,

			ID_ContextMenu_Highest,
		};

	private:

		DockingPanel* GetAssociatedPanel( void );

		// This routine is general purpose enough to be moved out of this class, if needed.
		static bool QueryForNodeType( const wxString& caption, wxString& nodeType );

		void NodeValueChangedByThisControl( Cornucopia::Node* node, bool updateDataViewItem, bool updatePanelCaption );

		void MoveNodeAmongSiblings( bool forward );

		DECLARE_EVENT_TABLE()
	};
}

// DataViewCtrl.h
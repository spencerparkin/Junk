// NoteBook.h

namespace CornucopiaEditor
{
	//=================================================================================
	class NoteBook : public wxAuiNotebook
	{
	public:

		NoteBook( wxWindow* parent );
		virtual ~NoteBook( void );

		void UpdateTabTitle( Panel* panel );
		void UpdateTabTitle( int pageIndex );

		Panel* FindPanel( const wxString& filePath );
		Tree* FindTree( const wxString& filePath );

	private:

		void OnPageClose( wxAuiNotebookEvent& event );
		void OnPageClosed( wxAuiNotebookEvent& event );
		void OnPageChanged( wxAuiNotebookEvent& event );
		void OnTabRightDown( wxAuiNotebookEvent& event );
		void OnContextMenu( wxCommandEvent& event );

		enum
		{
			ID_ContextMenu_DockableStyleInterface = wxID_HIGHEST,
			ID_ContextMenu_ControlTreeStyleInterface,
			ID_ContextMenu_PropertyGridStyleInterface,
		};

		int contextPageIndex;

		DECLARE_EVENT_TABLE()
	};
}

// NoteBook.h
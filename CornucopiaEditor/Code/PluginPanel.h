// PluginPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class PluginPanel : public wxPanel
	{
	public:

		PluginPanel( wxWindow* parent );
		virtual ~PluginPanel( void );

		void UpdateList( void );

	private:

		void BuildUserInterface( void );
		void RepopulateListControl( void );

		enum
		{
			ID_ContextMenu_LoadPlugin = wxID_HIGHEST,
			ID_ContextMenu_UnloadPlugin,
		};

		void OnContextMenu( wxDataViewEvent& event );
		void OnContextMenuItemSelected( wxCommandEvent& event );

		wxDataViewListCtrl* dataViewList;
		wxDataViewItem contextItem;
	};
}

// PluginPanel.h
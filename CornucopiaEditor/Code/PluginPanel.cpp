// PluginPanel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
PluginPanel::PluginPanel( wxWindow* parent ) : wxPanel( parent )
{
	dataViewList = 0;

	BuildUserInterface();
}

//=================================================================================
/*virtual*/ PluginPanel::~PluginPanel( void )
{
}

//=================================================================================
void PluginPanel::UpdateList( void )
{
	RepopulateListControl();
	dataViewList->Refresh();
}

//=================================================================================
void PluginPanel::BuildUserInterface( void )
{
	dataViewList = new wxDataViewListCtrl( this, wxID_ANY );

	dataViewList->AppendTextColumn( wxT( "Plugin" ), wxDATAVIEW_CELL_INERT );
	dataViewList->AppendTextColumn( wxT( "Author" ), wxDATAVIEW_CELL_INERT );
	dataViewList->AppendTextColumn( wxT( "Version" ), wxDATAVIEW_CELL_INERT );
	dataViewList->AppendTextColumn( wxT( "Description" ), wxDATAVIEW_CELL_INERT );
	dataViewList->AppendTextColumn( wxT( "Library Path" ), wxDATAVIEW_CELL_INERT );

	RepopulateListControl();

	wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
	sizer->Add( dataViewList, 1, wxALL | wxEXPAND, 0 );
	SetSizer( sizer );

	dataViewList->Bind( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &PluginPanel::OnContextMenu, this );
}

//=================================================================================
void PluginPanel::RepopulateListControl( void )
{
	dataViewList->DeleteAllItems();

	class PluginManagerListPopulator : public PluginManager::PluginProcessor
	{
	public:
		PluginManagerListPopulator( wxDataViewListCtrl* dataViewList ) { this->dataViewList = dataViewList; }
		virtual bool ProcessPlugin( Plugin* plugin, const Cornucopia::Path& libraryPath ) override
		{
			Plugin::Info pluginInfo;
			plugin->ProvideInfo( pluginInfo );

			wxString path = ( const char* )libraryPath;

			wxVector< wxVariant > data;
			data.push_back( wxVariant( pluginInfo.pluginName ) );
			data.push_back( wxVariant( pluginInfo.pluginAuthor ) );
			data.push_back( wxVariant( pluginInfo.pluginVersion ) );
			data.push_back( wxVariant( pluginInfo.pluginDesc ) );
			data.push_back( wxVariant( path ) );

			dataViewList->AppendItem( data );

			return true;
		}

	private:
		wxDataViewListCtrl* dataViewList;
	};

	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	PluginManagerListPopulator pluginManagerListPopulator( dataViewList );
	pluginManager->ProcessAllPlugins( &pluginManagerListPopulator );

	int columnCount = dataViewList->GetColumnCount();
	for( int columnIndex = 0; columnIndex < columnCount; columnIndex++ )
	{
		wxDataViewColumn* column = dataViewList->GetColumn( columnIndex );
		column->SetWidth( wxCOL_WIDTH_AUTOSIZE );
	}
}

//=================================================================================
void PluginPanel::OnContextMenu( wxDataViewEvent& event )
{
	contextItem = event.GetItem();

	wxMenu contextMenu;
	contextMenu.Append( ID_ContextMenu_LoadPlugin, wxT( "Load Plugin" ) );
	contextMenu.Append( ID_ContextMenu_UnloadPlugin, wxT( "Unload Plugin" ) );

	contextMenu.Bind( wxEVT_COMMAND_MENU_SELECTED, &PluginPanel::OnContextMenuItemSelected, this );

	wxPoint point = event.GetPosition();
	PopupMenu( &contextMenu, point );
}

//=================================================================================
void PluginPanel::OnContextMenuItemSelected( wxCommandEvent& event )
{
	switch( event.GetId() )
	{
		case ID_ContextMenu_LoadPlugin:
		{
			wxFileDialog fileDialog( this, wxT( "Open Cornucopia Plugin" ), wxEmptyString, wxEmptyString, wxT( "Cornucopia Plugin (*.cornucopiaplugin)|*.cornucopiaplugin" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST );
			if( fileDialog.ShowModal() == wxID_CANCEL )
				return;

			Cornucopia::Path libraryPath;
			libraryPath.SetPath( ( const char* )fileDialog.GetPath().c_str() );

			PluginManager* pluginManager = wxGetApp().GetPluginManager();
			if( !pluginManager->LoadPlugin( libraryPath ) )
				return;

			break;
		}
		case ID_ContextMenu_UnloadPlugin:
		{
			if( !contextItem.IsOk() )
				return;
			
			unsigned int row = dataViewList->ItemToRow( contextItem );
			unsigned int col = 4;
			wxString textValue = dataViewList->GetTextValue( row, col );
			Cornucopia::Path libraryPath;
			libraryPath.SetPath( ( const char* )textValue.c_str() );

			PluginManager* pluginManager = wxGetApp().GetPluginManager();
			if( !pluginManager->UnloadPlugin( libraryPath ) )
				return;

			break;
		}
	}

	RepopulateListControl();
}

// PluginPanel.cpp
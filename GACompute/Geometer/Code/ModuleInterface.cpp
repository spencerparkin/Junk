// ModuleInterface.cpp

#include "ModuleInterface.h"
#include "Application.h"

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( ModuleInterface, Interface )
}

using namespace Geometer;

ModuleInterface::ModuleInterface( void )
{
}

/*virtual*/ ModuleInterface::~ModuleInterface( void )
{
}

/*virtual*/ void ModuleInterface::FillOutRegistryEntry( RegistryEntry& entry )
{
	entry.interfaceMenuItemName = "Modules";
	entry.interfaceMenuItemHelp = "Manage Lua modules.";
}

/*virtual*/ bool ModuleInterface::CreateControls( void )
{
	dataViewCtrl = new wxDataViewCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES );

	wxObjectDataPtr< DataViewModel > dataViewModel;
	dataViewModel = new DataViewModel();
	dataViewCtrl->AssociateModel( dataViewModel.get() );

	dataViewCtrl->AppendTextColumn( "Entry", 0, wxDATAVIEW_CELL_INERT, 150 );
	dataViewCtrl->AppendTextColumn( "Description", 1, wxDATAVIEW_CELL_INERT, 300 );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( dataViewCtrl, 1, wxGROW );
	SetSizer( boxSizer );

	Bind( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &ModuleInterface::OnContextMenu, this );
	Bind( wxEVT_MENU, &ModuleInterface::OnContextMenu_LoadNewModule, this, ID_ContextMenu_LoadNewModule );
	Bind( wxEVT_MENU, &ModuleInterface::OnContextMenu_UnloadThisModule, this, ID_ContextMenu_UnloadThisModule );
	Bind( wxEVT_MENU, &ModuleInterface::OnContextMenu_ReloadThisModule, this, ID_ContextMenu_ReloadThisModule );

	return true;
}

void ModuleInterface::OnContextMenu( wxDataViewEvent& event )
{
	DataViewModel* dataViewModel = ( DataViewModel* )dataViewCtrl->GetModel();

	contextItem = event.GetItem();
	DataViewModel::Entry entry;
	if( !dataViewModel->LookupEntry( contextItem, entry ) )
		return;

	if( entry.type != DataViewModel::Entry::MODULE )
		return;

	wxMenu contextMenu;

	contextMenu.Append( ID_ContextMenu_LoadNewModule, "Load New Module" );
	contextMenu.Append( ID_ContextMenu_UnloadThisModule, "Unload This Module" );
	contextMenu.Append( ID_ContextMenu_ReloadThisModule, "Reload This Module" );

	wxPoint point = event.GetPosition();
	PopupMenu( &contextMenu, point );
}

void ModuleInterface::OnContextMenu_LoadNewModule( wxCommandEvent& event )
{
	// STPTODO: Let the user select a .lua or .dll file, then load it.  Patch up package.path or package.cpath to make sure Lua can find it.

	//TouchedLuaState();
}

void ModuleInterface::OnContextMenu_UnloadThisModule( wxCommandEvent& event )
{
	// STPTODO: This should be as simple as setting an entry in package.loaded to nil.

	//TouchedLuaState();
}

void ModuleInterface::OnContextMenu_ReloadThisModule( wxCommandEvent& event )
{
	// STPTODO: Re-use code here that is used in both the load and unload routines.
}

/*virtual*/ bool ModuleInterface::UpdateControls( void )
{
	dataViewCtrl->GetModel()->Cleared();
	return true;
}

ModuleInterface::DataViewModel::DataViewModel( void )
{
	nextId = 1;
}

/*virtual*/ ModuleInterface::DataViewModel::~DataViewModel( void )
{
}

/*virtual*/ unsigned int ModuleInterface::DataViewModel::GetColumnCount() const
{
	return 2;
}

/*virtual*/ wxString ModuleInterface::DataViewModel::GetColumnType( unsigned int col ) const
{
	return "";
}

bool ModuleInterface::DataViewModel::LookupEntry( const wxDataViewItem& item, Entry& entry ) const
{
	int id = ( int )item.m_pItem;

	ItemMap::const_iterator iter = itemMap.find( id );
	if( iter == itemMap.end() )
		return false;

	entry = iter->second;
	return true;
}

/*virtual*/ void ModuleInterface::DataViewModel::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const
{
	Entry entry;
	if( !LookupEntry( item, entry ) )
		variant = "???";
	else
	{
		if( col == 0 )
			variant = entry.label;
		else if( col == 1 )
			variant = entry.description;
	}
}

/*virtual*/ bool ModuleInterface::DataViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col )
{
	return false;
}

/*virtual*/ wxDataViewItem ModuleInterface::DataViewModel::GetParent( const wxDataViewItem& item ) const
{
	wxDataViewItem invalidItem;
	return invalidItem;
}

/*virtual*/ bool ModuleInterface::DataViewModel::IsContainer( const wxDataViewItem& item ) const
{
	if( !item.IsOk() )
		return true;

	int id = ( int )item.m_pItem;

	ItemMap::const_iterator iter = itemMap.find( id );
	if( iter == itemMap.end() )
		return false;

	const Entry& entry = iter->second;
	if( entry.type == Entry::MODULE )
		return true;

	return false;
}

void ModuleInterface::DataViewModel::MakeNewEntry( wxDataViewItem& item, const Entry& entry ) const
{
	int id = nextId++;
	itemMap[ id ] = entry;
	item.m_pItem = ( void* )id;
}

/*virtual*/ unsigned int ModuleInterface::DataViewModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const
{
	lua_State* L = wxGetApp().LuaState();
	int stackTop = lua_gettop( L );

	do
	{
		lua_getglobal( L, "package" );
		if( lua_isnil( L, -1 ) )
			break;

		lua_getfield( L, -1, "loaded" );

		if( !item.IsOk() )
		{
			nextId = 1;
			itemMap.clear();

			lua_pushnil( L );
			while( lua_next( L, -2 ) )
			{
				const char* moduleName = lua_tostring( L, -2 );

				Entry modEntry;
				modEntry.type = Entry::MODULE;
				modEntry.label = moduleName;
				modEntry.module = moduleName;
				modEntry.description = lua_typename( L, lua_type( L, -1 ) );
					
				wxDataViewItem item;
				MakeNewEntry( item, modEntry );
				children.push_back( item );

				lua_pop( L, 1 );
			}
		}
		else
		{
			Entry modEntry;
			if( !LookupEntry( item, modEntry ) )
				break;

			if( modEntry.type != Entry::MODULE )
				break;

			lua_getfield( L, -1, modEntry.module );
			if( !lua_istable( L, -1 ) )
				break;

			lua_pushnil( L );
			while( lua_next( L, -2 ) )
			{
				if( lua_isfunction( L, -1 ) )
				{
					const char* funcName = lua_tostring( L, -2 );
					
					Entry apiEntry;
					apiEntry.type = Entry::API_CALL;
					apiEntry.label = funcName;
					apiEntry.module = modEntry.module;
					apiEntry.description = lua_typename( L, lua_type( L, -1 ) );

					wxDataViewItem item;
					MakeNewEntry( item, apiEntry );
					children.push_back( item );
				}

				lua_pop( L, 1 );
			}
		}
	}
	while( false );

	while( stackTop < lua_gettop( L ) )
		lua_pop( L, 1 );

	return children.Count();
}

// ModuleInterface.cpp
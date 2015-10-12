// DataViewModel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
// Notice that the root of the given tree can be null.  The data-view control
// should be able to handle the vacuous case.
DataViewModel::DataViewModel( Tree* tree, DataViewCtrl* dataViewCtrl )
{
	this->tree = tree;

	// Maintaining a back-pointer to the control would be bad if the model
	// was applied to multiple controls or could be moved from control to
	// control, but since every control gets its own model instance, we can
	// safely maintain a back-pointer, which is useful in certain contexts.
	this->dataViewCtrl = dataViewCtrl;
}

//=================================================================================
/*virtual*/ DataViewModel::~DataViewModel( void )
{
}

//=================================================================================
// Return an item that will represent the root of the tree of the parent of the given item.
/*virtual*/ wxDataViewItem DataViewModel::GetParent( const wxDataViewItem& item ) const
{
	wxDataViewItem parentItem(0);

	// Being given an invalid item is a signal to us to return the root-level item.
	if( !item.IsOk() )
		parentItem.m_pItem = tree->GetRoot();
	else
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )item.GetID();
		Cornucopia::Node* parent = node->GetParent();
		parentItem.m_pItem = parent;
	}

	return parentItem;
}

//=================================================================================
// Return the list of items that will represent the children of the given item.
/*virtual*/ unsigned int DataViewModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const
{
	children.Clear();

	// Being given an invalid item is a signal to us to return the root-level item.
	if( !item.IsOk() )
	{
		if( tree->GetRoot() )
		{
			wxDataViewItem rootItem( tree->GetRoot() );
			children.Add( rootItem );
		}
	}
	else
	{
		const Cornucopia::Node* node = ( const Cornucopia::Node* )item.GetID();
		const Cornucopia::Node::List* nodeList = node->GetChildren();
		if( nodeList )
		{
			for( Cornucopia::Node::List::const_iterator iter = nodeList->begin(); iter != nodeList->end(); iter++ )
			{
				Cornucopia::Node* child = *iter;
				wxDataViewItem childItem( child );
				children.Add( childItem );
			}
		}
	}

	return children.Count();
}

//=================================================================================
/*virtual*/ unsigned int DataViewModel::GetColumnCount( void ) const
{
	// (Name, Type, Value, Feedback.)
	return 4;
}

//=================================================================================
/*virtual*/ wxString DataViewModel::GetColumnType( unsigned int col ) const
{
	return wxT( "void*" );
}

//=================================================================================
/*virtual*/ bool DataViewModel::IsContainer( const wxDataViewItem& item ) const
{
	if( !item.IsOk() )
		return true;
	
	const Cornucopia::Node* node = ( const Cornucopia::Node* )item.GetID();
	const Cornucopia::Node::List* nodeList = node->GetChildren();
	if( !nodeList || nodeList->size() == 0 )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ bool DataViewModel::HasContainerColumns( const wxDataViewItem& item ) const
{
	// Every node of a Cornucopia tree can have data associated with it.
	return true;
}

//=================================================================================
// The control calls this to pull for what it should display as contents of the control.
/*virtual*/ void DataViewModel::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const
{
	variant.Clear();

	const Cornucopia::Node* node = ( const Cornucopia::Node* )item.GetID();
	const EditorMetaData* editorMetaData = ( const EditorMetaData* )node->GetMetaData();

	wxString text = "?";

	switch( col )
	{
		case COL_NAME:
		{
			text = node->GetName().c_str();
			break;
		}
		case COL_TYPE:
		{
			text = node->GetType().c_str();
			break;
		}
		case COL_VALUE:
		{
			switch( editorMetaData->GetValueDisplayMode() )
			{
				case EditorMetaData::SHOW_DISPLAY_VALUE:
				{
					text = node->GetValueToDisplayString();
					break;
				}
				case EditorMetaData::SHOW_VALUE_AS_STRING:
				{
					std::string valueString;
					node->GetValueToString( valueString );
					text = ( const char* )valueString.c_str();
					break;
				}
			}
			break;
		}
		case COL_FEEDBACK:
		{
			text = editorMetaData->GetFeedback();
			break;
		}
		default:
		{
			// If we encounter a column that we don't know about, it may be owned by a plugin.
			PluginManager* pluginManager = wxGetApp().GetPluginManager();
			if( pluginManager )
			{
				PluginDataViewCtrlEvent event;
				event.SetEventType( CORNUCOPIA_PLUGIN_DATAVIEWCTRL_GET_VALUE );
				event.SetEventObject(0);
				event.SetDataViewCtrl( dataViewCtrl );
				event.SetDataViewItem( item );
				event.SetVariant( variant );
				event.SetColumn( col );
				if( pluginManager->CallAllPlugins( event ) )
				{
					variant = event.GetVariant();
					return;
				}
			}

			break;
		}
	}

	variant = text;
}

//=================================================================================
// Here the control is telling us that something has been edited, and that we
// need to now update our application file data?
/*virtual*/ bool DataViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col )
{
	switch( col )
	{
		case COL_NAME:
		case COL_TYPE:
		case COL_VALUE:
		case COL_FEEDBACK:
		{
			// I'm not really sure what to do here.
			//tree->SetModified( true );
			//...
			break;
		}
		default:
		{
			// If we encounter a column that we don't know about, it may be owned by a plugin.
			PluginManager* pluginManager = wxGetApp().GetPluginManager();
			if( pluginManager )
			{
				PluginDataViewCtrlEvent event;
				event.SetEventType( CORNUCOPIA_PLUGIN_DATAVIEWCTRL_SET_VALUE );
				event.SetEventObject(0);
				event.SetDataViewCtrl( dataViewCtrl );
				event.SetDataViewItem( item );
				event.SetVariant( variant );
				event.SetColumn( col );
				pluginManager->CallAllPlugins( event );
			}

			break;
		}
	}

	return true;
}

// DataViewModel.cpp
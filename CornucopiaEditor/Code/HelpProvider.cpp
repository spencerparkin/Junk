// HelpProvider.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
HelpProvider::HelpProvider( void )
{
}

//=================================================================================
/*virtual*/ HelpProvider::~HelpProvider( void )
{
}

//=================================================================================
/*virtual*/ bool HelpProvider::ShowHelpAtPoint( wxWindowBase* window, const wxPoint& point, wxHelpEvent::Origin origin )
{
	do
	{
		DataViewCtrl* dataViewCtrl = wxDynamicCast( window, DataViewCtrl );
		if( !dataViewCtrl )
			break;

		wxDataViewItem item;
		wxDataViewColumn* column = 0;
		dataViewCtrl->HitTest( point, item, column );
		if( !item.IsOk() )
			break;

		Cornucopia::Node* node = ( Cornucopia::Node* )item.m_pItem;
		wxString longExplanation = ( const char* )node->GetExplanation( Cornucopia::Node::LONG_EXPLANATION ).c_str();
		if( longExplanation.IsEmpty() )
			break;

		// TODO: Display the longer explanation here somehow.
		return true;
	}
	while( false );
	
	return wxHelpControllerHelpProvider::ShowHelpAtPoint( window, point, origin );
}

// HelpProvider.cpp
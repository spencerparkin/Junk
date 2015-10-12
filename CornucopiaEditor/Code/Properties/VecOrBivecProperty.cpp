// VecOrBivecProperty.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_CLASS2( VecOrBivecProperty, wxPGProperty, NodeProperty )

//=================================================================================
VecOrBivecPropertyEditor* VecOrBivecProperty::editor = 0;

//=================================================================================
/*static*/ VecOrBivecProperty* VecOrBivecProperty::Create( Cornucopia::Node* node )
{
	if( !editor )
	{
		editor = new VecOrBivecPropertyEditor();
		wxPropertyGrid::RegisterEditorClass( editor );
	}

	wxString label = node->GetName();
	return new VecOrBivecProperty( label );
}

//=================================================================================
VecOrBivecProperty::VecOrBivecProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/ ) : wxPGProperty( label, name )
{
}

//=================================================================================
/*virtual*/ VecOrBivecProperty::~VecOrBivecProperty( void )
{
}

//=================================================================================
/*virtual*/ bool VecOrBivecProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
	return true;
}

//=================================================================================
const wxPGEditor* VecOrBivecProperty::DoGetEditorClass( void ) const
{
	return editor;
}

//=================================================================================
/*virtual*/ bool VecOrBivecProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	return true;
}

//=================================================================================
/*virtual*/ bool VecOrBivecProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	return true;
}

// VecOrBivecProperty.cpp
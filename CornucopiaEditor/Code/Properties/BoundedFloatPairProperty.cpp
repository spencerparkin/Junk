// BoundedFloatPairProperty.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_CLASS2( BoundedFloatPairProperty, wxPGProperty, NodeProperty )

//=================================================================================
BoundedFloatPairPropertyEditor* BoundedFloatPairProperty::editor = 0;

//=================================================================================
/*static*/ BoundedFloatPairProperty* BoundedFloatPairProperty::Create( Cornucopia::Node* node )
{
	if( !editor )
	{
		editor = new BoundedFloatPairPropertyEditor();
		wxPropertyGrid::RegisterEditorClass( editor );
	}

	wxString label = node->GetName();
	BoundedFloatPairProperty* property = new BoundedFloatPairProperty( label );
	return property;
}

//=================================================================================
BoundedFloatPairProperty::BoundedFloatPairProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/ ) : wxPGProperty( label, name )
{
}

//=================================================================================
/*virtual*/ BoundedFloatPairProperty::~BoundedFloatPairProperty( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
	return true;
}

//=================================================================================
/*virtual*/ const wxPGEditor* BoundedFloatPairProperty::DoGetEditorClass( void ) const
{
	return editor;
}

//=================================================================================
/*virtual*/ wxString BoundedFloatPairProperty::ValueToString( wxVariant& variant, int argFlags /*= 0*/ ) const
{
	return "";
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	return true;
}

// BoundedFloatPairProperty.cpp
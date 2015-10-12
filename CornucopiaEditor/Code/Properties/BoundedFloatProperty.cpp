// BoundedFloatProperty.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_CLASS2( BoundedFloatProperty, wxPGProperty, NodeProperty )

//=================================================================================
BoundedFloatPropertyEditor* BoundedFloatProperty::editor = 0;

//=================================================================================
/*static*/ BoundedFloatProperty* BoundedFloatProperty::Create( Cornucopia::Node* node )
{
	if( !editor )
	{
		editor = new BoundedFloatPropertyEditor();
		wxPropertyGrid::RegisterEditorClass( editor );
	}

	wxString label = node->GetName();
	BoundedFloatProperty* property = new BoundedFloatProperty( label );
	return property;
}

//=================================================================================
BoundedFloatProperty::BoundedFloatProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/ ) : wxPGProperty( label, name )
{
}

//=================================================================================
/*virtual*/ BoundedFloatProperty::~BoundedFloatProperty( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
	return true;
}

//=================================================================================
/*virtual*/ const wxPGEditor* BoundedFloatProperty::DoGetEditorClass( void ) const
{
	return editor;
}

//=================================================================================
/*virtual*/ wxString BoundedFloatProperty::ValueToString( wxVariant& variant, int argFlags /*= 0*/ ) const
{
	return "";
}

//=================================================================================
/*virtual*/ bool BoundedFloatProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	// We don't use the variant for this property.
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedFloatProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	// We don't write to our node as a function of the variant data, because we're not using it.
	// Pretend we did it.
	return true;
}

// BoundedFloatProperty.cpp
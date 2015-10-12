// NodeProperty.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_ABSTRACT_CLASS( NodeProperty, wxObject )

//=================================================================================
NodeProperty::NodeProperty( void )
{
	node = 0;
}

//=================================================================================
/*virtual*/ NodeProperty::~NodeProperty( void )
{
}

//=================================================================================
void NodeProperty::SetNode( Cornucopia::Node* node )
{
	this->node = node;
}

//=================================================================================
Cornucopia::Node* NodeProperty::GetNode( void )
{
	return node;
}

//=================================================================================
/*static*/ wxPGProperty* NodeProperty::Create( Cornucopia::Node* node )
{
	wxPGProperty* property = 0;
	wxString nodeType = node->GetType();
	NodeProperty* nodeProperty = 0;

	if( nodeType == wxT( "StringNode" ) )
	{
		StringProperty* stringProperty = StringProperty::Create( node );
		property = stringProperty;
		nodeProperty = ( NodeProperty* )stringProperty;
	}
	else if( nodeType == wxT( "BoundedFloatNode" ) )
	{
		BoundedFloatProperty* boundedFloatProperty = BoundedFloatProperty::Create( node );
		property = boundedFloatProperty;
		nodeProperty = ( NodeProperty* )boundedFloatProperty;
	}
	else if( nodeType == wxT( "BoundedFloatPairNode" ) )
	{
		BoundedFloatPairProperty* boundedFloatPairProperty = BoundedFloatPairProperty::Create( node );
		property = boundedFloatPairProperty;
		nodeProperty = ( NodeProperty* )boundedFloatPairProperty;
	}
	else if( nodeType == wxT( "BooleanNode" ) )
	{
		BoolProperty* boolProperty = BoolProperty::Create( node );
		property = boolProperty;
		nodeProperty = ( NodeProperty* )boolProperty;
	}
	else if( nodeType == wxT( "SelectionNode" ) )
	{
		MultiChoiceProperty* multiChoiceProperty = MultiChoiceProperty::Create( node );
		property = multiChoiceProperty;
		nodeProperty = ( NodeProperty* )multiChoiceProperty;
	}
	else if( nodeType == wxT( "FileReferenceNode" ) )
	{
		FileProperty* fileProperty = FileProperty::Create( node );
		property = fileProperty;
		nodeProperty = ( NodeProperty* )fileProperty;
	}
	else if( nodeType == wxT( "ColorRGBANode" ) )
	{
		ColorProperty* colorProperty = ColorProperty::Create( node );
		property = colorProperty;
		nodeProperty = ( NodeProperty* )colorProperty;
	}
	else if( nodeType == wxT( "VectorE3GANode" ) || nodeType == wxT( "BivectorE3GANode" ) )
	{
		VecOrBivecProperty* vecOrBivecProperty = VecOrBivecProperty::Create( node );
		property = vecOrBivecProperty;
		nodeProperty = ( NodeProperty* )vecOrBivecProperty;
	}
	else
	{
		// TODO: Ask a plugin before defaulting?

		DefaultProperty* defaultProperty = DefaultProperty::Create( node );
		property = defaultProperty;
		nodeProperty = ( NodeProperty* )defaultProperty;
	}

	nodeProperty->SetNode( node );

	wxVariant castData( ( void* )nodeProperty );
	property->SetAttribute( wxT( "NodeProperty" ), castData );

	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	editorMetaData->SetProperty( property );

	wxString shortExplanation = node->GetExplanation( Cornucopia::Node::SHORT_EXPLANATION );
	property->SetHelpString( shortExplanation );

	return property;
}

//=================================================================================
// Is there a better way?
/*static*/ NodeProperty* NodeProperty::LameCast( wxPGProperty* property )
{
	NodeProperty* nodeProperty = 0;
	wxVariant castData = property->GetAttribute( wxT( "NodeProperty" ) );
	nodeProperty = ( NodeProperty* )castData.GetVoidPtr();
	return nodeProperty;
}

//=================================================================================
bool NodeProperty::PushNodeValueToProperty( void )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	wxPGProperty* property = editorMetaData->GetProperty();
	if( !property )
		return false;

	wxVariant variant;
	if( !PushNodeValueToVariant( variant ) )
		return false;

	property->SetValue( variant );
	return true;
}

//=================================================================================
bool NodeProperty::PullNodeValueFromProperty( void )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	wxPGProperty* property = editorMetaData->GetProperty();
	if( !property )
		return false;

	wxVariant variant = property->GetValue();
	return PullNodeValueFromVariant( variant );
}

// NodeProperty.cpp
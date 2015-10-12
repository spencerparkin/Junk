// CommonProperties.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_CLASS2( DefaultProperty, wxStringProperty, NodeProperty )

//=================================================================================
/*static*/ DefaultProperty* DefaultProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();
	return new DefaultProperty( label );
}

//=================================================================================
DefaultProperty::DefaultProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/ ) : wxStringProperty( label, name )
{
}

//=================================================================================
/*virtual*/ DefaultProperty::~DefaultProperty( void )
{
}

//=================================================================================
/*virtual*/ wxString DefaultProperty::ValueToString( wxVariant& variant, int argFlags /*= 0*/ ) const
{
	if( node && node->GetType() == "Node" && !IsExpanded() )
	{
		// This may make the interface look a little too busy, but it is more informative.
		wxString string = "";
		const Cornucopia::Node::List* children = node->GetChildren();
		for( Cornucopia::Node::List::const_iterator iter = children->begin(); iter != children->end(); iter++ )
		{
			const Cornucopia::Node* child = *iter;
			string += "[" + wxString( child->GetName() ) + "] ";
		}
		return string;
	}

	return wxStringProperty::ValueToString( variant, argFlags );
}

//=================================================================================
/*virtual*/ bool DefaultProperty::StringToValue( wxVariant& variant, const wxString& text, int argFlags /*= 0*/ ) const
{
	return wxStringProperty::StringToValue( variant, text, argFlags );
}

//=================================================================================
/*virtual*/ bool DefaultProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	std::string string;
	if( Cornucopia::Node::VSE_NONE != node->GetValueToString( string ) )
		return false;

	wxString value = string;
	variant = wxVariant( value );
	return true;
}

//=================================================================================
/*virtual*/ bool DefaultProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	wxString value = variant.GetString();
	std::string string = value;
	if( Cornucopia::Node::VSE_NONE != node->SetValueFromString( string ) )
		return false;

	return true;
}

//=================================================================================
// Reject the pending value if it is not different than the current value.
// We do this so that we do not needlessly create more history in the undo/redo system.
/*virtual*/ bool DefaultProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
	wxVariant currentValue = GetValue();
	if( currentValue == value )
		return false;
	return true;
}

//=================================================================================
wxIMPLEMENT_CLASS2( StringProperty, wxStringProperty, NodeProperty )

//=================================================================================
/*static*/ StringProperty* StringProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();
	return new StringProperty( label );
}

//=================================================================================
StringProperty::StringProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/, const wxString& value /*= wxEmptyString*/ ) : wxStringProperty( label, name, value )
{
}

//=================================================================================
/*virtual*/ StringProperty::~StringProperty( void )
{
}

//=================================================================================
/*virtual*/ bool StringProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	const Cornucopia::StringNode* stringNode = node->Cast< Cornucopia::StringNode >();
	if( !stringNode )
		return false;

	std::string string;
	if( !stringNode->GetString( string ) )
		return false;

	wxString value = ( const char* )string.c_str();
	variant = wxVariant( value );
	return true;
}

//=================================================================================
/*virtual*/ bool StringProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	Cornucopia::StringNode* stringNode = node->Cast< Cornucopia::StringNode >();
	if( !stringNode )
		return false;

	wxString string = variant.GetString();
	std::string value = ( const char* )string.c_str();
	return stringNode->SetString( value );
}

//=================================================================================
wxIMPLEMENT_CLASS2( IntProperty, wxIntProperty, NodeProperty )

//=================================================================================
/*static*/ IntProperty* IntProperty::Create( Cornucopia::Node* node )
{
	return 0;
}

//=================================================================================
IntProperty::IntProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/, long value /*= 0*/ ) : wxIntProperty( label, name, value )
{
}

//=================================================================================
/*virtual*/ IntProperty::~IntProperty( void )
{
}

//=================================================================================
/*virtual*/ bool IntProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	return false;
}

//=================================================================================
/*virtual*/ bool IntProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	return false;
}

//=================================================================================
wxIMPLEMENT_CLASS2( FloatProperty, wxFloatProperty, NodeProperty )

//=================================================================================
/*static*/ FloatProperty* FloatProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();
	return new FloatProperty( label );
}

//=================================================================================
FloatProperty::FloatProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/, double value /*= 0.0*/ ) : wxFloatProperty( label, name, value )
{
}

//=================================================================================
/*virtual*/ FloatProperty::~FloatProperty( void )
{
}

//=================================================================================
/*virtual*/ bool FloatProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	const Cornucopia::BoundedFloatNode* boundedFloatNode = node->Cast< Cornucopia::BoundedFloatNode >();
	if( !boundedFloatNode )
		return false;

	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );
	
	variant = wxVariant( data.value );
	return true;
}

//=================================================================================
/*virtual*/ bool FloatProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	Cornucopia::BoundedFloatNode* boundedFloatNode = node->Cast< Cornucopia::BoundedFloatNode >();
	if( !boundedFloatNode )
		return false;

	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	data.value = variant.GetDouble();
	if( !data.IsValid() )
		return false;

	return boundedFloatNode->Set( data );
}

//=================================================================================
/*virtual*/ bool FloatProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
	if( !wxFloatProperty::ValidateValue( value, validationInfo ) )
		return false;

	if( !node )
		return false;

	Cornucopia::BoundedFloatNode* boundedFloatNode = node->Cast< Cornucopia::BoundedFloatNode >();
	if( !boundedFloatNode )
		return false;

	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	data.value = value.GetDouble();
	if( !data.IsValid() )
		return false;

	return true;
}

//=================================================================================
wxIMPLEMENT_CLASS2( BoolProperty, wxBoolProperty, NodeProperty )

//=================================================================================
/*static*/ BoolProperty* BoolProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();
	return new BoolProperty( label );
}

//=================================================================================
BoolProperty::BoolProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/, bool value /*= false*/ ) : wxBoolProperty( label, name, value )
{
}

//=================================================================================
/*virtual*/ BoolProperty::~BoolProperty( void )
{
}

//=================================================================================
/*virtual*/ bool BoolProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	Cornucopia::BooleanNode* booleanNode = node->Cast< Cornucopia::BooleanNode >();
	if( !booleanNode )
		return false;

	variant = wxVariant( booleanNode->GetBoolean() );
	return true;
}

//=================================================================================
/*virtual*/ bool BoolProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	Cornucopia::BooleanNode* booleanNode = node->Cast< Cornucopia::BooleanNode >();
	if( !booleanNode )
		return false;

	booleanNode->SetBoolean( variant.GetBool() );
	return true;
}

//=================================================================================
wxIMPLEMENT_CLASS2( MultiChoiceProperty, wxMultiChoiceProperty, NodeProperty )

//=================================================================================
/*static*/ MultiChoiceProperty* MultiChoiceProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();

	Cornucopia::SelectionNode* selectionNode = node->Cast< Cornucopia::SelectionNode >();
	if( !selectionNode )
		return 0;

	wxPGChoices choices;
	const Cornucopia::SelectionNode::SelectionMap& selectionMap = selectionNode->GetSelectionMap();
	Cornucopia::SelectionNode::SelectionMap::const_iterator iter = selectionMap.begin();
	while( iter != selectionMap.end() )
	{
		wxString choiceLabel = iter->first;
		int choiceValue = iter->second ? 1 : 0;
		choices.Add( choiceLabel, choiceValue );
		iter++;
	}

	MultiChoiceProperty* multiChoiceProperty = new MultiChoiceProperty( label, label, choices );
	return multiChoiceProperty;
}

//=================================================================================
MultiChoiceProperty::MultiChoiceProperty( const wxString& label, const wxString& name, const wxPGChoices& choices ) : wxMultiChoiceProperty( label, name, choices )
{
}

//=================================================================================
/*virtual*/ MultiChoiceProperty::~MultiChoiceProperty( void )
{
}

//=================================================================================
/*virtual*/ bool MultiChoiceProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	Cornucopia::SelectionNode* selectionNode = node->Cast< Cornucopia::SelectionNode >();
	if( !selectionNode )
		return false;

	wxArrayString selection;
	const Cornucopia::SelectionNode::SelectionMap& selectionMap = selectionNode->GetSelectionMap();
	for( Cornucopia::SelectionNode::SelectionMap::const_iterator iter = selectionMap.begin(); iter != selectionMap.end(); iter++ )
		if( iter->second )
			selection.Add( iter->first );

	variant = wxVariant( selection );
	return true;
}

//=================================================================================
/*virtual*/ bool MultiChoiceProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	Cornucopia::SelectionNode* selectionNode = node->Cast< Cornucopia::SelectionNode >();
	if( !selectionNode )
		return false;

	wxArrayString selection = variant.GetArrayString();
	Cornucopia::SelectionNode::Mode mode = selectionNode->GetMode();
	if( selection.Count() != 1 && mode == Cornucopia::SelectionNode::SINGLE_SELECT )
		return false;

	selectionNode->SetMode( Cornucopia::SelectionNode::MULTI_SELECT );

	const Cornucopia::SelectionNode::SelectionMap& selectionMap = selectionNode->GetSelectionMap();
	for( Cornucopia::SelectionNode::SelectionMap::const_iterator iter = selectionMap.begin(); iter != selectionMap.end(); iter++ )
		selectionNode->Select( iter->first, false );

	for( unsigned int index = 0; index < selection.Count(); index++ )
		selectionNode->Select( ( const char* )selection[ index ].c_str(), true );

	selectionNode->SetMode( mode );
	return true;
}

//=================================================================================
/*virtual*/ bool MultiChoiceProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
	if( !node )
		return false;

	Cornucopia::SelectionNode* selectionNode = node->Cast< Cornucopia::SelectionNode >();
	if( !selectionNode )
		return false;

	wxArrayString selection = value.GetArrayString();
	if( selection.Count() != 1 && selectionNode->GetMode() == Cornucopia::SelectionNode::SINGLE_SELECT )
	{
		validationInfo.SetFailureMessage( wxT( "Please just select one and only one of the choices given." ) );
		return false;
	}

	return true;
}

//=================================================================================
wxIMPLEMENT_CLASS2( FileProperty, wxFileProperty, NodeProperty )

//=================================================================================
/*static*/ FileProperty* FileProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();
	return new FileProperty( label );
}

//=================================================================================
FileProperty::FileProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/, const wxString& value /*= wxEmptyString*/ ) : wxFileProperty( label, name, value )
{
}

//=================================================================================
/*virtual*/ FileProperty::~FileProperty( void )
{
}

//=================================================================================
/*virtual*/ bool FileProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	const Cornucopia::FileReferenceNode* fileReferenceNode = node->Cast< Cornucopia::FileReferenceNode >();
	if( !fileReferenceNode )
		return false;

	Cornucopia::Path path;
	if( !fileReferenceNode->GetFilePath( path ) )
		return false;

	wxString string = ( const char* )path;
	variant = wxVariant( string );
	return true;
}

//=================================================================================
/*virtual*/ bool FileProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	Cornucopia::FileReferenceNode* fileReferenceNode = node->Cast< Cornucopia::FileReferenceNode >();
	if( !fileReferenceNode )
		return false;

	wxString string = variant.GetString();
	Cornucopia::Path path( ( const char* )string.c_str() );
	if( !fileReferenceNode->SetFilePath( path ) )
		return false;

	return true;
}

//=================================================================================
wxIMPLEMENT_CLASS2( ColorProperty, wxColourProperty, NodeProperty )

//=================================================================================
/*static*/ ColorProperty* ColorProperty::Create( Cornucopia::Node* node )
{
	wxString label = node->GetName();
	ColorProperty* colorProperty = new ColorProperty( label );
	colorProperty->SetAttribute( wxPG_COLOUR_HAS_ALPHA, wxVariant( true ) );
	return colorProperty;
}

//=================================================================================
ColorProperty::ColorProperty( const wxString& label /*= wxPG_LABEL*/, const wxString& name /*= wxPG_LABEL*/, const wxColour& value /*= *wxWHITE*/ ) : wxColourProperty( label, name, value )
{
}

//=================================================================================
/*virtual*/ ColorProperty::~ColorProperty( void )
{
}

//=================================================================================
/*virtual*/ bool ColorProperty::PushNodeValueToVariant( wxVariant& variant ) const
{
	if( !node )
		return false;

	const Cornucopia::ColorRGBANode* colorNode = node->Cast< Cornucopia::ColorRGBANode >();
	if( !colorNode )
		return false;

	Cornucopia::ColorRGBANode::Color inColor;
	if( !colorNode->GetColor( inColor ) )
		return false;

	wxUint32 r, g, b, a;
	r = wxUint32( inColor.r * 255.0 );
	g = wxUint32( inColor.g * 255.0 );
	b = wxUint32( inColor.b * 255.0 );
	a = wxUint32( inColor.a * 255.0 );
	wxColour outColor;
	outColor.SetRGBA( ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | r );
	
	variant << outColor;
	return true;
}

//=================================================================================
/*virtual*/ bool ColorProperty::PullNodeValueFromVariant( const wxVariant& variant )
{
	if( !node )
		return false;

	Cornucopia::ColorRGBANode* colorNode = node->Cast< Cornucopia::ColorRGBANode >();
	if( !colorNode )
		return false;

	wxColour inColor;
	inColor << variant;

	Cornucopia::ColorRGBANode::Color outColor;
	outColor.r = double( inColor.Red() ) / 255.0;
	outColor.g = double( inColor.Green() ) / 255.0;
	outColor.b = double( inColor.Blue() ) / 255.0;
	outColor.a = double( inColor.Alpha() ) / 255.0;
	
	return colorNode->SetColor( outColor );
}

// CommonProperties.cpp
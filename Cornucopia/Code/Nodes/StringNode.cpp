// StringNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( StringNode, Node );

//=================================================================================
StringNode::StringNode( void )
{
	string = "";
}

//=================================================================================
/*virtual*/ StringNode::~StringNode( void )
{
}

//=================================================================================
/*virtual*/ bool StringNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !Node::ReadFromTable( L, context ) )
		return context.IssueError( "StringNode failed to read base-class portion." );

	if( !ReadString( L, context, "stringValue", string, "???" ) )
		return context.IssueError( "StringNode \"%s\" failed to read \"stringValue\", because it does not exist or is not a string.", GetName().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool StringNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "StringNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	WriteString( L, context, "stringValue", string );
	return true;
}

//=================================================================================
/*virtual*/ bool StringNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "StringNode \"%s\" failed to copy base-class portion.", GetName().c_str() );

	const StringNode* stringNode = ( const StringNode* )node;
	string = stringNode->string;
	return true;
}

//=================================================================================
bool StringNode::SetString( const std::string& string )
{
	this->string = string;
	return true;
}

//=================================================================================
bool StringNode::GetString( std::string& string ) const
{
	string = this->string;
	return true;
}

//=================================================================================
/*virtual*/ std::string StringNode::GetValueToDisplayString( void ) const
{
	std::string displayString;
	GetValueToString( displayString );
	return displayString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError StringNode::GetValueToString( std::string& valueString ) const
{
	valueString = string;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError StringNode::SetValueFromString( const std::string& valueString )
{
	if( string == valueString )
		return VSE_NO_CHANGE;

	string = valueString;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError StringNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Enter any string (i.e. sequence of characters.)";
	return VSE_NONE;
}

// StringNode.cpp
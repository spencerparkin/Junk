// BooleanNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BooleanNode, Node );

//=================================================================================
BooleanNode::BooleanNode( void )
{
	value = false;
}

//=================================================================================
/*virtual*/ BooleanNode::~BooleanNode( void )
{
}

//=================================================================================
/*virtual*/ bool BooleanNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !Node::ReadFromTable( L, context ) )
		return context.IssueError( "BooleanNode failed to read Lua table for base class portion." );

	if( !ReadBoolean( L, context, "booleanValue", value, false ) )
		return context.IssueError( "Node \"%s\" of type \"%s\" failed to read it's boolean value, because it doesn't exist or is not a boolean.", GetName().c_str(), GetType().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool BooleanNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BooleanNode failed to write Lua table for base class portion." );

	WriteBoolean( L, context, "booleanValue", value );
	return true;
}

//=================================================================================
/*virtual*/ bool BooleanNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "BooleanNode failed to copy base class portion." );
	
	const BooleanNode* booleanNode = ( const BooleanNode* )node;
	value = booleanNode->value;
	return true;
}

//=================================================================================
void BooleanNode::SetBoolean( bool value )
{
	this->value = value;
}

//=================================================================================
bool BooleanNode::GetBoolean( void ) const
{
	return value;
}

//=================================================================================
/*virtual*/ std::string BooleanNode::GetValueToDisplayString( void ) const
{
	std::string displayString;
	GetValueToString( displayString );
	return displayString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BooleanNode::GetValueToString( std::string& valueString ) const
{
	if( value )
		valueString = "true";
	else
		valueString = "false";
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BooleanNode::SetValueFromString( const std::string& valueString )
{
	if( valueString == "true" || valueString == "1" )
	{
		if( value == true )
			return VSE_NO_CHANGE;
		value = true;
	}
	else if( valueString == "false" || valueString == "0" )
	{
		if( value == false )
			return VSE_NO_CHANGE;
		value = false;
	}
	else
		return VSE_INVALID;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BooleanNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Enter \"true\" or \"false\", \"1\" or \"0\", no quotes needed.";
	return VSE_NONE;
}

// BooleanNode.cpp
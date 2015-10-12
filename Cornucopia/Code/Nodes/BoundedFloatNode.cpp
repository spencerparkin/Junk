// BoundedFloatNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BoundedFloatNode, Node );

//=================================================================================
BoundedFloatNode::BoundedFloatNode( void )
{
	data.min = 0.0;
	data.max = 1.0;
	data.value = 0.5;
}

//=================================================================================
/*virtual*/ BoundedFloatNode::~BoundedFloatNode( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatNode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		// Read the base-class info.
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "BoundedFloatNode failed to read base-class portion of node." );

		// Read the minimum.
		if( !ReadNumber( L, context, "min", data.min, 0.0 ) )
			throw new Error( "BoundedFloatNode \"%s\" failed to read \"min\", because it doesn't exist or is not a number.", GetName().c_str() );
		
		// Read the maximum.
		if( !ReadNumber( L, context, "max", data.max, data.min + 1.0 ) )
			throw new Error( "BoundedFloatNode \"%s\" failed to read \"max\", because it doesn't exist or is not a number.", GetName().c_str() );

		// Read the value.
		if( !ReadNumber( L, context, "value", data.value, 0.5 * ( data.max - data.min ) ) )
			throw new Error( "BoundedFloatNode \"%s\" failed to read \"value\", because it doesn't exist or is not a number.", GetName().c_str() );

		// Does the data make sense?
		if( !data.IsValid() )
			throw new Error( "BoundedFloatNode \"%s\" failed to read its contents, because its contents is invalid.  ( min = %f, max = %f, value = %f )", GetName().c_str(), data.min, data.max, data.value );

		// All is well.
		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	// Make sure the stack is restored to its original state.
	lua_settop( L, top );

	return success;
}

//=================================================================================
/*virtual*/ bool BoundedFloatNode::WriteToTable( lua_State* L, Context& context ) const
{
	// Write the base class info.
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BoundedFloatNode \"%s\" failed to write base-class portion of node.", GetName().c_str() );

	// Now write our info.
	WriteNumber( L, context, "min", data.min );
	WriteNumber( L, context, "max", data.max );
	WriteNumber( L, context, "value", data.value );
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedFloatNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "BoundedFloatNode \"%s\" failed to copy base-class portion of node.", GetName().c_str() );

	const BoundedFloatNode* boundFloatNode = ( const BoundedFloatNode* )node;
	data.min = boundFloatNode->data.min;
	data.max = boundFloatNode->data.max;
	data.value = boundFloatNode->data.value;
	return true;
}

//=================================================================================
bool BoundedFloatNode::Data::IsValid( void ) const
{
	if( min <= value && value <= max )
		return true;
	return false;
}

//=================================================================================
bool BoundedFloatNode::Data::operator==( const Data& data ) const
{
	if( min != data.min )
		return false;
	if( max != data.max )
		return false;
	if( value != data.value )
		return false;
	return true;
}

//=================================================================================
bool BoundedFloatNode::Set( const Data& data )
{
	if( !data.IsValid() )
		return false;

	this->data = data;
	return true;
}

//=================================================================================
void BoundedFloatNode::Get( Data& data ) const
{
	data = this->data;
}

//=================================================================================
/*virtual*/ std::string BoundedFloatNode::GetValueToDisplayString( void ) const
{
	char valueString[ 256 ];
	sprintf_s( valueString, sizeof( valueString ), "%1.2f in [%1.2f,%1.2f]", data.value, data.min, data.max );
	return valueString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedFloatNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "min" ] = data.min;
	varMap[ "max" ] = data.max;
	varMap[ "value" ] = data.value;

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedFloatNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	Data trialData = data;
	if( varMap.end() != varMap.find( "min" ) )
		trialData.min = varMap[ "min" ];
	if( varMap.end() != varMap.find( "max" ) )
		trialData.max = varMap[ "max" ];
	if( varMap.end() != varMap.find( "value" ) )
		trialData.value = varMap[ "value" ];

	if( !trialData.IsValid() )
		return VSE_INVALID;

	if( trialData == data )
		return VSE_NO_CHANGE;

	data = trialData;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedFloatNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: min, max, value";
	return VSE_NONE;
}

// BoundedFloatNode.cpp
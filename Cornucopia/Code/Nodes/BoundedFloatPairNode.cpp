// BoundedFloatPairNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BoundedFloatPairNode, Node );

//=================================================================================
BoundedFloatPairNode::BoundedFloatPairNode( void )
{
	data.min = 0.0;
	data.max = 1.0;
	data.minValue = 0.0;
	data.maxValue = 1.0;
}

//=================================================================================
/*virtual*/ BoundedFloatPairNode::~BoundedFloatPairNode( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairNode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		// Read the base-class info.
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "BoundedFloatPairNode failed to read base-class portion of node." );

		// Read the minimum.
		if( !ReadNumber( L, context, "min", data.min, 0.0 ) )
			throw new Error( "BoundedFloatPairNode \"%s\" failed to read \"min\".", GetName().c_str() );

		// Read the maximum.
		if( !ReadNumber( L, context, "max", data.max, data.min + 1.0 ) )
			throw new Error( "BoundedFloatPairNode \"%s\" failed to read \"max\".", GetName().c_str() );

		// Read the minimum value.
		if( !ReadNumber( L, context, "minValue", data.minValue, data.min ) )
			throw new Error( "BoundedFloatPairNode \"%s\" failed to read \"minValue\".", GetName().c_str() );

		// Read the maximum value.
		if( !ReadNumber( L, context, "maxValue", data.maxValue, data.max ) )
			throw new Error( "BoundedFloatPairNode \"%s\" failed to read \"maxValue\".", GetName().c_str() );

		// Does the data make sense?
		if( !data.IsValid() )
			throw new Error( "BoundedFloatPairNode \"%s\" failed to read, because it's content are invalid.  ( min = %f, max = %f, minValue = %f, maxValue = %f )", GetName().c_str(), data.min, data.max, data.minValue, data.maxValue );

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
/*virtual*/ bool BoundedFloatPairNode::WriteToTable( lua_State* L, Context& context ) const
{
	// Write the base class info.
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BoundedFloatPairNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	// Now write our info.
	WriteNumber( L, context, "min", data.min );
	WriteNumber( L, context, "max", data.max );
	WriteNumber( L, context, "minValue", data.minValue );
	WriteNumber( L, context, "minValue", data.maxValue );
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "BoundedFloatPairNode \"%s\" failed to copy base-class portion.", GetName().c_str() );

	const BoundedFloatPairNode* boundFloatPairNode = ( const BoundedFloatPairNode* )node;
	data.min = boundFloatPairNode->data.min;
	data.max = boundFloatPairNode->data.max;
	data.minValue = boundFloatPairNode->data.minValue;
	data.maxValue = boundFloatPairNode->data.maxValue;
	return true;
}

//=================================================================================
bool BoundedFloatPairNode::Data::IsValid( void ) const
{
	if( min <= minValue && minValue <= maxValue && maxValue <= max )
		return true;
	return false;
}

//=================================================================================
bool BoundedFloatPairNode::Data::operator==( const Data& data ) const
{
	if( min != data.min )
		return false;
	if( max != data.max )
		return false;
	if( minValue != data.minValue )
		return false;
	if( maxValue != data.maxValue )
		return false;
	return true;
}

//=================================================================================
bool BoundedFloatPairNode::Set( const Data& data )
{
	if( !data.IsValid() )
		return false;

	this->data = data;
	return true;
}

//=================================================================================
void BoundedFloatPairNode::Get( Data& data ) const
{
	data = this->data;
}

//=================================================================================
/*virtual*/ std::string BoundedFloatPairNode::GetValueToDisplayString( void ) const
{
	char valueString[ 256 ];
	sprintf_s( valueString, sizeof( valueString ), "[%1.2f,%1.2f] in [%1.2f,%1.2f]", data.minValue, data.maxValue, data.min, data.max );
	return valueString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedFloatPairNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "min" ] = data.min;
	varMap[ "max" ] = data.max;
	varMap[ "minValue" ] = data.minValue;
	varMap[ "maxValue" ] = data.maxValue;

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedFloatPairNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	Data trialData = data;
	if( varMap.end() != varMap.find( "min" ) )
		trialData.min = varMap[ "min" ];
	if( varMap.end() != varMap.find( "max" ) )
		trialData.max = varMap[ "max" ];
	if( varMap.end() != varMap.find( "minValue" ) )
		trialData.minValue = varMap[ "minValue" ];
	if( varMap.end() != varMap.find( "maxValue" ) )
		trialData.maxValue = varMap[ "maxValue" ];

	if( !trialData.IsValid() )
		return VSE_INVALID;

	if( trialData == data )
		return VSE_NO_CHANGE;

	data = trialData;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedFloatPairNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: min, max, minValue, maxValue";
	return VSE_NONE;
}

// BoundedFloatPairNode.cpp
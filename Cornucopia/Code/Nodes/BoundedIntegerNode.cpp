// BoundedIntegerNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BoundedIntegerNode, Node );

//=================================================================================
BoundedIntegerNode::BoundedIntegerNode( void )
{
	data.min = 0;
	data.max = 10;
	data.value = 0;
	data.multiple = 1;
}

//=================================================================================
/*virtual*/ BoundedIntegerNode::~BoundedIntegerNode( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedIntegerNode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		// Read the base-class info.
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "BoundedIntegerNode failed to read base-class portion of node." );

		// Read the minimum.
		if( !ReadInteger( L, context, "min", data.min, 0 ) )
			throw new Error( "BoundedIntegerNode \"%s\" failed to read \"min\", because it doesn't exist or is not an integer.", GetName().c_str() );
		
		// Read the maximum.
		if( !ReadInteger( L, context, "max", data.max, data.min + 10 ) )
			throw new Error( "BoundedIntegerNode \"%s\" failed to read \"max\", because it doesn't exist or is not an integer.", GetName().c_str() );

		// Read the value.
		if( !ReadInteger( L, context, "value", data.value, data.min ) )
			throw new Error( "BoundedIntegerNode \"%s\" failed to read \"value\", because it doesn't exist or is not an integer.", GetName().c_str() );

		// Does the data make sense?
		if( !data.IsValid() )
			throw new Error( "BoundedIntegerNode \"%s\" failed to read its contents, because its contents is invalid.  ( min = %d, max = %d, value = %d, multiple = %d )", GetName().c_str(), data.min, data.max, data.value, data.multiple );

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
/*virtual*/ bool BoundedIntegerNode::WriteToTable( lua_State* L, Context& context ) const
{
	// Write the base class info.
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BoundedIntegerNode \"%s\" failed to write base-class portion of node.", GetName().c_str() );

	// Now write our info.
	WriteInteger( L, context, "min", data.min );
	WriteInteger( L, context, "max", data.max );
	WriteInteger( L, context, "value", data.value );
	WriteInteger( L, context, "multiple", data.multiple );
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedIntegerNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "BoundedIntegerNode \"%s\" failed to copy base-class portion of node.", GetName().c_str() );

	const BoundedIntegerNode* boundedIntegerNode = ( const BoundedIntegerNode* )node;
	data.min = boundedIntegerNode->data.min;
	data.max = boundedIntegerNode->data.max;
	data.value = boundedIntegerNode->data.value;
	data.multiple = boundedIntegerNode->data.multiple;
	return true;
}

//=================================================================================
bool BoundedIntegerNode::Data::IsValid( void ) const
{
	if( multiple <= 0 || ( max - min ) % multiple != 0 )
		return false;
	if( value < min || value > max )
		return false;
	if( ( value - min ) % multiple != 0 )
		return false;
	return true;
}

//=================================================================================
bool BoundedIntegerNode::Data::operator==( const Data& data ) const
{
	if( min != data.min )
		return false;
	if( max != data.max )
		return false;
	if( value != data.value )
		return false;
	if( multiple != data.multiple )
		return false;
	return true;
}

//=================================================================================
int BoundedIntegerNode::Data::Cardinality( void ) const
{
	return ( max - min ) / multiple + 1;
}

//=================================================================================
bool BoundedIntegerNode::Set( const Data& data )
{
	if( !data.IsValid() )
		return false;

	this->data = data;
	return true;
}

//=================================================================================
void BoundedIntegerNode::Get( Data& data ) const
{
	data = this->data;
}

//=================================================================================
/*virtual*/ std::string BoundedIntegerNode::GetValueToDisplayString( void ) const
{
	char valueString[ 256 ];
	if( !data.IsValid() )
		sprintf_s( valueString, sizeof( valueString ), "Invalid data!  (min=%d, max=%d, value=%d, multiple=%d)", data.min, data.max, data.value, data.multiple );
	else if( data.Cardinality() > 3 )
	{
		if( data.min > 0 )
			sprintf_s( valueString, sizeof( valueString ), "%d in {%d*i+%d|i=0,1,2,...,%d}", data.value, data.multiple, data.min, data.Cardinality() - 1 );
		else if( data.min < 0 )
			sprintf_s( valueString, sizeof( valueString ), "%d in {%d*i%d|i=0,1,2,...,%d}", data.value, data.multiple, data.min, data.Cardinality() - 1 );
		else if( data.min == 0 )
			sprintf_s( valueString, sizeof( valueString ), "%d in {%d*i|i=0,1,2,...,%d}", data.value, data.multiple, data.Cardinality() - 1 );
	}
	else
	{
		if( data.min > 0 )
			sprintf_s( valueString, sizeof( valueString ), "%d in {%d*i+%d|i=0,...,%d}", data.value, data.multiple, data.min, data.Cardinality() - 1 );
		else if( data.min < 0 )
			sprintf_s( valueString, sizeof( valueString ), "%d in {%d*i%d|i=0,...,%d}", data.value, data.multiple, data.min, data.Cardinality() - 1 );
		else if( data.min == 0 )
			sprintf_s( valueString, sizeof( valueString ), "%d in {%d*i|i=0,...,%d}", data.value, data.multiple, data.Cardinality() - 1 );
	}
	return valueString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedIntegerNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "min" ] = data.min;
	varMap[ "max" ] = data.max;
	varMap[ "value" ] = data.value;
	varMap[ "multiple" ] = data.multiple;

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedIntegerNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	Data trialData = data;
	if( varMap.end() != varMap.find( "min" ) )
		trialData.min = int( varMap[ "min" ] );
	if( varMap.end() != varMap.find( "max" ) )
		trialData.max = int( varMap[ "max" ] );
	if( varMap.end() != varMap.find( "value" ) )
		trialData.value = int( varMap[ "value" ] );
	if( varMap.end() != varMap.find( "multiple" ) )
		trialData.multiple = int( varMap[ "multiple" ] );

	if( !trialData.IsValid() )
		return VSE_INVALID;

	if( trialData == data )
		return VSE_NO_CHANGE;

	data = trialData;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedIntegerNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: min, max, value, multiple";
	return VSE_NONE;
}

//=================================================================================
/*static*/ int BoundedIntegerNode::RoundDownToNearestMultiple( int integer, int multiple )
{
	int remainder = integer % multiple;
	return integer - remainder;
}

//=================================================================================
/*static*/ int BoundedIntegerNode::RoundUpToNearestMultiple( int integer, int multiple )
{
	int remainder = integer % multiple;
	return integer + multiple - remainder;
}

//=================================================================================
/*static*/ int BoundedIntegerNode::RoundToNearestMultiple( int integer, int multiple )
{
	int remainder = integer % multiple;
	if( remainder <= multiple / 2 )
		return integer - remainder;
	return integer + multiple - remainder;
}

// BoundedIntegerNode.cpp
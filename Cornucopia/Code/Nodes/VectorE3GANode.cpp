// VectorE3GANode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( VectorE3GANode, Node );

//=================================================================================
VectorE3GANode::VectorE3GANode( void )
{
	vector = new c3ga::vectorE3GA();

	vector->set_e1( 0.0 );
	vector->set_e2( 0.0 );
	vector->set_e3( 0.0 );

	maxLength = 10.0;
}

//=================================================================================
/*virtual*/ VectorE3GANode::~VectorE3GANode( void )
{
	delete vector;
}

//=================================================================================
/*virtual*/ bool VectorE3GANode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "VectorE3GANode failed to read base-class portion of node." );

		if( !ReadVectorE3GA( L, context, "vector", *vector ) )
			throw new Error( "VectorE3GANode \"%s\" failed to read \"vector\".", GetName().c_str() );

		if( !ReadNumber( L, context, "maxLength", maxLength, c3ga::norm( *vector ) ) )
			throw new Error( "VectorE3GANode \"%s\" failed to read \"maxLength\".", GetName().c_str() );
		
		//if( c3ga::norm( *vector ) > maxLength )
		//	throw new Error( "VectorE3GANode \"%s\" failed to read its data, because the data is invalid.", GetName().c_str() );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
/*virtual*/ bool VectorE3GANode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "VectorE3GANode \"%s\" failed to write base-class portion.", GetName().c_str() );

	if( !WriteVectorE3GA( L, context, "vector", *vector ) )
		return context.IssueError( "VectorE3GANode \"%s\" failed to write \"vector\".", GetName().c_str() );

	WriteNumber( L, context, "maxLength", maxLength );
	return true;
}

//=================================================================================
/*virtual*/ bool VectorE3GANode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return false;

	const VectorE3GANode* vectorNode = ( const VectorE3GANode* )node;
	*vector = *vectorNode->vector;
	maxLength = vectorNode->maxLength;
	return true;
}

//=================================================================================
/*virtual*/ std::string VectorE3GANode::GetValueToDisplayString( void ) const
{
	char formattedString[ 256 ];
	sprintf_s( formattedString, sizeof( formattedString ), "< %1.2f, %1.2f, %1.2f > (maxLength: %1.2f)", vector->get_e1(), vector->get_e2(), vector->get_e3(), maxLength );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError VectorE3GANode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "x" ] = vector->get_e1();
	varMap[ "y" ] = vector->get_e2();
	varMap[ "z" ] = vector->get_e3();
	varMap[ "maxLength" ] = maxLength;

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError VectorE3GANode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	// Get the new vector.
	c3ga::vectorE3GA trialVector = *vector;
	if( varMap.end() != varMap.find( "x" ) )
		trialVector.set_e1( varMap[ "x" ] );
	if( varMap.end() != varMap.find( "y" ) )
		trialVector.set_e2( varMap[ "y" ] );
	if( varMap.end() != varMap.find( "z" ) )
		trialVector.set_e3( varMap[ "z" ] );

	// Validate the new vector.
	if( c3ga::norm( trialVector ) > maxLength )
		return VSE_INVALID;

	// Get the new max-length.
	double trialMaxLength = maxLength;
	if( varMap.end() != varMap.find( "maxLength" ) )
		trialMaxLength = varMap[ "maxLength" ];

	// Validate the new max-length.
	if( trialMaxLength <= 0.0 )
		return VSE_INVALID;
	
	// Reject if no change occurred.
	if( c3ga::equals( trialVector, *vector, 0.0 ) && trialMaxLength == maxLength )
		return VSE_NO_CHANGE;

	// Accept the new values.
	*vector = trialVector;
	maxLength = trialMaxLength;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError VectorE3GANode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: x, y, z, maxLength";
	return VSE_NONE;
}

//=================================================================================
c3ga::vectorE3GA& VectorE3GANode::GetVector( void )
{
	return *vector;
}

//=================================================================================
const c3ga::vectorE3GA& VectorE3GANode::GetVector( void ) const
{
	return *vector;
}

//=================================================================================
bool VectorE3GANode::SetVector( const c3ga::vectorE3GA& vector, bool rescaleIfNeeded /*= false*/ )
{
	double norm = c3ga::norm( vector );
	if( norm <= maxLength )
		*this->vector = vector;
	else if( !rescaleIfNeeded )
		return false;
	else
		*this->vector = c3ga::gp( vector, 1.0 / norm );
	return true;
}

//=================================================================================
double VectorE3GANode::GetMaxLength( void ) const
{
	return maxLength;
}

//=================================================================================
void VectorE3GANode::SetMaxLength( double maxLength )
{
	this->maxLength = maxLength;
}

// VectorE3GANode.cpp
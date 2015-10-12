// BivectorE3GANode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BivectorE3GANode, Node );

//=================================================================================
BivectorE3GANode::BivectorE3GANode( void )
{
	bivector = new c3ga::bivectorE3GA();

	bivector->set_e1_e2( 0.0 );
	bivector->set_e2_e3( 0.0 );
	bivector->set_e3_e1( 0.0 );

	maxArea = 10.0;
}

//=================================================================================
/*virtual*/ BivectorE3GANode::~BivectorE3GANode( void )
{
	delete bivector;
}

//=================================================================================
/*virtual*/ bool BivectorE3GANode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "BivectorE3GANode failed to read base-class portion of node." );

		if( !ReadBivectorE3GA( L, context, "bivector", *bivector ) )
			throw new Error( "BivectorE3GANode \"%s\" failed to read \"bivector\".", GetName().c_str() );

		if( !ReadNumber( L, context, "maxArea", maxArea, c3ga::norm( *bivector ) ) )
			throw new Error( "BivectorE3GANode \"%s\" failed to read \"maxArea\".", GetName().c_str() );

		//if( c3ga::norm( *bivector ) > maxArea )
		//	throw new Error( "BivectorE3GANode \"%s\" failed to read its data, because the data is invalid.", GetName().c_str() );

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
/*virtual*/ bool BivectorE3GANode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BivectorE3GANode \"%s\" failed to write base-class portion.", GetName().c_str() );

	if( !WriteBivectorE3GA( L, context, "bivector", *bivector ) )
		return context.IssueError( "BivectorE3GANode \"%s\" failed to write \"bivector\".", GetName().c_str() );

	WriteNumber( L, context, "maxArea", maxArea );
	return true;
}

//=================================================================================
/*virtual*/ bool BivectorE3GANode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return false;

	const BivectorE3GANode* bivectorNode = ( const BivectorE3GANode* )node;
	*bivector = *bivectorNode->bivector;
	maxArea = bivectorNode->maxArea;
	return true;
}

//=================================================================================
/*virtual*/ std::string BivectorE3GANode::GetValueToDisplayString( void ) const
{
	char formattedString[ 256 ];
	sprintf_s( formattedString, sizeof( formattedString ), "< %1.2f, %1.2f, %1.2f > (maxArea: %1.2f)", bivector->get_e2_e3(), bivector->get_e3_e1(), bivector->get_e1_e2(), maxArea );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BivectorE3GANode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "yz" ] = bivector->get_e2_e3();
	varMap[ "zx" ] = bivector->get_e3_e1();
	varMap[ "xy" ] = bivector->get_e1_e2();
	varMap[ "maxArea" ] = maxArea;

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BivectorE3GANode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	// Get the new bivector.
	c3ga::bivectorE3GA trialBivector = *bivector;
	if( varMap.end() != varMap.find( "yz" ) )
		trialBivector.set_e2_e3( varMap[ "yz" ] );
	if( varMap.end() != varMap.find( "zx" ) )
		trialBivector.set_e3_e1( varMap[ "zx" ] );
	if( varMap.end() != varMap.find( "xy" ) )
		trialBivector.set_e1_e2( varMap[ "xy" ] );
	
	// Validate the new bivector.
	if( c3ga::norm( trialBivector ) > maxArea )
		return VSE_INVALID;

	// Get the new max-area.
	double trialMaxArea = maxArea;
	if( varMap.end() != varMap.find( "maxArea" ) )
		trialMaxArea = varMap[ "maxArea" ];

	// Validate the new max-area.
	if( trialMaxArea <= 0.0 )
		return VSE_INVALID;
	
	// Reject if no change occurred.
	if( c3ga::equals( trialBivector, *bivector, 0.0 ) && trialMaxArea == maxArea )
		return VSE_NO_CHANGE;

	// Accept the new values.
	*bivector = trialBivector;
	maxArea = trialMaxArea;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BivectorE3GANode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: yz, xz, xy, maxArea";
	return VSE_NONE;
}

//=================================================================================
c3ga::bivectorE3GA& BivectorE3GANode::GetBivector( void )
{
	return *bivector;
}

//=================================================================================
const c3ga::bivectorE3GA& BivectorE3GANode::GetBivector( void ) const
{
	return *bivector;
}

//=================================================================================
bool BivectorE3GANode::SetBivector( const c3ga::bivectorE3GA& bivector, bool rescaleIfNeeded /*= false*/ )
{
	double norm = c3ga::norm( bivector );
	if( norm <= maxArea )
		*this->bivector = bivector;
	else if( !rescaleIfNeeded )
		return false;
	else
		*this->bivector = c3ga::gp( bivector, 1.0 / norm );
	return true;
}

//=================================================================================
double BivectorE3GANode::GetMaxArea( void ) const
{
	return maxArea;
}

//=================================================================================
void BivectorE3GANode::SetMaxArea( double maxArea )
{
	this->maxArea = maxArea;
}

// BivectorE3GANode.cpp
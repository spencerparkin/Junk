// OrientationNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( OrientationNode, Node );

//=================================================================================
OrientationNode::OrientationNode( void )
{
	rotor = new c3ga::rotorE3GA();
	rotor->set( c3ga::rotorE3GA::coord_scalar_e1e2_e2e3_e3e1, 1.0, 0.0, 0.0, 0.0 );
}

//=================================================================================
/*virtual*/ OrientationNode::~OrientationNode( void )
{
	delete rotor;
}

//=================================================================================
/*virtual*/ bool OrientationNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !Node::ReadFromTable( L, context ) )
		return context.IssueError( "OrientationNode failed to read base-class portion." );

	if( !Node::ReadRotorE3GA( L, context, "rotor", *rotor ) )
		return context.IssueError( "OrientationNode \"%s\" failed to read \"rotor\".", GetName().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool OrientationNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "OrientationNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	if( !Node::WriteRotorE3GA( L, context, "rotor", *rotor ) )
		return context.IssueError( "OrientationNode \"%s\" failed to write \"rotor\".", GetName().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool OrientationNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "OrientationNode \"%s\" failed to copy base-class portion.", GetName().c_str() );

	const OrientationNode* orientationNode = ( const OrientationNode* )node;
	*rotor = *orientationNode->rotor;
	return true;
}

//=================================================================================
/*virtual*/ std::string OrientationNode::GetValueToDisplayString( void ) const
{
	char formattedString[ 64 ];
	sprintf_s( formattedString, sizeof( formattedString ), "< %1.2f, %1.2f, %1.2f, %1.2f >", rotor->get_scalar(), rotor->get_e2_e3(), rotor->get_e3_e1(), rotor->get_e1_e2() );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError OrientationNode::GetValueToString( std::string& valueString ) const
{
	return VSE_NOT_SUPPORTED;
}

//=================================================================================
/*virtual*/ Node::ValueStringError OrientationNode::SetValueFromString( const std::string& valueString )
{
	return VSE_NOT_SUPPORTED;
}

//=================================================================================
/*virtual*/ Node::ValueStringError OrientationNode::GetValueAsStringHelp( std::string& helpString ) const
{
	return VSE_NOT_SUPPORTED;
}

//=================================================================================
const c3ga::rotorE3GA& OrientationNode::GetRotor( void ) const
{
	return *rotor;
}

//=================================================================================
void OrientationNode::SetRotor( c3ga::rotorE3GA& rotor )
{
	*this->rotor = rotor;
}

// OrientationNode.cpp
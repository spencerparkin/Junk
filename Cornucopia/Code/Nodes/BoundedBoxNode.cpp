// BoundedBoxNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BoundedBoxNode, Node );

//=================================================================================
BoundedBoxNode::BoundedBoxNode( void )
{
	data.box.min.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	data.box.max.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	data.boundingBox.min.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	data.boundingBox.max.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
}

//=================================================================================
/*virtual*/ BoundedBoxNode::~BoundedBoxNode( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedBoxNode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "BoundedBoxNode failed to read base-class portion of node." );

		if( !ReadVectorE3GA( L, context, "boxMin", data.box.min ) )
			throw new Error( "BoundedBoxNode \"%s\" failed to read \"boxMin\".", GetName().c_str() );

		if( !ReadVectorE3GA( L, context, "boxMax", data.box.max, &data.box.min ) )
			throw new Error( "BoundedBoxNode \"%s\" failed to read \"boxMax\".", GetName().c_str() );

		if( !ReadVectorE3GA( L, context, "boundingBoxMin", data.boundingBox.min, &data.box.min ) )
			throw new Error( "BoundedBoxNode \"%s\" failed to read \"boundingBoxMin\".", GetName().c_str() );

		if( !ReadVectorE3GA( L, context, "boundingBoxMax", data.boundingBox.max, &data.box.max ) )
			throw new Error( "BoundedBoxNode \"%s\" failed to read \"boundingBoxMax\".", GetName().c_str() );

		//if( !data.IsValid() )
		//	throw new Error( "BoundedBoxNode \"%s\" failed to read its data, because it's data is invalid.", GetName().c_str() );

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
/*virtual*/ bool BoundedBoxNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BoundedBoxNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	if( !WriteVectorE3GA( L, context, "boxMin", data.box.min ) )
		return context.IssueError( "BoundedBoxNode \"%s\" failed to write \"boxMin\".", GetName().c_str() );

	if( !WriteVectorE3GA( L, context, "boxMax", data.box.max ) )
		return context.IssueError( "BoundedBoxNode \"%s\" failed to write \"boxMax\".", GetName().c_str() );

	if( !WriteVectorE3GA( L, context, "boundingBoxMin", data.boundingBox.min ) )
		return context.IssueError( "BoundedBoxNode \"%s\" failed to write \"boundingBoxMin\".", GetName().c_str() );

	if( !WriteVectorE3GA( L, context, "boundingBoxMax", data.boundingBox.max ) )
		return context.IssueError( "BoundedBoxNode \"%s\" failed to write \"boundingBoxMax\".", GetName().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedBoxNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return false;

	const BoundedBoxNode* boundedBoxNode = ( const BoundedBoxNode* )node;
	data = boundedBoxNode->data;
	return true;
}

//=================================================================================
bool BoundedBoxNode::Data::IsValid( void ) const
{
	if( box.IsValid() && boundingBox.IsValid() && boundingBox.Contains( box ) )
		return true;
	return false;
}

//=================================================================================
bool BoundedBoxNode::Data::operator==( const Data& data ) const
{
	if( box == data.box && boundingBox == data.boundingBox )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ std::string BoundedBoxNode::GetValueToDisplayString( void ) const
{
	char formattedString[ 128 ];
	sprintf_s( formattedString, sizeof( formattedString ), "< %1.2f, %1.2f, %1.2f >-< %1.2f, %1.2f, %1.2f >",
						data.box.min.get_e1(), data.box.min.get_e2(), data.box.min.get_e3(),
						data.box.max.get_e1(), data.box.max.get_e2(), data.box.max.get_e3() );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedBoxNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "xMin" ] = data.boundingBox.min.get_e1();
	varMap[ "xMax" ] = data.boundingBox.max.get_e1();
	varMap[ "yMin" ] = data.boundingBox.min.get_e2();
	varMap[ "yMax" ] = data.boundingBox.max.get_e2();
	varMap[ "zMin" ] = data.boundingBox.min.get_e3();
	varMap[ "zMax" ] = data.boundingBox.max.get_e3();

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedBoxNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	Data trialData = data;
	if( varMap.end() != varMap.find( "xMin" ) )
		trialData.boundingBox.min.set_e1( varMap[ "xMin" ] );
	if( varMap.end() != varMap.find( "xMax" ) )
		trialData.boundingBox.max.set_e1( varMap[ "xMax" ] );
	if( varMap.end() != varMap.find( "yMin" ) )
		trialData.boundingBox.min.set_e2( varMap[ "yMin" ] );
	if( varMap.end() != varMap.find( "yMax" ) )
		trialData.boundingBox.max.set_e2( varMap[ "yMax" ] );
	if( varMap.end() != varMap.find( "zMin" ) )
		trialData.boundingBox.min.set_e3( varMap[ "zMin" ] );
	if( varMap.end() != varMap.find( "zMax" ) )
		trialData.boundingBox.max.set_e3( varMap[ "zMax" ] );

	if( !trialData.boundingBox.Contains( trialData.box.min ) )
		trialData.box.min = trialData.boundingBox.min;
	if( !trialData.boundingBox.Contains( trialData.box.max ) )
		trialData.box.max = trialData.boundingBox.max;

	if( !trialData.IsValid() )
		return VSE_INVALID;

	if( trialData == data )
		return VSE_NO_CHANGE;

	data = trialData;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedBoxNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: xMin, xMax, yMin, yMax, zMin, zMax";
	return VSE_NONE;
}

//=================================================================================
bool BoundedBoxNode::Set( const Data& data )
{
	if( !data.IsValid() )
		return false;
	this->data = data;
	return true;
}

//=================================================================================
void BoundedBoxNode::Get( Data& data ) const
{
	data = this->data;
}

//=================================================================================
bool BoundedBoxNode::Box::IsValid( void ) const
{
	if( min.get_e1() > max.get_e1() )
		return false;
	if( min.get_e2() > max.get_e2() )
		return false;
	if( min.get_e3() > max.get_e3() )
		return false;
	return true;
}

//=================================================================================
// We assume here that this and the given box are valid.
bool BoundedBoxNode::Box::Contains( const Box& box ) const
{
	if( !Contains( box.min ) )
		return false;
	if( !Contains( box.max ) )
		return false;
	return true;
}

//=================================================================================
// It is assumed here that this box is valid.
bool BoundedBoxNode::Box::Contains( const c3ga::vectorE3GA& point ) const
{
	if( !( min.get_e1() <= point.get_e1() && point.get_e1() <= max.get_e1() ) )
		return false;
	if( !( min.get_e2() <= point.get_e2() && point.get_e2() <= max.get_e2() ) )
		return false;
	if( !( min.get_e3() <= point.get_e3() && point.get_e3() <= max.get_e3() ) )
		return false;
	return true;
}

//=================================================================================
bool BoundedBoxNode::Box::operator==( const Box& box ) const
{
	if( min.get_e1() != box.min.get_e1() )
		return false;
	if( min.get_e2() != box.min.get_e2() )
		return false;
	if( min.get_e3() != box.min.get_e3() )
		return false;

	if( max.get_e1() != box.max.get_e1() )
		return false;
	if( max.get_e2() != box.max.get_e2() )
		return false;
	if( max.get_e3() != box.max.get_e3() )
		return false;

	return true;
}

// BoundedBoxNode.cpp
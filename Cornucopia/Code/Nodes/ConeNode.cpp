// ConeNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( ConeNode, VectorE3GANode );

//=================================================================================
ConeNode::ConeNode( void )
{
	coneAngle = 0.0;
}

//=================================================================================
/*virtual*/ ConeNode::~ConeNode( void )
{
}

//=================================================================================
/*virtual*/ bool ConeNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !VectorE3GANode::ReadFromTable( L, context ) )
		return false;

	if( !ReadNumber( L, context, "coneAngle", coneAngle, 0.0 ) )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ bool ConeNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !VectorE3GANode::WriteToTable( L, context ) )
		return false;

	WriteNumber( L, context, "coneAngle", coneAngle );
	return true;
}

//=================================================================================
/*virtual*/ bool ConeNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !VectorE3GANode::Copy( node, context, copyParameters ) )
		return false;

	const ConeNode* coneNode = ( const ConeNode* )node;
	coneAngle = coneNode->coneAngle;
	return true;
}

//=================================================================================
/*virtual*/ std::string ConeNode::GetValueToDisplayString( void ) const
{
	std::string displayString = VectorE3GANode::GetValueToDisplayString();
	
	char formattedString[ 256 ];
	sprintf_s( formattedString, sizeof( formattedString ), "%s, coneAngle: %1.2f", displayString.c_str(), coneAngle );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError ConeNode::GetValueToString( std::string& valueString ) const
{
	std::string vectorValueString;
	ValueStringError vse = VectorE3GANode::GetValueToString( vectorValueString );
	if( vse != VSE_NONE )
		return vse;

	VarMap varMap;
	varMap[ "coneAngle" ] = coneAngle;
	ConvertValueStringFromVarMap( valueString, varMap );

	valueString = vectorValueString + ", " + valueString;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError ConeNode::SetValueFromString( const std::string& valueString )
{
	ValueStringError vse = VectorE3GANode::SetValueFromString( valueString );
	if( !( vse == VSE_NONE || vse == VSE_NO_CHANGE ) )
		return vse;

	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	double trialConeAngle = coneAngle;
	if( varMap.end() != varMap.find( "coneAngle" ) )
		trialConeAngle = varMap[ "coneAngle" ];

	if( trialConeAngle < 0.0 || trialConeAngle > M_PI )
		return VSE_INVALID;

	if( trialConeAngle == coneAngle && vse == VSE_NO_CHANGE )
		return VSE_NO_CHANGE;

	coneAngle = trialConeAngle;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError ConeNode::GetValueAsStringHelp( std::string& helpString ) const
{
	VectorE3GANode::GetValueAsStringHelp( helpString );
	helpString += ", coneAngle";
	return VSE_NONE;
}

//=================================================================================
bool ConeNode::SetConeAngle( double coneAngle )
{
	if( coneAngle < 0.0 || coneAngle > M_PI )
		return false;

	this->coneAngle = coneAngle;
	return true;
}

//=================================================================================
double ConeNode::GetConeAngle( void ) const
{
	return coneAngle;
}

// ConeNode.cpp
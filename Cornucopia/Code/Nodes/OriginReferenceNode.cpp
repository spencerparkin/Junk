// OriginReferenceNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( OriginReferenceNode, FileReferenceNode );

//=================================================================================
OriginReferenceNode::OriginReferenceNode( void )
{
	autoUpgrade = false;
	version = 0.0;
}

//=================================================================================
/*virtual*/ OriginReferenceNode::~OriginReferenceNode( void )
{
}

//=================================================================================
/*virtual*/ bool OriginReferenceNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !FileReferenceNode::ReadFromTable( L, context ) )
		return false;

	if( !ReadBoolean( L, context, "autoUpgrade", autoUpgrade, false ) )
		return false;

	if( !ReadNumber( L, context, "version", version, 0.0 ) )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ bool OriginReferenceNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !FileReferenceNode::WriteToTable( L, context ) )
		return false;

	if( !WriteBoolean( L, context, "autoUpgrade", autoUpgrade ) )
		return false;

	if( !WriteNumber( L, context, "version", version ) )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ bool OriginReferenceNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !FileReferenceNode::Copy( node, context, copyParameters ) )
		return false;

	const OriginReferenceNode* originRefNode = ( const OriginReferenceNode* )node;
	autoUpgrade = originRefNode->autoUpgrade;
	version = originRefNode->version;
	return true;
}

//=================================================================================
/*virtual*/ std::string OriginReferenceNode::GetValueToDisplayString( void ) const
{
	std::string displayString = FileReferenceNode::GetValueToDisplayString();

	char formattedString[ 256 ];
	if( displayString.empty() )
		displayString = "File not set";
	sprintf_s( formattedString, sizeof( formattedString ), "%s, v%1.1f, autoUpgrade: %s", displayString.c_str(), version, ( autoUpgrade ? "Yes" : "No" ) );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError OriginReferenceNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;
	varMap[ "autoUpgrade" ] = autoUpgrade;
	varMap[ "version" ] = version;
	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError OriginReferenceNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	bool trialAutoUpgrade = autoUpgrade;
	double trialVersion = version;
	if( varMap.end() != varMap.find( "autoUpgrade" ) )
		trialAutoUpgrade = varMap[ "autoUpgrade" ] == 0.0 ? false : true;
	if( varMap.end() != varMap.find( "version" ) )
		trialVersion = varMap[ "version" ];

	if( trialAutoUpgrade == autoUpgrade && trialVersion == version )
		return VSE_NO_CHANGE;

	autoUpgrade = trialAutoUpgrade;
	version = trialVersion;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError OriginReferenceNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: autoUpgrade, version";
	return VSE_NONE;
}

//=================================================================================
void OriginReferenceNode::SetAutoUpgrade( bool autoUpgrade )
{
	this->autoUpgrade = autoUpgrade;
}

//=================================================================================
bool OriginReferenceNode::GetAutoUpgrade( void ) const
{
	return autoUpgrade;
}

//=================================================================================
void OriginReferenceNode::SetVersion( double version )
{
	this->version = version;
}

//=================================================================================
double OriginReferenceNode::GetVersion( void ) const
{
	return version;
}

// OriginReferenceNode.cpp
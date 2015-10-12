// ColorRGBANode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( ColorRGBANode, Node );

//=================================================================================
ColorRGBANode::ColorRGBANode( void )
{
}

//=================================================================================
/*virtual*/ ColorRGBANode::~ColorRGBANode( void )
{
}

//=================================================================================
/*virtual*/ bool ColorRGBANode::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( !Node::ReadFromTable( L, context ) )
			throw new Error( "ColorRGBANode failed to read base-class portion." );

		if( !ReadNumber( L, context, "red", color.r, 1.0 ) )
			throw new Error( "ColorRGBANode \"%s\" failed to read \"red\".", GetName().c_str() );

		if( !ReadNumber( L, context, "green", color.g, 1.0 ) )
			throw new Error( "ColorRGBANode \"%s\" failed to read \"green\".", GetName().c_str() );

		if( !ReadNumber( L, context, "blue", color.b, 1.0 ) )
			throw new Error( "ColorRGBANode \"%s\" failed to read \"blue\".", GetName().c_str() );

		if( !ReadNumber( L, context, "alpha", color.a, 1.0 ) )
			throw new Error( "ColorRGBANode \"%s\" failed to read \"alpha\".", GetName().c_str() );

		if( !color.IsValid() )
			throw new Error( "ColorRGBANode \"%s\" failed to read its color, because the color is invalid.  (%f,%f,%f,%f)", GetName().c_str(), color.r, color.g, color.b, color.a );

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
/*virtual*/ bool ColorRGBANode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "ColorRGBANode \"%s\" failed to write base-class portion.", GetName().c_str() );

	WriteNumber( L, context, "red", color.r );
	WriteNumber( L, context, "green", color.g );
	WriteNumber( L, context, "blue", color.b );
	WriteNumber( L, context, "alpha", color.a );
	return true;
}

//=================================================================================
/*virtual*/ bool ColorRGBANode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "ColorRGBANode \"%s\" failed to copy base-class portion.", GetName().c_str() );

	const ColorRGBANode* colorRGBANode = ( const ColorRGBANode* )node;
	color = colorRGBANode->color;
	return true;
}

//=================================================================================
/*virtual*/ std::string ColorRGBANode::GetValueToDisplayString( void ) const
{
	char formattedString[ 128 ];
	sprintf_s( formattedString, sizeof( formattedString ), "< %1.2f, %1.2f, %1.2f, %1.2f >", color.r, color.g, color.b, color.a );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError ColorRGBANode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "r" ] = color.r;
	varMap[ "g" ] = color.g;
	varMap[ "b" ] = color.b;
	varMap[ "a" ] = color.a;

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError ColorRGBANode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	Color trialColor = color;
	if( varMap.end() != varMap.find( "r" ) )
		trialColor.r = varMap[ "r" ];
	if( varMap.end() != varMap.find( "g" ) )
		trialColor.g = varMap[ "g" ];
	if( varMap.end() != varMap.find( "b" ) )
		trialColor.b = varMap[ "b" ];
	if( varMap.end() != varMap.find( "a" ) )
		trialColor.a = varMap[ "a" ];

	if( !trialColor.IsValid() )
		return VSE_INVALID;

	if( trialColor == color )
		return VSE_NO_CHANGE;

	color = trialColor;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError ColorRGBANode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: r, g, b, a";
	return VSE_NONE;
}

//=================================================================================
bool ColorRGBANode::SetColor( const Color& color )
{
	if( !color.IsValid() )
		return false;

	this->color = color;
	return true;
}

//=================================================================================
bool ColorRGBANode::GetColor( Color& color ) const
{
	color = this->color;
	return true;
}

//=================================================================================
ColorRGBANode::Color::Color( void )
{
	r = 1.0;
	g = 1.0;
	b = 1.0;
	a = 1.0;
}

//=================================================================================
ColorRGBANode::Color::Color( const Color& color )
{
	this->operator=( color );
}

//=================================================================================
ColorRGBANode::Color& ColorRGBANode::Color::operator=( const Color& color )
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;

	return *this;
}

//=================================================================================
bool ColorRGBANode::Color::operator==( const Color& color ) const
{
	if( r != color.r )
		return false;
	if( g != color.g )
		return false;
	if( b != color.b )
		return false;
	if( a != color.a )
		return false;

	return true;
}

//=================================================================================
double ColorRGBANode::Color::Brightness( void ) const
{
	return sqrt( r*r + g*g + b*b );
}

//=================================================================================
bool ColorRGBANode::Color::IsValid( void ) const
{
	if( r < 0.0 || r > 1.0 )
		return false;
	if( g < 0.0 || g > 1.0 )
		return false;
	if( b < 0.0 || b > 1.0 )
		return false;
	if( a < 0.0 || a > 1.0 )
		return false;

	return true;
}

// ColorRGBANode.cpp
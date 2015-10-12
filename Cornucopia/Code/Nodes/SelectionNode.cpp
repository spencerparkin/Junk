// SelectionNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( SelectionNode, Node );

//=================================================================================
SelectionNode::SelectionNode( void )
{
	mode = MULTI_SELECT;
	selectionMap = new SelectionMap();
}

//=================================================================================
/*virtual*/ SelectionNode::~SelectionNode( void )
{
	delete selectionMap;
}

//=================================================================================
/*virtual*/ bool SelectionNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !Node::ReadFromTable( L, context ) )
		return context.IssueError( "SelectionNode failed to read base-class portion." );

	int count;
	if( !Node::ReadInteger( L, context, "count", count, 0 ) )
		return context.IssueError( "SelectionNode \"%s\" failed to read \"count\".", GetName().c_str() );

	selectionMap->clear();
	for( int index = 0; index < count; index++ )
	{
		char formattedKey[ 32 ];
		sprintf_s( formattedKey, sizeof( formattedKey ), "mapEntry%d", index );

		lua_getfield( L, -1, formattedKey );
		if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
			return context.IssueError( "SelectionNode \"%s\" failed to read sub-table \"%s\".", GetName().c_str(), formattedKey );

		std::string selectionName;
		if( !Node::ReadString( L, context, "name", selectionName, "" ) )
			return context.IssueError( "SelectionNode \"%s\" failed to read \"name\" from sub-table \"%s\".", GetName().c_str(), formattedKey );

		bool selected;
		if( !Node::ReadBoolean( L, context, "selected", selected, false ) )
			return context.IssueError( "SelectionNode \"%s\" failed to read \"selected\" from sub-table \"%s\".", GetName().c_str(), formattedKey );

		( *selectionMap )[ selectionName ] = selected;
		lua_pop( L, 1 );
	}

	int intMode;
	if( !Node::ReadInteger( L, context, "mode", intMode, MULTI_SELECT ) )
		return context.IssueError( "SelectionNode \"%s\" failed to read \"mode\".", GetName().c_str() );

	mode = ( Mode )intMode;
	if( !SelectionStateValid() )
		return context.IssueError( "SelectionNode \"%s\" fails to read, because the data read is invalid.", GetName().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool SelectionNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "SelectionNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	int count = SelectableCount();
	if( !Node::WriteInteger( L, context, "count", count ) )
		return context.IssueError( "SelectionNode \"%s\" failed to write \"count\".", GetName().c_str() );

	int index = 0;
	for( SelectionMap::iterator iter = selectionMap->begin(); iter != selectionMap->end(); iter++ )
	{
		char formattedKey[ 32 ];
		sprintf_s( formattedKey, sizeof( formattedKey ), "mapEntry%d", index++ );

		lua_newtable( L );

		if( !Node::WriteString( L, context, "name", iter->first ) )
			return context.IssueError( "SelectionNode \"%s\" failed to write \"name\" to sub-table \"%s\".", GetName().c_str(), formattedKey );

		if( !Node::WriteBoolean( L, context, "selected", iter->second ) )
			return context.IssueError( "SelectionNode \"%s\" failed to write \"selected\" to sub-table \"%s\".", GetName().c_str(), formattedKey );
		
		lua_setfield( L, -2, formattedKey );
	}

	if( !Node::WriteInteger( L, context, "mode", mode ) )
		return context.IssueError( "SelectionNode \"%s\" failed to write \"mode\".", GetName().c_str() );

	return true;
}

//=================================================================================
/*virtual*/ bool SelectionNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "SelectionNode \"%s\" failed to copy base-class portion.", GetName().c_str() );

	const SelectionNode* selectionNode = ( const SelectionNode* )node;
	*selectionMap = *selectionNode->selectionMap;
	mode = selectionNode->mode;
	return true;
}

//=================================================================================
/*virtual*/ std::string SelectionNode::GetValueToDisplayString( void ) const
{
	char formattedString[ 128 ];
	sprintf_s( formattedString, sizeof( formattedString ), "%d/%d (%s)", SelectionCount(), SelectableCount(), ( mode == SINGLE_SELECT ? "single" : "multi" ) );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError SelectionNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	for( SelectionMap::const_iterator iter = selectionMap->begin(); iter != selectionMap->end(); iter++ )
		varMap[ iter->first ] = iter->second ? 1.0 : 0.0;
	
	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError SelectionNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	SelectionMap trialSelectionMap;
	for( VarMap::iterator iter = varMap.begin(); iter != varMap.end(); iter++ )
		trialSelectionMap[ iter->first ] = iter->second != 0.0 ? true : false;

	if( trialSelectionMap == *selectionMap )
		return VSE_NO_CHANGE;

	SelectionMap* restoreSelectionMap = selectionMap;
	selectionMap = &trialSelectionMap;
	if( !SelectionStateValid() )
	{
		selectionMap = restoreSelectionMap;
		return VSE_INVALID;
	}

	selectionMap = restoreSelectionMap;
	*selectionMap = trialSelectionMap;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError SelectionNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Unknown items are added; deleted items are removed.";
	return VSE_NONE;
}

//=================================================================================
SelectionNode::Mode SelectionNode::GetMode( void ) const
{
	return mode;
}

//=================================================================================
bool SelectionNode::SetMode( Mode mode )
{
	Mode restoreMode = this->mode;
	this->mode = mode;
	
	if( !SelectionStateValid() )
	{
		this->mode = restoreMode;
		return false;
	}
	
	return true;
}

//=================================================================================
bool SelectionNode::Select( const std::string& selectionName, bool selected, bool addIfNotFound /*= false*/ )
{
	SelectionMap::iterator selectionIter = selectionMap->find( selectionName );
	if( selectionIter == selectionMap->end() )
	{
		if( !addIfNotFound )
			return false;

		( *selectionMap )[ selectionName ] = false;
		selectionIter = selectionMap->find( selectionName );
	}

	if( mode == SINGLE_SELECT )
	{
		// You cannot unselect anything in single select mode.  You can only select stuff.
		// Selecting something selects it and then unselects everything else.
		if( !selected )
			return false;

		for( SelectionMap::iterator iter = selectionMap->begin(); iter != selectionMap->end(); iter++ )
		{
			if( iter->first == selectionIter->first )
				iter->second = true;
			else
				iter->second = false;
		}
	}
	else if( mode == MULTI_SELECT )
	{
		selectionIter->second = selected;
		return true;
	}

	return false;
}

//=================================================================================
bool SelectionNode::IsSelected( const std::string& selectionName ) const
{
	SelectionMap::const_iterator iter = selectionMap->find( selectionName );
	if( iter == selectionMap->end() )
		return false;
	return iter->second;
}

//=================================================================================
bool SelectionNode::SelectionStateValid( void ) const
{
	if( mode == MULTI_SELECT )
		return true;
	else if( mode == SINGLE_SELECT )
	{
		int selectionCount = SelectionCount();
		if( selectionCount != 1 )
			return false;
		return true;
	}
	return true;
}

//=================================================================================
int SelectionNode::SelectionCount( void ) const
{
	int selectionCount = 0;
	for( SelectionMap::const_iterator iter = selectionMap->begin(); iter != selectionMap->end(); iter++ )
		if( iter->second )
			selectionCount++;
	return selectionCount;
}

//=================================================================================
int SelectionNode::SelectableCount( void ) const
{
	return selectionMap->size();
}

//=================================================================================
const SelectionNode::SelectionMap& SelectionNode::GetSelectionMap( void ) const
{
	return *selectionMap;
}

// SelectionNode.cpp
// Writer.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

//=================================================================================
Writer::Writer( const std::string& file, std::string* buffer /*= 0*/ )
{
	this->file = file;
	this->buffer = buffer;
	fileHandle = INVALID_HANDLE_VALUE;
	indentLevel = 0;
}

//=================================================================================
/*virtual*/ Writer::~Writer( void )
{
	if( fileHandle != INVALID_HANDLE_VALUE )
		CloseHandle( fileHandle );
}

//=================================================================================
bool Writer::Write( lua_State* L, Context& context )
{
	if( !file.empty() )
	{
		const char* fileName = file.c_str();
		fileHandle = CreateFileA( fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( fileHandle == INVALID_HANDLE_VALUE )
			return context.IssueError( "Failed to open file \"%s\" for writing.", file.c_str() );
	}

	indentLevel = 0;
	if( !WriteLine( "return", context ) )
		return context.IssueError( "Failed to write return key-word in file \"%s\".", file.c_str() );

	if( !WriteTable( L, false, context ) )
		return context.IssueError( "Failed to write root-level node table in file \"%s\".", file.c_str() );

	return true;
}

//=================================================================================
bool Writer::WriteTable( lua_State* L, bool comma, Context& context )
{
	int top = lua_gettop( L );

	if( !WriteLine( "{", context ) )
		return false;

	indentLevel++;

	try
	{
		// Iterate over the given Lua table, populating
		// the key list as we go.
		std::string key;
		std::list< std::string > keyList;
		lua_pushnil( L );
		while( 0 != lua_next( L, -2 ) )
		{
			// Formulate the key.
			switch( lua_type( L, -2 ) )
			{
				case LUA_TNUMBER:
				{
					int index = ( int )lua_tonumber( L, -2 );
					std::ostringstream stream;
					stream << index;
					key = stream.str();
					break;
				}
				case LUA_TSTRING:
				{
					key = lua_tostring( L, -2 );
					break;
				}
				default:
				{
					throw new Error( "The Lua table writer encountered a yet-to-be handle Lua key type." );
				}
			}

			// Add the key to a list for later processing.
			keyList.push_back( key );

			// Remove the value, but keep the key.
			lua_pop( L, 1 );
		}

		// Before processing the list of keys, sort the list.
		// By writing out the keys of the table in a consistent order,
		// our output here plays nice with version control systems.
		keyList.sort();

		// Now go process the list of keys, writing out the table as we go.
		for( std::list< std::string >::iterator iter = keyList.begin(); iter != keyList.end(); iter++ )
		{
			// Grab the next key and its corresponding value.
			key = *iter;
			if( !isdigit( key[0] ) )
				lua_getfield( L, -1, key.c_str() );
			else
			{
				std::istringstream stream( key );
				int index;
				stream >> index;
				lua_pushinteger( L, index );
				lua_gettable( L, -2 );
			}

			// Now that we've used the key to index the table, change it to be Lua code.
			if( isdigit( key[0] ) )
				key = "[" + key + "]";
			else
				key = "[\"" + key + "\"]";

			// What type of value do we have?
			std::string value = "";
			switch( lua_type( L, -1 ) )
			{
				case LUA_TNUMBER:
				{
					double number = lua_tonumber( L, -1 );
					std::ostringstream stream;
					stream << number;
					value = stream.str();
					break;
				}
				case LUA_TSTRING:
				{
					value = lua_tostring( L, -1 );
					value = "\"" + value + "\"";
					break;
				}
				case LUA_TBOOLEAN:
				{
					int boolean = lua_toboolean( L, -1 );
					if( boolean )
						value = "true";
					else
						value = "false";
					break;
				}
				case LUA_TTABLE:
				{
					if( !WriteLine( key + " = ", context ) )
						throw new Error( "Write line failure." );
					if( !WriteTable( L, true, context ) )
						throw new Error( "Failed to write sub-table." );
					break;
				}
				case LUA_TNIL:
				{
					value = "nil";
					break;
				}
				default:
				{
					throw new Error( "The Lua table writer encountered a yet-to-be handle Lua value type." );
				}
			}

			// Finally, write out the key-value pair, if we have a simple case.
			if( !value.empty() )
				if( !WriteLine( key + " = " + value + ",", context ) )
					throw new Error( "Write line failure." );

			// Remove value.
			lua_pop( L, 1 );
		}
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	indentLevel--;

	if( !WriteLine( comma ? "}," : "}", context ) )
		return false;

	return true;
}

//=================================================================================
bool Writer::WriteLine( const std::string& line, Context& context )
{
	// Formulate the line we'll write out.
	std::string lineToWrite = line + "\n";
	for( int indent = 0; indent < indentLevel; indent++ )
		lineToWrite = "\t" + lineToWrite;

	// Try to go write it out.
	if( fileHandle == INVALID_HANDLE_VALUE )
		*buffer += lineToWrite;
	else
	{
		DWORD bytesToWrite = lineToWrite.size();
		DWORD bytesWritten = 0;
		if( !WriteFile( fileHandle, lineToWrite.c_str(), bytesToWrite, &bytesWritten, NULL ) )
			return context.IssueError( "Failed to write line \"%s\".", line.c_str() );
		if( bytesWritten != bytesToWrite )
			return context.IssueError( "Wrote only %d of %d bytes of line \"%s\".", bytesWritten, bytesToWrite, line.c_str() );
	}

	// Hurray!
	return true;
}

// Writer.cpp
// LuaInterpreter.cpp

#include "LuaInterpreter.h"

int main( int argc, char** argv )
{
	lua_State* L = luaL_newstate();
	if( !L )
		return -1;

	luaL_openlibs( L );

	if( argc <= 1 )
	{
		for(;;)
		{
			fprintf( stdout, "Lua: " );
			fflush( stdout );

			char buf[ 5*1024 ];
			if( !fgets( buf, sizeof( buf ), stdin ) )
				break;

			l_execute( L, buf );
		}
	}
	else
	{
		FILE* fp = nullptr;
		char* buf = nullptr;
		const char* file = argv[1];

		do
		{
			fopen_s( &fp, file, "r" );
			if( !fp )
				break;
		
			fseek( fp, 0, SEEK_END );
			long size = ftell( fp );
			if( size <= 0 )
				break;
			
			buf = new char[ size + 1 ];
			if( !buf )
				break;

			fseek( fp, 0, SEEK_SET );
			long bytesRead = fread( buf, 1, size, fp );
			buf[ bytesRead ] = '\0';
				
			l_execute( L, buf );
		}
		while( false );

		if( fp )
			fclose( fp );

		delete[] buf;
	}

	lua_close( L );
	return 0;
}

void l_execute( lua_State* L, const char* buf )
{
	int result = luaL_loadbuffer( L, buf, strlen( buf ), "LuaInterpreter" );
	if( result != 0 )
	{
		fprintf( stderr, "Chunk load failure: %s", lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
	}

	result = lua_pcall( L, 0, 0, 0 );
	if( result != 0 )
	{
		fprintf( stderr, "Chunk call failure: %s", lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
	}
}

// LuaInterpreter.cpp
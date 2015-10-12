// Writer.h

namespace Cornucopia
{
	//=================================================================================
	class Writer
	{
	public:

		// Pass in an empty string if all you want is the buffer.
		Writer( const std::string& file, std::string* buffer = 0 );
		virtual ~Writer( void );

		// Here we assume that we've been given a table at the top
		// of the stack, and we go write it out to file with a return
		// statement that returns the table.
		bool Write( lua_State* L, Context& context );

	private:

		bool WriteLine( const std::string& line, Context& context );
		bool WriteTable( lua_State* L, bool comma, Context& context );

		HANDLE fileHandle;
		std::string file;
		int indentLevel;
		std::string* buffer;
	};
}

// Writer.h
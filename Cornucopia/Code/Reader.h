// Reader.h

namespace Cornucopia
{
	//=================================================================================
	class Reader
	{
	public:

		// Pass in an empty string for the file name if the buffer is to be used.
		Reader( const std::string& file, std::string* buffer = 0 );
		~Reader( void );

		static const char* Read( lua_State* L, void* data, size_t* size );

		DWORD GetErrorCode( void );

	private:

		const char* ReadChunk( lua_State* L, size_t* size );

		// Store last chunk read here so that it remains
		// in scope while Lua has a chance to process it.
		char* chunk;

		DWORD errorCode;
		HANDLE fileHandle;
		std::string file;
		std::string* buffer;
	};
}

// Reader.h
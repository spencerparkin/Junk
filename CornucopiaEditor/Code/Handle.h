// Handle.h

namespace CornucopiaEditor
{
	template< typename Type >
	class HandleSystem
	{
	public:

		enum
		{
			INVALID_HANDLE = -1,
		};

		HandleSystem( void );
		~HandleSystem( void );

		int CreateHandleFor( Type* type );
		void InvalidateHandle( int handle );
		Type* DereferenceHandle( int handle );

	private:

		// A handle is valid if and only if it appears in the map.
		typedef std::map< int, Type* > TypeMap;
		TypeMap typeMap;

		int nextHandle;
	};
}

#include "Handle.hpp"

// Handle.h
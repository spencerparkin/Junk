// Handle.hpp

//=================================================================================
template< typename Type >
CornucopiaEditor::HandleSystem< Type >::HandleSystem( void )
{
	nextHandle = 0;
}

//=================================================================================
template< typename Type >
inline CornucopiaEditor::HandleSystem< Type >::~HandleSystem( void )
{
}

//=================================================================================
template< typename Type >
inline int CornucopiaEditor::HandleSystem< Type >::CreateHandleFor( Type* type )
{
	typeMap[ nextHandle ] = type;
	int newHandle = nextHandle++;
	if( nextHandle == INVALID_HANDLE )
		nextHandle++;
	return newHandle;
}

//=================================================================================
template< typename Type >
inline void CornucopiaEditor::HandleSystem< Type >::InvalidateHandle( int handle )
{
	typeMap.erase( handle );
}

//=================================================================================
template< typename Type >
inline Type* CornucopiaEditor::HandleSystem< Type >::DereferenceHandle( int handle )
{
	TypeMap::iterator iter = typeMap.find( handle );
	if( iter == typeMap.end() )
		return 0;
	Type* type = iter->second;
	return type;
}

// Handle.hpp
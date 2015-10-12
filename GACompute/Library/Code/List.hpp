// List.hpp

template< class Data >
inline /*static*/ void BlankDataManager< Data >::Create( Data& data )
{
}

template< class Data >
inline /*static*/ void BlankDataManager< Data >::Destroy( Data& data )
{
}

template< class Data >
inline /*static*/ void BlankDataManager< Data >::Copy( Data& dataA, const Data& dataB )
{
	dataA = dataB;
}

template< class Data >
inline /*static*/ int BlankDataManager< Data >::SortCompare( const Data& dataA, const Data& dataB )
{
	return 0;
}

template< class Data >
inline /*static*/ void DefaultDataManager< Data >::Create( Data& data )
{
	data = 0;
}

template< class Data >
inline /*static*/ void DefaultDataManager< Data >::Destroy( Data& data )
{
	delete data;
	data = 0;
};

template< class Data >
inline /*static*/ void DefaultDataManager< Data >::Copy( Data& dataA, const Data& dataB )
{
	dataA = ( Data )dataB->Clone();
}

template< class Data >
inline /*static*/ int DefaultDataManager< Data >::SortCompare( const Data& dataA, const Data& dataB )
{
	//return dataA->SortCompareWith( dataB );
	return 0;
}

template< class Data, class DataManager >
inline List< Data, DataManager >::List( void )
{
	count = 0;

	head = 0;
	tail = 0;
}

template< class Data, class DataManager >
inline List< Data, DataManager >::~List( void )
{
	RemoveAll();
}

template< class Data, class DataManager >
inline List< Data, DataManager >::Node::Node( void )
{
	next = 0;
	prev = 0;

	DataManager::Create( data );
}

template< class Data, class DataManager >
inline List< Data, DataManager >::Node::~Node( void )
{
	DataManager::Destroy( data );
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::Node::Couple( Node* before, Node* after )
{
	if( before )
		before->next = this;

	if( after )
		after->prev = this;

	next = after;
	prev = before;
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::Node::Decouple( void )
{
	if( next )
		next->prev = prev;
	if( prev )
		prev->next = next;

	next = 0;
	prev = 0;
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::Node::Next( void )
{
	return next;
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::Node::Prev( void )
{
	return prev;
}

template< class Data, class DataManager >
inline typename const List< Data, DataManager >::Node* List< Data, DataManager >::Node::Next( void ) const
{
	return next;
}

template< class Data, class DataManager >
inline typename const List< Data, DataManager >::Node* List< Data, DataManager >::Node::Prev( void ) const
{
	return prev;
}

template< class Data, class DataManager >
inline int List< Data, DataManager >::Count( void ) const
{
	return count;
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::Head( void )
{
	return head;
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::Tail( void )
{
	return tail;
}

template< class Data, class DataManager >
inline typename const List< Data, DataManager >::Node* List< Data, DataManager >::Head( void ) const
{
	return head;
}

template< class Data, class DataManager >
inline typename const List< Data, DataManager >::Node* List< Data, DataManager >::Tail( void ) const
{
	return tail;
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::InsertBefore( Node* node /*= 0*/, Node* insertedNode /*= 0*/ )
{
	if( !insertedNode )
		insertedNode = new Node();

	if( count == 0 )
	{
		head = insertedNode;
		tail = insertedNode;
	}
	else
	{
		if( !node )
			node = head;

		insertedNode->Couple( node->prev, node );

		if( !insertedNode->prev )
			head = insertedNode;
	}

	count++;

	return insertedNode;
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::InsertAfter( Node* node /*= 0*/, Node* insertedNode /*= 0*/ )
{
	if( !insertedNode )
		insertedNode = new Node();

	if( count == 0 )
	{
		head = insertedNode;
		tail = insertedNode;
	}
	else
	{
		if( !node )
			node = tail;

		insertedNode->Couple( node, node->next );

		if( !insertedNode->next )
			tail = insertedNode;
	}

	count++;

	return insertedNode;
}

template< class Data, class DataManager >
inline bool List< Data, DataManager >::Remove( Node* node, bool deleteNode /*= true*/ )
{
	if( node == head )
		head = head->next;
	if( node == tail )
		tail = tail->prev;

	node->Decouple();

	if( deleteNode )
		delete node;

	count--;

	return true;
}

template< class Data, class DataManager >
inline bool List< Data, DataManager >::RemoveAll( void )
{
	while( count > 0 )
		if( !Remove( head, true ) )
			return false;

	return true;
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::Absorb( List* list )
{
	if( count == 0 )
	{
		head = list->head;
		tail = list->tail;
	}
	else
	{
		tail->next = list->head;
		tail->next->prev = tail;
		tail = list->tail;
	}

	count = list->count;

	list->head = 0;
	list->tail = 0;
	list->count = 0;
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::Concatinate( const List& list )
{
	const Node* node = list.head;
	while( node )
	{
		Node* insertedNode = InsertAfter( tail );
		DataManager::Copy( insertedNode->data, node->data );
		node = node->next;
	}
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::Copy( const List& list )
{
	if( this != &list )
	{
		RemoveAll();
		Concatinate( list );
	}
}

template< class Data, class DataManager >
inline int List< Data, DataManager >::Sort( SortOrder sortOrder, SortCompareFunc sortCompareFunc /*= 0*/, void* data /*= 0*/ )
{
	int adjacentSwapCount = 0;

	bool keepGoing = true;
	while( keepGoing )
	{
		keepGoing = false;

		Node* node = head;
		while( node && node->next )
		{
			Node* next = node->next;

			int result = 0;
			if( sortCompareFunc )
				result = sortCompareFunc( node->data, next->data, data );
			else
				result = DataManager::SortCompare( node->data, next->data );

			if( ( result > 0 && sortOrder == SORT_ASCENDING ) || ( result < 0 && sortOrder == SORT_DESCENDING ) )
			{
				Remove( node, false );
				InsertAfter( next, node );

				keepGoing = true;
				adjacentSwapCount++;
			}
			else
			{
				node = next;
			}
		}
	}

	return adjacentSwapCount;
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::Reverse( void )
{
	if( count == 1 )
		return;

	Node* node = head;
	Remove( head, false );
	Reverse();
	InsertAfter( tail, node );
}

template< class Data, class DataManager >
inline typename List< Data, DataManager >::Node* List< Data, DataManager >::Find( Data data )
{
	for( Node* node = head; node; node = node->next )
		if( node->data == data )
			return node;
	return 0;
}

template< class Data, class DataManager >
inline typename const List< Data, DataManager >::Node* List< Data, DataManager >::Find( const Data data ) const
{
	return const_cast< List< Data, DataManager >* >( this )->Find( data );
}

template< class Data, class DataManager >
inline void List< Data, DataManager >::NullifyData( void )
{
	for( Node* node = head; node; node = node->Next() )
		node->data = 0;
}

// List.hpp
// List.hpp

template< typename Data >
inline /*static*/ void GAExpressionLib::DumbDataManager< Data >::Initialize( Data& data )
{
}

template< typename Data >
inline /*static*/ void GAExpressionLib::DumbDataManager< Data >::Finalize( Data& data )
{
}

template< typename Data >
inline /*static*/ void GAExpressionLib::DumbDataManager< Data >::Copy( Data& dataA, const Data& dataB )
{
	dataA = dataB;
}

template< typename Data >
inline /*static*/ int GAExpressionLib::DumbDataManager< Data >::Compare( const Data& dataA, const Data& dataB )
{
	return 0;
}

template< typename Data >
inline /*static*/ void GAExpressionLib::PointerDataManager< Data >::Initialize( Data& data )
{
	data = nullptr;
}

template< typename Data >
inline /*static*/ void GAExpressionLib::PointerDataManager< Data >::Finalize( Data& data )
{
	delete data;
}

template< typename Data >
inline /*static*/ void GAExpressionLib::PointerDataManager< Data >::Copy( Data& dataA, const Data& dataB )
{
	dataA = dataB->Clone();
}

template< typename Data >
inline /*static*/ int GAExpressionLib::PointerDataManager< Data >::Compare( const Data& dataA, const Data& dataB )
{
	return 0;
}

template< typename Data, class DataManager >
inline GAExpressionLib::List< Data, DataManager >::List( void )
{
	head = nullptr;
	tail = nullptr;

	count = 0;
}

template< typename Data, class DataManager >
inline GAExpressionLib::List< Data, DataManager >::~List( void )
{
	RemoveAll();
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::Insert( Node* newNode, Node* prev, Node* next )
{
	newNode->prev = prev;
	newNode->next = next;

	if( prev )
		prev->next = newNode;
	else
		head = newNode;

	if( next )
		next->prev = newNode;
	else
		tail = newNode;
}

template< typename Data, class DataManager >
inline typename GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::InsertAfter( Node* node /*= nullptr*/, Node* newNode /*= nullptr*/ )
{
	if( !newNode )
		newNode = new Node;

	if( !node )
		node = tail;

	if( node )
		Insert( newNode, node, node->next );
	else
		Insert( newNode, nullptr, nullptr );

	count++;
	newNode->list = this;

	return newNode;
}

template< typename Data, class DataManager >
inline typename GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::InsertBefore( Node* node /*= nullptr*/, Node* newNode /*= nullptr*/ )
{
	if( !newNode )
		newNode = new Node;

	if( !node )
		node = head;

	if( node )
		Insert( newNode, node->prev, node );
	else
		Insert( newNode, nullptr, nullptr );

	count++;
	newNode->list = this;

	return newNode;
}

template< typename Data, class DataManager >
inline bool GAExpressionLib::List< Data, DataManager >::Remove( Node* oldNode, bool deleteNode /*= true*/ )
{
	if( oldNode->list != this )
		return false;

	if( head == oldNode )
		head = head->next;

	if( tail == oldNode )
		tail = tail->prev;

	if( oldNode->prev )
		oldNode->prev->next = oldNode->next;

	if( oldNode->next )
		oldNode->next->prev = oldNode->prev;

	count--;
	oldNode->list = nullptr;
	oldNode->next = nullptr;
	oldNode->prev = nullptr;

	if( deleteNode )
		delete oldNode;

	return true;
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::RemoveAll( void )
{
	while( count > 0 )
		Remove( head );
}

template< typename Data, class DataManager >
inline int GAExpressionLib::List< Data, DataManager >::Sort( SortCompareFunc sortCompareFunc /*= nullptr*/ )
{
	int adjacentSwapCount = 0;
	bool inOrder;

	do
	{
		inOrder = true;

		Node* nodeA = head;
		while( nodeA )
		{
			int result = 0;

			Node* nodeB = nodeA->next;
			if( nodeB )
			{
				if( sortCompareFunc )
					result = sortCompareFunc( nodeA->data, nodeB->data );
				else
					result = DataManager::Compare( nodeA->data, nodeB->data );
			}

			if( result <= 0 )
				nodeA = nodeB;
			else
			{
				Remove( nodeB, false );
				InsertBefore( nodeA, nodeB );
				inOrder = false;
				adjacentSwapCount++;
			}
		}
	}
	while( !inOrder );

	return adjacentSwapCount;
}

template< typename Data, class DataManager >
inline typename GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Head( void )
{
	return head;
}

template< typename Data, class DataManager >
inline typename GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Tail( void )
{
	return tail;
}

template< typename Data, class DataManager >
inline typename const GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Head( void ) const
{
	return head;
}

template< typename Data, class DataManager >
inline typename const GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Tail( void ) const
{
	return tail;
}

template< typename Data, class DataManager >
inline int GAExpressionLib::List< Data, DataManager >::Count( void ) const
{
	return count;
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::AbsorbAfter( Node* node, List* list )
{
	if( !node )
		node = tail;

	if( node && node->list != this )
		return;

	while( list->count > 0 )
	{
		Node* otherNode = list->head;
		list->Remove( otherNode, false );
		node = InsertAfter( node, otherNode );
	}
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::AbsorbBefore( Node* node, List* list )
{
	if( !node )
		node = head;

	if( node && node->list != this )
		return;

	while( list->count > 0 )
	{
		Node* otherNode = list->tail;
		list->Remove( otherNode, false );
		node = InsertBefore( node, otherNode );
	}
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::CopyAfter( Node* node, const List* list )
{
	if( !node )
		node = tail;

	if( node && node->list != this )
		return;

	for( const Node* otherNode = list->head; otherNode; otherNode = otherNode->Next() )
	{
		node = InsertAfter( node );
		DataManager::Copy( node->data, otherNode->data );
	}
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::CopyBefore( Node* node, const List* list )
{
	if( !node )
		node = head;

	if( node && node->list != this )
		return;

	for( const Node* otherNode = list->tail; otherNode; otherNode = otherNode->Prev() )
	{
		node = InsertBefore( node );
		DataManager::Copy( node->data, otherNode->data );
	}
}

template< typename Data, class DataManager >
inline void GAExpressionLib::List< Data, DataManager >::Reverse( void )
{
	if( count > 1 )
	{
		Node* node = head;
		Remove( node, false );
		Reverse();
		InsertAfter( tail, node );
	}
}

template< typename Data, class DataManager >
inline GAExpressionLib::List< Data, DataManager >::Node::Node( void )
{
	next = nullptr;
	prev = nullptr;
	list = nullptr;

	DataManager::Initialize( data );
}

template< typename Data, class DataManager >
inline GAExpressionLib::List< Data, DataManager >::Node::~Node( void )
{
	DataManager::Finalize( data );
}
		
template< typename Data, class DataManager >
inline typename GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Node::Next( void )
{
	return next;
}

template< typename Data, class DataManager >
inline typename GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Node::Prev( void )
{
	return prev;
}

template< typename Data, class DataManager >
inline typename const GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Node::Next( void ) const
{
	return next;
}

template< typename Data, class DataManager >
inline typename const GAExpressionLib::List< Data, DataManager >::Node* GAExpressionLib::List< Data, DataManager >::Node::Prev( void ) const
{
	return prev;
}

// List.hpp
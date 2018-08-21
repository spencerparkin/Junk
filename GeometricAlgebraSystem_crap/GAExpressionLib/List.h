// List.h

#pragma once

#include "GAExpLibApi.h"

namespace GAExpressionLib
{
	template< typename Data >
	class DumbDataManager;

	template< typename Data >
	class PointerDataManager;

	template< typename Data, class DataManager >
	class List;
}

template< typename Data >
class GA_EXPRESSION_LIB_API GAExpressionLib::DumbDataManager
{
public:

	static void Initialize( Data& data );
	static void Finalize( Data& data );
	static void Copy( Data& dataA, const Data& dataB );
	static int Compare( const Data& dataA, const Data& dataB );
};

template< typename Data >
class GA_EXPRESSION_LIB_API GAExpressionLib::PointerDataManager
{
public:

	static void Initialize( Data& data );
	static void Finalize( Data& data );
	static void Copy( Data& dataA, const Data& dataB );
	static int Compare( const Data& dataA, const Data& dataB );
};

template< typename Data, class DataManager = PointerDataManager< Data > >
class GA_EXPRESSION_LIB_API GAExpressionLib::List
{
public:

	List( void );
	~List( void );

	struct Node
	{
		friend class List;

	public:
		
		Node( void );
		~Node( void );

		Data data;

		Node* Next( void );
		Node* Prev( void );

		const Node* Next( void ) const;
		const Node* Prev( void ) const;

	private:

		Node* next;
		Node* prev;
		
		List* list;
	};

	Node* InsertAfter( Node* node = nullptr, Node* newNode = nullptr );
	Node* InsertBefore( Node* node = nullptr, Node* newNode = nullptr );

	bool Remove( Node* oldNode, bool deleteNode = true );
	void RemoveAll( void );

	typedef int ( *SortCompareFunc )( /*const*/ Data dataA, /*const*/ Data dataB );
	int Sort( SortCompareFunc sortCompareFunc = nullptr );

	Node* Head( void );
	Node* Tail( void );

	const Node* Head( void ) const;
	const Node* Tail( void ) const;

	int Count( void ) const;

	void AbsorbAfter( Node* node, List* list );
	void AbsorbBefore( Node* node, List* list );

	void CopyAfter( Node* node, const List* list );
	void CopyBefore( Node* node, const List* list );

	void Reverse( void );

private:

	inline void Insert( Node* newNode, Node* prev, Node* next );

	Node* head;
	Node* tail;

	int count;
};

#if defined GA_EXPRESSION_LIB_API_EXPORT
#	include "List.hpp"
#endif

// List.h
// LL.cpp

// Programmed by Spencer T. Parkin

#include "LL.h"
#include "main.h"

// Insert the given node into the given linked-list
// before and after the other two given nodes which
// are assumed to be adjacent to one another and
// also members of the given list.  It is also assumed
// that the head and tail of the given list are assigned.
void LL_insert(LL *ll, LL_node *n, LL_node *before, LL_node *after)
{
	// Do a sanity check.
	if(before && before->prev != after ||
		after && after->next != before)
	{
		fatal_error("Misuse of linked-list.");
	}

	// Couple the node into the list.
	n->prev = after;
	n->next = before;
	if(after)
		after->next = n;
	if(before)
		before->prev = n;

	// Adjust the head/tail if nessiary.
	if(ll->head->prev)
		ll->head = ll->head->prev;
	if(ll->tail->next)
		ll->tail = ll->tail->next;

	// Keep track of how many items are in the list.
	ll->num_items++;
}

// Insert the given node before the head.
void LL_insert(LL *ll, LL_node *n)
{
	if(!ll->head)
	{
		ll->head = ll->tail = n;
		n->next = n->prev = 0;
		ll->num_items++;
	}
	else
	{
		LL_insert(ll, n, ll->head, 0 /*ll->head->prev*/);
	}
}

// Append the given node after the tail.
void LL_append(LL *ll, LL_node *n)
{
	if(!ll->head)
		LL_insert(ll, n);
	else
		LL_insert(ll, n, 0 /*ll->tail->next*/, ll->tail);
}

// Remove the given node from the list.
void LL_remove(LL *ll, LL_node *n)
{
	// Adjust the head/tail if nessiary.
	if(ll->head == n)
		ll->head = ll->head->next;
	if(ll->tail == n)
		ll->tail = ll->tail->prev;

	// Decouple the node from the list.
	if(n->next)
		n->next->prev = n->prev;
	if(n->prev)
		n->prev->next = n->next;

	// It's probably good practice to
	// nullify the prev/next pointers for
	// removed nodes, though in some cases
	// it may be convenient to keep them
	// if the node is being deleted from
	// a for-loop or something.  But to be
	// as clean as possible, I'm going to
	// nullify them here.
	n->next = n->prev = 0;

	// Keep track of how many items are in the list.
	ll->num_items--;
}

// endof LL.cpp
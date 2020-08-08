// LL.h

// Programmed by Spencer T. Parkin

#ifndef __LL_h__
#define __LL_h__

// Yes, one of the points of c++ is to
// help you write reuseable code, yet I
// end up writing a link-list every time I
// need one.  I could've used the STL,
// but the STL drives me nuts.  One of these
// days I'll get around to keeping a library
// of personal routines.  This will be a
// quick and dirty doubly-linked list.
struct LL
{
	struct LL_node *head, *tail;
	int num_items;
};
struct LL_node
{
	LL_node(void *data) { this->data = data; }
	void *data;
	LL_node *next, *prev;
};
void LL_insert(LL *ll, LL_node *n, LL_node *before, LL_node *after);
void LL_insert(LL *ll, LL_node *n);
void LL_append(LL *ll, LL_node *n);
void LL_remove(LL *ll, LL_node *n);

#endif __LL_h__

// endof LL.h
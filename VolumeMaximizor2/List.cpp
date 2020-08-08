// List.cpp

#include "List.h"

void WipeList(List *list)
{
	list->head = list->tail = 0;
	list->item_count = 0;
}

int ListInsert(List *list, ListItem *item)
{
	if(item->list)
		return 0;
	item->list = list;
	item->next = list->head;
	item->prev = 0;
	if(item->next)
		item->next->prev = item;
	else
		list->tail = item;
	list->head = item;
	list->item_count++;
	return 1;
}

int ListAppend(List *list, ListItem *item)
{
	if(item->list)
		return 0;
	item->list = list;
	item->next = 0;
	item->prev = list->tail;
	if(item->prev)
		item->prev->next = item;
	else
		list->head = item;
	list->tail = item;
	list->item_count++;
	return 1;
}

int ListRemove(ListItem *item)
{
	if(!item->list)
		return 0;
	List *list = item->list;
	if(list->head == item)
		list->head = item->next;
	if(list->tail == item)
		list->tail = item->prev;
	if(item->next)
		item->next->prev = item->prev;
	if(item->prev)
		item->prev->next = item->next;
	item->next = item->prev = 0;
	item->list = 0;
	list->item_count--;
	return 1;
}

// with looped lists, only the head pointer
// is used to keep track of the list elements.
// you cannot mix the loop-list and non-loop-
// list functions for a single list.  insert
// the given item after the head pointer.
// the nice thing about looped lists is that
// the next/prev pointers are never null.
int LoopListInsert(List *list, ListItem *item)
{
	if(list->head)
	{
		ListItem *before, *after;
		before = list->head;
		after = list->head->next;
		before->next = item;
		after->prev = item;
		item->next = after;
		item->prev = before;
	}
	else
	{
		list->head = item;
		item->next = item;
		item->prev = item;		
	}
	item->list = list;
	list->item_count++;
	return 1;
}

// notice that if you remove the head
// pointer, then the head is adjusted
// to the item prior to it.  this must
// be paid attention to if you are
// removing items from the list in a loop.
int LoopListRemove(ListItem *item)
{
	List *list = item->list;
	if(list->item_count == 1)
		list->head = 0;
	else if(list->head == item)
		list->head = item->prev;
	item->prev->next = item->next;
	item->next->prev = item->prev;
	item->next = 0;
	item->prev = 0;
	list->item_count--;
	return 1;
}

// endof List.cpp
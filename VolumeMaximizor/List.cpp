// List.cpp

// This software was written by Spencer T. Parkin, Copyright (c) 2007, All Rights Reserved.

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

// endof List.cpp
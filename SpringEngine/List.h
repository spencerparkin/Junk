// List.h

#ifndef __List_h__
#define __List_h__

// one bumber is that list items
// cannot be members of more than
// one list at the same time.  but
// we won't need that functionality here.
struct ListItem
{
	ListItem *next, *prev;
	struct List *list;
};

struct List
{
	ListItem *head, *tail;
	int item_count;
};

void WipeList(List *list);
int ListInsert(List *list, ListItem *item);
int ListAppend(List *list, ListItem *item);
int ListRemove(ListItem *item);
int LoopListInsert(List *list, ListItem *item);
int LoopListRemove(ListItem *item);

#endif //__List_h__

// endof List.h
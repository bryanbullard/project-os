/**
 *    Copyright (c) 2003, 2006 Bryan Bullard.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without 
 * restriction, including without limitation the rights to use, 
 * copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom 
 * the Software is furnished to do so, subject to the following 
 * conditions:
 *
 *   The above copyright notice and this permission notice 
 *   shall be included in all copies or substantial portions 
 *   of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
 * OTHER DEALINGS IN THE SOFTWARE.
 *
**/


#include <totem/base.h>
#include <totem/linklist.h>



/***
 ** Double and Single Linked List Implementation
 **
 **/


/** 
 * Place a node on the head of double linked list 
 */
DLINK_NODE_T*
DLinkPushHead(
	IN OUT DLINK_LIST_T* list,
	IN OUT DLINK_NODE_T* node,
	IN OUT SPINLOCK_T* lock
	)
{
	node->next = list->head;
	node->prev = NULL;
   
	if (node->next != NULL)
		node->next->prev = node;
	else
		list->tail = node;
		
	list->head = node;
   
	++(list->depth);
	
	return node->next;
}


/** 
 * Place a node on the end of double linked list 
 */
DLINK_NODE_T*
DLinkPushTail(
	IN OUT DLINK_LIST_T* list, 
	IN OUT DLINK_NODE_T* node,
	IN OUT SPINLOCK_T* lock
	)
{  	
   	node->next = NULL;
	node->prev = list->tail;
   
	if (node->prev != NULL)
		node->prev->next = node;
	else
		list->head = node;

	list->tail = node;

	++(list->depth);
	
	return node->prev;
}

/** 
 * Inserts a node into the list after a double linked node 
 */
DLINK_NODE_T*
DLinkInsertAfter(
	IN OUT DLINK_LIST_T* list, 
	IN OUT DLINK_NODE_T* node, 
	IN OUT DLINK_NODE_T* after,
	IN OUT SPINLOCK_T* lock
	)
{
	node->next = after->next;
	node->prev = after;
   
	if (after->next != NULL)
		after->next->prev = node;
	else
		list->tail = node;
   
	after->next = node;
   
	++(list->depth);
	
	return node->next;
}

/** 
 * Inserts a node into the list before a double linked node 
 */
DLINK_NODE_T* 
DLinkInsertBefore(
	IN OUT DLINK_LIST_T* list, 
	IN OUT DLINK_NODE_T* node, 
	IN OUT DLINK_NODE_T* before, 
	IN OUT SPINLOCK_T* lock
	)
{
	node->prev = before->prev;
	node->next = before;
   
	if (before->prev != NULL)
		before->prev->next = node;
	else
		list->head = node;
   
	before->prev = node;
   
	++(list->depth);
	
	return node->prev;
}

/**
 * Removes the specified node from the double linked list 
 */
DLINK_NODE_T*
DLinkRemove(	
	IN OUT DLINK_LIST_T* list, 
	IN OUT DLINK_NODE_T* node,
	IN OUT SPINLOCK_T* lock
	)
{
	if (node->prev == NULL)
		list->head = node->next;
	else
		node->prev->next = node->next;
   
	if (node->next == NULL)
		list->tail = node->prev;
	else
		node->next->prev = node->prev;
      
	--(list->depth);
	
	return node;
}

/** 
 * Flush an entire double linked list 
 */
VOID 
DLinkFlushList( 
	IN OUT DLINK_LIST_T* list,
	IN OUT SPINLOCK_T* lock	
	)
{
	DLINK_NODE_T* node = NULL;
	
	while (list->head != NULL)
	{
		node = list->head;
		list->head = list->head->next;	
		node->next = NULL;
		node->prev = NULL;
	}
	
	list->tail = NULL;
	list->depth = 0;
}

/** 
 * Initialize a double linked list structure 
 */
VOID 
InitializeDLinkList(
	OUT DLINK_LIST_T* list
	)
{
	list->depth = 0;
	list->head = NULL; 
	list->tail = NULL;
}

/**
 * Push a node to the head of a single linked list 
 */
SLINK_NODE_T*
SLinkPush(
	IN OUT SLINK_LIST_T* list, 
	IN OUT SLINK_NODE_T* node,
	IN OUT SPINLOCK_T* lock
	)
{   
	node->next = list->head;
	list->head = node;
   
	++(list->depth);
	
	return node->next;
}

/**
 * Pop a node from the head of a single linked list 
 */
SLINK_NODE_T* 
SLinkPop(
	IN OUT SLINK_LIST_T* list,
	IN OUT SPINLOCK_T* lock
	)
{
	SLINK_NODE_T* node = list->head;
	list->head = list->head->next;
	node->next = NULL;
   
	--(list->depth);
	
	return node;
}

/** 
 * Flush an entire single linked list 
 */
VOID 
SLinkFlushList( 
	IN OUT SLINK_LIST_T* list,
	IN OUT SPINLOCK_T* lock
	)
{
	SLINK_NODE_T* node = NULL;
	
	while (list->head != NULL)
	{
		node = list->head;
		list->head = list->head->next;	
		node->next = NULL;
	}
	
	list->depth = 0;
}

/** 
 * Initialize a single linked list structure 
 */
VOID 
InitializeSLinkList(
	OUT SLINK_LIST_T* list
	)
{
	list->depth = 0;
	list->head = NULL; 
}

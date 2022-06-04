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

#ifndef _TOTEM_LINKLIST_H
#define _TOTEM_LINKLIST_H


#include <totem/base.h>
#include <totem/spinlock.h>

typedef struct dlink_node DLINK_NODE_T;
typedef struct dlink_list DLINK_LIST_T;
typedef struct slink_node SLINK_NODE_T;
typedef struct slink_list SLINK_LIST_T;

/**
 * Single linked list implementation w/inter-locking 
 */
struct slink_node
{
	SLINK_NODE_T*	next;
	VOID*		data;
};

struct slink_list
{
	SLINK_NODE_T*	head;
	ULONG_T		depth;
};

SLINK_NODE_T* SLinkPush( SLINK_LIST_T* list, SLINK_NODE_T* node, SPINLOCK_T* lock);
SLINK_NODE_T* SLinkPop( SLINK_LIST_T* list, SPINLOCK_T* lock);
VOID SLinkFlushList( SLINK_LIST_T* list, SPINLOCK_T* lock);
VOID InitializeSLinkList( SLINK_LIST_T* list );

#define INITIALIZE_SLINK_NODE(n)		((n).next = (n).data = NULL)
#define INITIALIZE_SLINK_NODE_POINTER(n)	((n)->next = (n)->data = NULL)

/**
 * Double linked list implementation w/inter-locking 
 */
struct dlink_node
{
	DLINK_NODE_T*	next;
	DLINK_NODE_T*	prev;
	VOID*		data;
};

struct dlink_list
{
	DLINK_NODE_T*	head;
	DLINK_NODE_T*	tail;
	ULONG_T		depth;
	ULONG_T		reserved;
};

DLINK_NODE_T* DLinkPushHead( DLINK_LIST_T* list, DLINK_NODE_T* node, SPINLOCK_T* lock );
DLINK_NODE_T* DLinkPopHead( DLINK_LIST_T* list, DLINK_NODE_T* node, SPINLOCK_T* lock );
DLINK_NODE_T* DLinkPushTail( DLINK_LIST_T* list, DLINK_NODE_T* node, SPINLOCK_T* lock );
DLINK_NODE_T* DLinkPopTail( DLINK_LIST_T* list, DLINK_NODE_T* node, SPINLOCK_T* lock );
DLINK_NODE_T* DLinkInsertAfter( DLINK_LIST_T* list, DLINK_NODE_T* node, DLINK_NODE_T* after, SPINLOCK_T* lock );
DLINK_NODE_T* DLinkInsertBefore( DLINK_LIST_T* list, DLINK_NODE_T* node, DLINK_NODE_T* before, SPINLOCK_T* lock );
DLINK_NODE_T* DLinkRemove( DLINK_LIST_T* list, DLINK_NODE_T* node, SPINLOCK_T* lock );
VOID DLinkFlushList( DLINK_LIST_T* list, SPINLOCK_T* lock );
VOID InitializeDLinkList( DLINK_LIST_T* list );

#define INITIALIZE_DLINK_NODE(n)		((n).next = (n).prev = (n).data = NULL)
#define INITIALIZE_DLINK_NODE_POINTER(n)	((n)->next = (n)->prev = (n)->data = NULL)

#endif /* _TOTEM_LINKLIST_H */

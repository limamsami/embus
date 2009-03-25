#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "llist.h"
#include "embus_notifier.h"

/* Construct notifier pool */
int embus_notifier_pool_construct(embus_notifier_pool_t *pool)
{
	pool->nr_notifier_head = 0;

	INIT_LIST_HEAD(&pool->head);

	return 0;
}

/* Destruct notifier pool */
void embus_notifier_pool_destruct(embus_notifier_pool_t *pool)
{
	embus_notifier_head_t *h;

	struct list_head *pos, *n;

	if (pool->nr_notifier_head) {
		list_for_each_safe(pos, n, &pool->head) {
			h = list_entry(pos, embus_notifier_head_t, list);
			embus_notifier_head_free(h);
		}
	}
	
	return;
}

/* Allocate notifier head */
embus_notifier_head_t *embus_notifier_head_alloc(embus_notifier_pool_t *pool, char *name)
{
	embus_notifier_head_t *h;

	h = malloc(sizeof(embus_notifier_head_t));
	if (!h)
		return NULL;

	strncpy(h->name, name, EMBUS_SZ_HEAD_NAME);

	h->nr_node = 0;

	h->pool = pool;

	INIT_LIST_HEAD(&h->node_head);
	INIT_LIST_HEAD(&h->list);

	pool->nr_notifier_head ++;

	list_add_tail(&h->list, &pool->head);

	return h;
}

/* Free notifier head */
void embus_notifier_head_free(embus_notifier_head_t *head)
{
	embus_notifier_node_t *node;
	struct list_head *pos, *n;

	if (head->nr_node) {
		list_for_each_safe(pos, n, &head->node_head) {
			node = list_entry(pos, embus_notifier_node_t, node_list);	
			embus_notifier_detach(node);
			embus_notifier_node_free(node);
		}
	}

	list_del(&head->list);

	head->pool->nr_notifier_head --;

	free(head);

	return;
}

/* Allocate notifier node */
embus_notifier_node_t *embus_notifier_node_alloc(embus_notify_fn_t *notify, void *data)
{
	embus_notifier_node_t *node;

	node = malloc(sizeof(embus_notifier_node_t));
	if (!node)
		return NULL;

	node->notify = notify;
	node->data = data;

	INIT_LIST_HEAD(&node->node_list);
	node->head = NULL;

	return node;
}

/* Setup notifier node for reuse */
int embus_notifier_node_setup(embus_notifier_node_t *node, embus_notify_fn_t *notify, void *data)
{
	if (node->head)
		return -1;

	node->notify = notify;
	node->data = data;
	
	INIT_LIST_HEAD(&node->node_list);
	
	return 0;
}

/* Free notifier node */
void embus_notifier_node_free(embus_notifier_node_t *node)
{
	node->notify(node, NULL);

	if (node->head)
		embus_notifier_detach(node);
	
	free(node);

	return;
}

/* Attach node to notifier head */
int embus_notifier_attach(embus_notifier_head_t *head, embus_notifier_node_t *node)
{
	list_add_tail(&node->node_list, &head->node_head);
	
	node->head = head;
	head->nr_node ++;

	return 0;
}

/* Detach node from notifier head */
int embus_notifier_detach(embus_notifier_node_t *node)
{
	list_del(&node->node_list);
	
	node->head->nr_node --;
	node->head = NULL;

	return 0;
}

/* Do event notifier on notifier head */
int embus_notifier_do_notify(embus_notifier_head_t *head, void *event)
{
	embus_notifier_node_t *node;

	struct list_head *pos, *n;
	
	/* Null event keep for node free event */
	if (!event)
		return -1;

	list_for_each_safe(pos, n, &head->node_head) {
		node = list_entry(pos, embus_notifier_node_t, node_list);	
		node->notify(node, event);
	}

	return 0;
}

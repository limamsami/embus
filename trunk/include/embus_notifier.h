/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Event Notifier */

#ifndef EMBUS_NOTIFIER_H
#define EMBUS_NOTIFIER_H

#include <stdint.h>
#include "llist.h"

#define EMBUS_SZ_HEAD_NAME 	16

typedef struct __embus_notifier_node embus_notifier_node_t;
typedef struct __embus_notifier_head embus_notifier_head_t;
typedef struct __embus_notifier_pool embus_notifier_pool_t;

typedef void embus_notify_fn_t(embus_notifier_node_t *, void *);

/* Notifier Block Node */
struct __embus_notifier_node {
	embus_notifier_head_t *head;

	void *data;
	embus_notify_fn_t *notify;

	struct list_head node_list;
};

/* Notifier Block Head */
struct __embus_notifier_head {
	char name[EMBUS_SZ_HEAD_NAME + 1];
	
	uint32_t nr_node;

	embus_notifier_pool_t *pool;

	struct list_head node_head;
	struct list_head list;
};

struct __embus_notifier_pool {
	uint32_t nr_notifier_head;
	struct list_head head;
};

int embus_notifier_pool_construct(embus_notifier_pool_t *pool);

void embus_notifier_pool_destruct(embus_notifier_pool_t *pool);

embus_notifier_head_t *embus_notifier_head_alloc(embus_notifier_pool_t *pool, char *name);

void embus_notifier_head_free(embus_notifier_head_t *head);

embus_notifier_node_t *embus_notifier_node_alloc(embus_notify_fn_t *notify, void *data);

int embus_notifier_node_setup(embus_notifier_node_t *node, embus_notify_fn_t *notify, void *data);

void embus_notifier_node_free(embus_notifier_node_t *node);

int embus_notifier_attach(embus_notifier_head_t *head, embus_notifier_node_t *node);

int embus_notifier_detach(embus_notifier_node_t *node);

int embus_notifier_do_notify(embus_notifier_head_t *head, void *event);

#endif /* Define EMBUS_NOTIFIER_H */


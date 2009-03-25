#include <stdio.h>
#include <stdlib.h>

#include "embus_notifier.h"

#define NR_MAX_NODE 16

void my_notify(embus_notifier_node_t *node, void *event)
{
	if (event)
		fprintf(stderr, "Node %d: Receive Event: %d.\n", (int)node->data, (int)event);
	else{
		fprintf(stderr, "Node %d: Receive Destruct Event. Free node.\n", (int)node->data);
	}	

	return;
}

int main(void)
{
	embus_notifier_pool_t pool;
	embus_notifier_head_t *head;
	embus_notifier_node_t *node[NR_MAX_NODE];
	
	int i;
	int ret;

	ret = embus_notifier_pool_construct(&pool);
	if (ret) {
		fprintf(stderr, "Failed to construct notifier pool.\n");
		return -1;
	}

	head = embus_notifier_head_alloc(&pool, "my_head");
	if (!head) {
		fprintf(stderr, "Failed to alloc head.\n");
		return -1;
	}

	for (i = 0; i < NR_MAX_NODE; i++) {
		node[i] = embus_notifier_node_alloc(my_notify, (void *)i);
		if (!node[i]) {
			fprintf(stderr, "Failed to alloc node.\n");
			return -1;
		}

		ret = embus_notifier_attach(head, node[i]);
		if (ret) {
			fprintf(stderr, "Failed to attach head.\n");
			return -1;
		}
	}
	
	embus_notifier_do_notify(head, (void *)2);
	embus_notifier_head_free(head);

	return 0;
}

/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS DEBUG function */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "embus_def.h"
#include "embus_msg.h"

void embus_debug_dump_msg_head_info(embus_msg_head_t *h)
{
	fprintf(stderr, "Message Head Information Dump:\n");
	fprintf(stderr, "=====================================\n");
	fprintf(stderr, "MSG:\n");
	fprintf(stderr, "MAGIC: 0x%x.\n", h->magic);
	fprintf(stderr, "TYPE: 0x%x.\n", h->msg_type);
	fprintf(stderr, "ATTR: 0x%x.\n", h->msg_attr);
	fprintf(stderr, "LEN: %d.\n", h->msg_len);
	fprintf(stderr, "TO: %s.\n", h->to.name);
	fprintf(stderr, "TO_MOD_TYPE: 0x%x.\n", h->to_mod_type);
	fprintf(stderr, "=====================================\n");
	
	return;
}


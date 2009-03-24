/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Debug Function */

#ifndef EMBUS_DEBUG_H
#define EMBUS_DEBUG_H

#ifdef __EMBUS_DEBUG__
#include <stdio.h>
#include <embus_msg.h>

#define DBG(x...) do {	\
	fprintf(stderr, "File: %s, Function: %s, Line: %d: ", __FILE__, __func__, __LINE__);	\
	fprintf(stderr, x);	\
	fprintf(stderr, "\n");	\
}while(0)

void embus_debug_dump_msg_head_info(embus_msg_head_t *h);

#else
#define embus_debug_dump_msg_head_info(x)
#define DBG(x)
#endif

#endif /* Define EMBUS_DEBUG_H */

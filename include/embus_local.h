/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Local IPC Implementation */

#ifndef EMBUS_LOCAL_H
#define EMBUS_LOCAL_H

#include "embus_ctx.h"

typedef struct {
	embus_ctx_t *embus_ctx;
	int sd;
}embus_local_ctx_t;

int embus_local_ctx_construct(embus_ctx_t *ec);

void embus_local_ctx_destruct(embus_ctx_t *ec);

#endif /* Defien EMBUS_LOCAL_H */

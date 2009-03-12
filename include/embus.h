/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Control Interface */

#ifndef EMBUS_H
#define EMBUS_H

#include "embus_def.h"
#include "embus_ctx.h"

embus_ctx_t *embus_construct(embus_name_t *name);

void embus_destruct(embus_ctx_t *ec);

int embus_send(embus_ctx_t *ec, embus_msg_t *m);

int embus_recv(embus_ctx_t *ec, embus_conn_t *conn, embus_msg_t *r, int timeout);

int embus_reply(embus_ctx_t *ec, embus_conn_t *conn, embus_msg_t *r);

int embus_send_with_reply(embus_ctx_t *ec, embus_msg_t *s, embus_msg_t *r, int timeout);

#endif /* Define EMBUS_H */

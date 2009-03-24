/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Control Interface */

#include <stdlib.h>
#include <string.h>

#include "embus_def.h"
#include "embus_common.h"
#include "embus_ctx.h"
#include "embus_msg.h"

#include "embus_local.h"
#include "embus.h"
#include "embus_debug.h"

embus_ctx_t *embus_construct(embus_name_t *name)
{
	embus_ctx_t *ec;

	int ret;

	ec = (embus_ctx_t *)embus_zalloc(sizeof(embus_ctx_t));
	if (!ec)
		return NULL;
	
	memcpy(&ec->name, name, sizeof(embus_name_t));

	ret = embus_local_ctx_construct(ec);
	if (ret) {
		free(ec);
		return NULL;
	}

	return ec;
}

void embus_destruct(embus_ctx_t *ec)
{
	embus_local_ctx_destruct(ec);
	free(ec);

	return;
}

static int embus_do_send(embus_ctx_t *ec, embus_conn_t *conn, embus_msg_t *m)
{
	embus_ctx_io_t *io = ec->io;

	int ret;

	m->head.magic = EMBUS_MSG_MAGIC;

	switch (m->head.msg_type) {
		case EMBUS_MSG_TYPE_PEER:
			switch (m->head.to_mod_type) {
				case EMBUS_MOD_TYPE_LOCAL:
					ret = io->send(conn, m, 0);
					if (ret == -1) {
						goto err;
					}

					break;

				case EMBUS_MOD_TYPE_REMOTE:
					return -1;
			}	

			break;

		case EMBUS_MSG_TYPE_BROADCAST:
			return -1;
	}

	return 0;

err:
	return -1;
}

int embus_send(embus_ctx_t *ec, embus_msg_t *m)
{
	embus_conn_t conn;
	embus_ctx_io_t *io = ec->io;
	embus_name_t *p;

	int ret;

	if (m->head.msg_type == EMBUS_MSG_TYPE_PEER && m->head.to_mod_type == EMBUS_MOD_TYPE_LOCAL) {
		p = &m->head.to;
	}else
		return -1;

	ret = io->connect(ec, &conn, p);
	if (ret == -1)
		return ret;

	ret = embus_do_send(ec, &conn, m);
	if (ret == -1) {
		io->disconnect(&conn);
		return -1;
	}

	io->disconnect(&conn);

	return 0;
}

int embus_recv(embus_ctx_t *ec, embus_conn_t *conn, embus_msg_t *r, int timeout)
{
	embus_ctx_io_t *io = ec->io;

	int ret;

	ret = io->wait(ec, conn, timeout);
	if (ret == -1) {
		DBG("Failed to wait connection.");
		return ret;
	}

	ret = io->recv(conn, r, 0);
	if (ret == -1) {
		DBG("Failed to recv data.");
		io->disconnect(conn);
		return ret;
	}

	if (!(r->head.msg_attr & EMBUS_MSG_ATTR_NEED_REPLY)) {
		io->disconnect(conn);
		return 0;
	}
	
	return 0;
}

int embus_reply(embus_ctx_t *ec, embus_conn_t *conn, embus_msg_t *r)
{
	embus_ctx_io_t *io = ec->io;

	int ret;

	ret = io->send(conn, r, 0);
	if (ret == -1) {
		io->disconnect(conn);
		return -1;
	}
	
	io->disconnect(conn);
	return 0;
}

int embus_send_with_reply(embus_ctx_t *ec, embus_msg_t *s, embus_msg_t *r, int timeout)
{
	embus_ctx_io_t *io = ec->io;
	embus_conn_t conn;

	int ret;
	
	s->head.msg_attr |= EMBUS_MSG_ATTR_NEED_REPLY;

	switch (s->head.msg_type) {
		case EMBUS_MSG_TYPE_PEER:
			switch (s->head.to_mod_type) {
				case EMBUS_MOD_TYPE_LOCAL:
					ret = io->connect(ec, &conn, &s->head.to);
					if (ret == -1) {
						DBG("Failed to connect to target module %s.\n", s->head.to.name);
						return ret;
					}

					ret = embus_do_send(ec, &conn, s);
					if (ret == -1) {
						DBG("Failed to do embus_do_send.\n");
						io->disconnect(&conn);
						return -1;
					}

					ret = io->recv(&conn, r, timeout);
					if (ret == -1) {
						DBG("Failed to receive data.\n");
						io->disconnect(&conn);
						return -1;
					}

					io->disconnect(&conn);

					break;

				default:
					DBG("Unknown module type.");
					return -1;
			}

			break;

		default:
			DBG("Unknown message type.");
			return -1;
	}

	return 0;
}

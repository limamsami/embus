/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Local IPC Implementation */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

#include "embus_debug.h"
#include "embus_common.h"
#include "embus_msg.h"
#include "embus_ctx.h"
#include "embus_local.h"

#define UNIX_PATH_MAX 108

static void embus_name_to_unix_name(embus_name_t *ename, char *uname)
{
	memset(uname, 0, UNIX_PATH_MAX);
	memcpy(uname + 1, ename->name, EMBUS_SZ_MOD_NAME);
	
	return;
}

static void unix_name_to_embus_name(char *uname, socklen_t uname_len, embus_name_t *ename)
{
	memset(ename, 0, sizeof(embus_name_t));
	memcpy(ename->name, uname + 1, uname_len - 1);

	return;
}

static int embus_open_local_server(embus_local_ctx_t *c)
{
	embus_ctx_t *ec = c->embus_ctx;

	struct sockaddr_un my_end;
	struct sigaction sa;

	socklen_t socklen;

	int ret;

	ret = socket(PF_UNIX, SOCK_STREAM, 0);
	if (ret == -1)
		return ret;

	c->sd = ret;
	
	my_end.sun_family = AF_UNIX;
	embus_name_to_unix_name(&ec->name, my_end.sun_path);
	
	socklen = sizeof(my_end.sun_family) + strlen(my_end.sun_path + 1) + 1;
	ret = bind(c->sd, (struct sockaddr *)&my_end, socklen);
	if (ret == -1)
		goto err;

	ret = listen(c->sd, 10);
	if (ret) 
		goto err;
	
	embus_set_nonblock(c->sd);

	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	
	ret = sigaction(SIGPIPE, &sa, NULL);
	if (ret == -1) 
		goto err;

	return 0;
err:
	close(c->sd);
	return -1;
}

static void embus_close_local_server(embus_local_ctx_t *c)
{
	close(c->sd);
	return;
}

static int embus_local_connect(embus_ctx_t *ec, embus_conn_t *conn, embus_name_t *name)
{
	struct sockaddr_un his_end;
	socklen_t socklen;

	int sd;
	int ret;
	
	sd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sd == -1)
		return -1;

	his_end.sun_family = AF_UNIX;
	embus_name_to_unix_name(name, his_end.sun_path);

	socklen = sizeof(his_end.sun_family) + strlen(his_end.sun_path + 1) + 1;

re_connect:
	ret = connect(sd, (struct sockaddr *)&his_end, socklen);
	if (ret == -1 && errno == EINTR)
		goto re_connect;

	if (ret == -1) {
		close(sd);
		return -1;
	}

	embus_set_nonblock(sd);

	conn->sd = sd;
	conn->embus_ctx = ec;
	memcpy(&conn->name, name, sizeof(embus_name_t));

	return 0;
}

static int embus_local_send(embus_conn_t *conn, embus_msg_t *m, int timeout)
{
	embus_ctx_io_t *io = conn->embus_ctx->io;

	if (!timeout)
		timeout = io->io_timeout;

	return embus_send_with_timeout(conn->sd, m, sizeof(embus_msg_head_t) + m->head.msg_len, timeout);
}

static int embus_local_recv(embus_conn_t *conn, embus_msg_t *m, int timeout)
{
	embus_ctx_io_t *io = conn->embus_ctx->io;

	int ret;

	if (!timeout)
		timeout = io->io_timeout;
	
	ret = embus_recv_with_timeout(conn->sd, m, sizeof(embus_msg_head_t), timeout);
	if (ret == -1) {
		DBG("Error on embus_recv_with_timeout when get message head.");
		return -1;
	}

	if (m->head.magic != EMBUS_MSG_MAGIC) {
		DBG("Invalid EMBUS Magic number.");
		embus_debug_dump_msg_head_info(&m->head);
		return -1;
	}

	ret = embus_recv_with_timeout(conn->sd, m->data, m->head.msg_len, timeout);
	if (ret == -1) {
		DBG("Error on embus_recv_with_timeout when get message body.");
		embus_debug_dump_msg_head_info(&m->head);
		return -1;
	}

	return 0;
}

static int embus_local_wait(embus_ctx_t *ec, embus_conn_t *conn, int timeout)
{
	embus_local_ctx_t *c = ec->ctx;
	embus_ctx_io_t *io = ec->io;
	
	struct sockaddr_un his_end;
	struct pollfd pollfd;
	
	socklen_t his_len;

	int ret;

	if (!timeout)
		timeout = io->io_timeout;

	pollfd.fd = c->sd;
	pollfd.events = POLLIN;

	while (1) {
		ret = poll(&pollfd, 1, timeout);
		if (ret == -1 && errno == EINTR) 
			continue;
		
		if (ret == -1) {
			DBG("Failed to POLL.");
			return -1;
		}

		if (!ret) {
			DBG("Wait connection timeout.");
			return 0;
		}

		if (pollfd.revents & POLLIN)
			break;
	}

re_accept:
	his_len = sizeof(his_end);

	ret = accept(c->sd, (struct sockaddr *)&his_end, &his_len);
	if ((ret == -1 && errno == EINTR)) 
		goto re_accept;

	if (ret == -1) {
		DBG("Failed to accept.");
		return -1;
	}

	unix_name_to_embus_name(his_end.sun_path, his_len, &conn->name);

	conn->sd = ret; 
	conn->embus_ctx = ec;

	return 0;
}

static void embus_local_disconnect(embus_conn_t *conn)
{
	close(conn->sd);

	return;
}

static embus_ctx_io_t embus_local_ctx_io = {
	.connect = embus_local_connect,
	.send = embus_local_send,
	.recv = embus_local_recv,
	.disconnect = embus_local_disconnect,
	.wait = embus_local_wait,
};

int embus_local_ctx_construct(embus_ctx_t *ec)
{	
	embus_local_ctx_t *c;

	int ret;

	c = (embus_local_ctx_t *)embus_zalloc(sizeof(embus_local_ctx_t));
	if (!c) 
		return -1;

	c->embus_ctx = ec;

	ec->ctx = c;
	ec->type = EMBUS_CTX_TYPE_LOCAL;
	ec->io = &embus_local_ctx_io;

	ret = embus_open_local_server(c);
	if (ret)
		goto err;

	return 0;
err:
	free(c);
	ec->type = EMBUS_CTX_TYPE_NONE;
	ec->io = NULL;
	ec->ctx = NULL;

	return -1;
}

void embus_local_ctx_destruct(embus_ctx_t *ec)
{
	embus_local_ctx_t *c = (embus_local_ctx_t *)ec->ctx;

	if (ec->type != EMBUS_CTX_TYPE_LOCAL || ec->io != &embus_local_ctx_io)
		return;

	embus_close_local_server(c);
	free(c);

	ec->type = EMBUS_CTX_TYPE_NONE;
	ec->io = NULL;
	ec->ctx = NULL;

	return;
}



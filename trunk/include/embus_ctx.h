/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Context */

#ifndef EMBUS_CTX_H
#define EMBUS_CTX_H

#include "embus_def.h"
#include "embus_msg.h"

struct __embus_conn;
typedef struct __embus_conn embus_conn_t;

struct __embus_ctx_io;
typedef struct __embus_ctx_io embus_ctx_io_t;

struct __embus_ctx;
typedef struct __embus_ctx embus_ctx_t;

typedef enum {
	EMBUS_CTX_TYPE_NONE = 0,
	EMBUS_CTX_TYPE_LOCAL,
	EMBUS_CTX_TYPE_TCP,
}embus_ctx_type_t;

struct __embus_ctx {
	embus_ctx_type_t type;
	embus_name_t name;

	void *ctx;
	embus_ctx_io_t *io;
};

struct __embus_conn {
	int sd;
	embus_name_t name;
	embus_ctx_t *embus_ctx;
};

struct __embus_ctx_io {
	int io_timeout;
	
	int (*connect)(embus_ctx_t *, embus_conn_t *, embus_name_t *);
	int (*send)(embus_conn_t *, embus_msg_t *, int);
	int (*recv)(embus_conn_t *, embus_msg_t *, int);
	int (*wait)(embus_ctx_t *, embus_conn_t *, int);
	void (*disconnect)(embus_conn_t *);
};

#endif /* Define EMBUS_CTX_H */

/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Local Bus Message */

#ifndef EMBUS_MSG_H
#define EMBUS_MSG_H

#include <stdint.h>

#include "embus_def.h"

#define EMBUS_MSG_MAGIC		0xdeadbeef

enum {
	EMBUS_MSG_TYPE_PEER = 0,
	EMBUS_MSG_TYPE_BROADCAST,
	EMBUS_MSG_TYPE_PROXY,
};

enum {
	EMBUS_MSG_ATTR_NEED_REPLY = (1 << 0),
};

typedef struct {
	uint32_t magic;

	uint32_t msg_type;
	uint32_t msg_attr;
	uint32_t msg_len;

	embus_name_t from;
	embus_name_t to;
	uint32_t to_mod_type;
}embus_msg_head_t;


typedef struct {
	embus_msg_head_t head;
	char data[1];
}embus_msg_t;

#define EMBUS_MSG_LEN(x) (sizeof(embus_msg_t) + x -1)

int embus_setup_local_peer_msg(embus_msg_t *m, embus_name_t *from, embus_name_t *to, uint32_t len);

#endif

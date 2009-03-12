/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Message Functions */

#include <stdio.h>
#include <string.h>

#include "embus_common.h"
#include "embus_msg.h"

int embus_setup_local_peer_msg(embus_msg_t *m, embus_name_t *from, embus_name_t *to, uint32_t len)
{
	memcpy(&m->head.from, from, sizeof(embus_name_t));
	memcpy(&m->head.to, to, sizeof(embus_name_t));
	
	m->head.magic = EMBUS_MSG_MAGIC;
	m->head.msg_type = EMBUS_MSG_TYPE_PEER;
	m->head.msg_len = len;
	m->head.to_mod_type = EMBUS_MOD_TYPE_LOCAL;

	return 0;
}

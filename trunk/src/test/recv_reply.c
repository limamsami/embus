#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "embus.h"
#include "embus_msg.h"
#include "embus_common.h"

#define MY_MOD_NAME "recv"

#define REPLY_MSG "Hello Sender!"

#define BUF_SIZE (1024)

int main(void)
{
	embus_ctx_t *ec;
	embus_name_t n;
	embus_msg_t *s, *r;
	embus_conn_t conn;

	char sbuf[EMBUS_MSG_LEN(BUF_SIZE)];
	char rbuf[EMBUS_MSG_LEN(BUF_SIZE)];

	int ret;

	ec = embus_construct(embus_make_name(&n, MY_MOD_NAME));
	if (!ec) {
		return -1;	
	}
	
	ec->io->io_timeout = -1;

	s = (embus_msg_t *)sbuf;
	r = (embus_msg_t *)rbuf;

	strcpy(r->data, REPLY_MSG);

	while (1) {
		ret = embus_recv(ec, &conn, s, -1);
		if (ret == -1) {
			fprintf(stderr, "ERROR.\n");
			sleep(1);
			continue;
		}

		fprintf(stderr, "\nMSG:\n");
		fprintf(stderr, "MAGIC: 0x%x.\n", s->head.magic);
		fprintf(stderr, "TYPE: 0x%x.\n", s->head.msg_type);
		fprintf(stderr, "ATTR: 0x%x.\n", s->head.msg_attr);
		fprintf(stderr, "LEN: %d.\n", s->head.msg_len);
		fprintf(stderr, "TO: %s.\n", s->head.to.name);
		fprintf(stderr, "TO_MOD_TYPE: 0x%x.\n", s->head.to_mod_type);
		fprintf(stderr, "CONTENT: %s.\n", s->data);

		if (s->head.msg_attr & EMBUS_MSG_ATTR_NEED_REPLY) {
			fprintf(stderr, "====> Reply Message.\n");
			
			memcpy(&r->head.to, &conn.name, sizeof(embus_name_t));
			embus_setup_local_peer_msg(r, strlen(REPLY_MSG) + 1);

			ret = embus_reply(ec, &conn, r);
			if (ret == -1) {
				fprintf(stderr, "ERROR.\n");
				sleep(1);
				continue;
			}
		}
	}

	return 0;
}

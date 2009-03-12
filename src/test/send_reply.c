#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "embus.h"
#include "embus_common.h"

#define MY_MOD_NAME "send"
#define HIS_MOD_NAME "recv"
#define SMSG "Hello Recv!"

#define BUF_SIZE (1024)

int main(void)
{
	embus_ctx_t *ec;
	embus_name_t n;
	embus_msg_t *s, *r;

	char *sbuf = malloc(EMBUS_MSG_LEN(BUF_SIZE));
	char *rbuf = malloc(EMBUS_MSG_LEN(BUF_SIZE));
	
	int ret;
	
	if (!sbuf || !rbuf) 
		return -1;

	ec = embus_construct(embus_make_name(&n, MY_MOD_NAME));
	if (!ec) {
		return -1;	
	}
	
	ec->io->io_timeout = -1;
	
	s = (embus_msg_t *)sbuf;

	embus_make_name(&s->head.to, HIS_MOD_NAME);
	embus_setup_local_peer_msg(s, strlen(SMSG) + 1);
	strcpy(s->data, SMSG);
	
	r = (embus_msg_t *)rbuf;

	while (1) {
		ret = embus_send_with_reply(ec, s, r, 0);
		if (ret == -1) {
			fprintf(stderr, "#");
			sleep(1);
			continue;
		}

		fprintf(stderr, "\nMSG:\n");
		fprintf(stderr, "MAGIC: 0x%x.\n", r->head.magic);
		fprintf(stderr, "TYPE: 0x%x.\n", r->head.msg_type);
		fprintf(stderr, "ATTR: 0x%x.\n", r->head.msg_attr);
		fprintf(stderr, "LEN: %d.\n", r->head.msg_len);
		fprintf(stderr, "TO: %s.\n", r->head.to.name);
		fprintf(stderr, "TO_MOD_TYPE: 0x%x.\n", r->head.to_mod_type);
		fprintf(stderr, "CONTENT: %s.\n", r->data);

		sleep(1);
	}

	return 0;
}

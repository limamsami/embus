#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "embus.h"
#include "embus_common.h"

#define MY_MOD_NAME "recv"

#define BUF_SIZE (4096)

int main(void)
{
	embus_ctx_t *ec;
	embus_name_t n;
	embus_msg_t *m;
	embus_conn_t conn;

	char buf[EMBUS_MSG_LEN(BUF_SIZE)];
	
	int i = 0;
	int ret;
	
	ec = embus_construct(embus_make_name(&n, MY_MOD_NAME));
	if (!ec) {
		return -1;	
	}
	
	ec->io->io_timeout = 5;

	m = (embus_msg_t *)buf;
	
	while (1) {
		ret = embus_recv(ec, &conn, m, -1);
		if (ret == -1) {
			fprintf(stderr, "ERROR.\n");
			sleep(1);
			continue;
		}
		
		i++;

		fprintf(stderr, "\nMSG: %d\n", i);
		fprintf(stderr, "MAGIC: 0x%x.\n", m->head.magic);
		fprintf(stderr, "TYPE: 0x%x.\n", m->head.msg_type);
		fprintf(stderr, "ATTR: 0x%x.\n", m->head.msg_attr);
		fprintf(stderr, "LEN: %d.\n", m->head.msg_len);
		fprintf(stderr, "TO: %s.\n", m->head.to.name);
		fprintf(stderr, "TO_MOD_TYPE: 0x%x.\n", m->head.to_mod_type);
		fprintf(stderr, "CONTENT: %s.\n", m->data);
	}

	return 0;
}

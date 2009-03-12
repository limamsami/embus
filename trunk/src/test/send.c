#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "embus.h"
#include "embus_common.h"

#define MY_MOD_NAME "send"
#define HIS_MOD_NAME "recv"
#define MSG "Hello World!"

#define BUF_SIZE (4 * 1024)

#define TIMES 16384

int main(void)
{
	embus_ctx_t *ec;
	embus_name_t n;
	embus_msg_t *m;
	
	struct timeval before, after, v;

	char buf[EMBUS_MSG_LEN(BUF_SIZE)];
	
	unsigned long size = 0;

	int i = 0;
	int ret, len;

	ec = embus_construct(embus_make_name(&n, MY_MOD_NAME));
	if (!ec) {
		return -1;	
	}
	
	ec->io->io_timeout = 5;

	m = (embus_msg_t *)buf;
	len = strlen(MSG) + 1;
	
	fprintf(stderr, "Message Size: %d Count: %d.\n", len + sizeof(embus_msg_t) - 1, TIMES);

	embus_make_name(&m->head.to, HIS_MOD_NAME);
	
	embus_setup_local_peer_msg(m, len);
	strcpy(m->data, MSG);
	
	gettimeofday(&before, NULL);

	while (i < TIMES) {
		ret = embus_send(ec, m);
		if (ret == -1) {
			fprintf(stderr, "#");
			sleep(1);
			continue;
		}
		
		i++;
		size += len + sizeof(embus_msg_t) - 1;

	}

	gettimeofday(&after, NULL);
	timersub(&after, &before, &v);

	fprintf(stderr, "Data Size: %d, Seconds: %d, Microseconds: %d.\n", size, v.tv_sec, v.tv_usec);

	return 0;
}

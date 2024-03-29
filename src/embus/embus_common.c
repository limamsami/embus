/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Common function */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <poll.h>

#include "embus_def.h"
#include "embus_common.h"
#include "embus_debug.h"

embus_name_t *embus_make_name(embus_name_t *n, char *name)
{
	memset(n->name, 0, EMBUS_SZ_MOD_NAME + 1);
	strncpy(n->name, name, EMBUS_SZ_MOD_NAME);

	return n;
}

void *embus_zalloc(size_t size)
{
	void *p;

	p = malloc(size);
	
	if (p)
		memset(p, 0, size);

	return p;
}

int embus_set_nonblock(int fd)
{
	int tmp;
	
	tmp = fcntl(fd, F_GETFL);
	tmp |= O_NONBLOCK;

	return fcntl(fd, F_SETFL, tmp);
}

int embus_send_with_timeout(int sd, void *buf, size_t len, int timeout)
{
	struct pollfd pollfd;

	size_t io = 0;
	int ret;

	pollfd.fd = sd;
	pollfd.events = POLLOUT | POLLIN | POLLHUP;

	while (io != len) {
		ret = poll(&pollfd, 1, timeout);
		if (ret == -1 && errno == EINTR)
			continue;

		if (ret <= 0) 
			return -1;
		
		if (pollfd.revents & POLLHUP)
			return -1;

		if ((pollfd.revents & POLLOUT) || (pollfd.revents & POLLIN)) {
			ret = write(sd, buf + io, len - io);
			if ((ret == -1 && errno == EINTR) || (ret == -1 && errno == EAGAIN))
				continue;

			if (ret <= 0)
				return -1;
			
			io += ret;
		}
	}

	return io;
}

int embus_recv_with_timeout(int sd, void *buf, size_t len, int timeout)
{
	struct pollfd pollfd;

	size_t io = 0;
	int ret;

	pollfd.fd = sd;
	pollfd.events = POLLIN;

	while (io != len) {
		ret = poll(&pollfd, 1, timeout);
		if (ret == -1 && errno == EINTR)
			continue;

		if (ret <= 0) {
			DBG("Error on poll. errno: %d.", errno);
			return -1;
		}

		if (pollfd.revents & POLLIN) {
			ret = read(sd, buf + io, len - io);
			if (ret == 0)
				return io;

			if (ret == -1 && errno == EINTR)
				continue;

			if (ret < 0) {
				DBG("Error on read. ret: %d, errno: %d.", ret, errno);
				return -1;
			}

			io += ret;
		}
	}

	return io;
}

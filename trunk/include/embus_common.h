/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

#ifndef EMBUS_COMMON_H
#define EMBUS_COMMON_H

#include "embus_def.h"

embus_name_t *embus_make_name(embus_name_t *n, char *name);

void *zalloc(size_t size);

int set_nonblock(int fd);

int send_with_timeout(int sd, void *buf, size_t len, int timeout);

int recv_with_timeout(int sd, void *buf, size_t len, int timeout);

#endif /* Define EMBUS_COMMON_H */

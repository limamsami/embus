/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Common Definations */

#ifndef EMBUS_DEF_H
#define EMBUS_DEF_H

#define EMBUS_SZ_CHAIN_NAME	32
#define EMBUS_SZ_MOD_NAME	48
#define EMBUS_REMOTE_PROXY_NAME	"embus_remote_proxy"
#define EMBUS_LOCAL_PROXY_NAME	"embus_local_proxy"

enum {
	EMBUS_MOD_TYPE_LOCAL = 0,
	EMBUS_MOD_TYPE_REMOTE,
	EMBUS_MOD_TYPE_LOCAL_PROXY,
	EMBUS_MOD_TYPE_REMOTE_PROXY,
};

typedef struct {
	char name[EMBUS_SZ_MOD_NAME + 1];
}embus_name_t;

#endif /* Define EMBUS_DEF_H */

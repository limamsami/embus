/*
 * Author: coredump <coredump@live.cn>
 * License: GPLv2
 */

/* EMBUS Debug Function */

#ifndef EMBUS_DEBUG_H
#define EMBUS_DEBUG_H

#ifdef __EMBUS_DEBUG__
#include <stdio.h>
#define DBG(x...) do {	\
	fprintf(stderr, "Function: %s, Line: %d: ", __func__, __LINE__);	\
	fprintf(stderr, x);	\
	fprintf(stderr, "\n");	\
}while(0)
#else
#define DBG(x)
#endif

#endif /* Define EMBUS_DEBUG_H */

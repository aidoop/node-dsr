#ifndef __NODEDSRCONFIG_H_
#define __NODEDSRCONFIG_H_

#include <stdio.h>

// Debug messages
#define DRADEBUG
#ifdef DRADEBUG
#define DBGPRINT(fmt, args...) printf(fmt, ##args)
#else
#define DBGPRINT(fmt, args...)
#endif

// NodeDsr Number Limitation
#define NODEDSR_NUMBER_LIMIT (10)

#endif // __NODEDSRCONFIG_H_
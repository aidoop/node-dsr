#pragma once

#include <stdio.h>

// Debug messages
#define DRADEBUG
#ifdef DRADEBUG
#define DBGPRINT(fmt, args...) printf(fmt, ##args)
#else
#define DBGPRINT(fmt, args...)
#endif

// NodeDsr Number Limitation
#define NODEDSR_INSTANCE_LIMIT (10)

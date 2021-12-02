#pragma once

#include <stdio.h>

// Debug messages
#undef DRADEBUG
#ifdef DRADEBUG
#define DBGPRINT(fmt, args...) printf(fmt, ##args)
#else
#ifdef _WIN32
#define DBGPRINT(fmt, args, ...)
#else
#define DBGPRINT(fmt, args...)
#endif
#endif

// NodeDsr Number Limitation
#define NODEDSR_INSTANCE_LIMIT (10)

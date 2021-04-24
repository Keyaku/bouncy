#ifndef BOUNCY_API_H
#define BOUNCY_API_H

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define b_fprint(pipe, fmt, ...) \
	do { fprintf(pipe, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define b_print(fmt, ...) b_fprint(stdout, fmt, ##__VA_ARGS__)

#endif

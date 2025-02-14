#ifndef __SIGNALS_H__
#define __SIGNALS_H__

#include <setjmp.h>

pid_t run_child (size_t, int[], char*);
char *sig2msg (int, size_t *);
int getsig (char *);

pid_t run_with_jump (char*);
void reset_signal (int);

#endif

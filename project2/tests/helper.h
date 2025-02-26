#ifndef __helper_h__
#define __helper_h__

#include <mqueue.h>
#include <unistd.h>

pid_t test_server (char *, char *, char *);
char *build_mq (char *);

#endif

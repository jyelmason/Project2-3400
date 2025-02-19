#ifndef __pipe_h__
#define __pipe_h__

#include <unistd.h>

#define BUFFER_LENGTH 100

char *split_string (char *);
pid_t create_cksum_child (int *, const char *);
char *get_cksum (const char *);
char *fifo_server (char *, char *);

#endif

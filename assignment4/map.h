#ifndef __map_h__
#define __map_h__

size_t get_file_size (int);
char *open_index (const char *, int *, size_t *);
char *get_file_name (char *, size_t, size_t);
char *spawn_cksum (char *);

#endif

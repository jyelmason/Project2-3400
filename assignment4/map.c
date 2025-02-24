#include <assert.h>
#include <fcntl.h>
#include <spawn.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pipe.h"

#define LINE_LENGTH 12

/* Uses fstat() to get the file size for the provided file descriptor.
   Sample call:

     int fd = open ("data/f1.txt");
     size_t size = get_file_size (fd); // returns 6
*/
size_t
get_file_size (int fd)
{
	struct stat info;
	assert(fstat(fd, &info) >= 0);
	
	return info.st_size;
}

/* Opens the index file based on the provided name. Returns a pointer to
   the memory map of the file. Also sets the call-by-reference parameters
   fd (file descriptor for the open file) and size (the file size in bytes).
   Uses get_file_size() as a helper. Return NULL if anything fails, closing
   the file if needed. Sample call:

     int fd;
     size_t size;
     // map contains the contents of data/index, fd is set to the
     // opened file descriptor, and size is 36 (number of bytes in data/index)
     char *map = open_index ("data/index", &fd, &size);
*/
char *
open_index (const char *index, int *fd, size_t *size)
{
  assert (fd != NULL);
  assert (size != NULL);
  
	*fd = open(index, O_RDONLY);
  *size = get_file_size(*fd);
 	char *mmap_addr = mmap (NULL, *size, PROT_READ, MAP_PRIVATE, *fd, 0);
  
  if(*fd < 0 || mmap_addr == MAP_FAILED){
		close(*fd);
 		return NULL;
 	}
 	else
 		return mmap_addr;
}

/* Returns a pointer to the beginning of line number lineno. First check
   to make sure that the line number is valid (the position would not be
   beyond the filesize). Note that in this lab, all lines of the file are
   LINE_LENGTH characters long. Hence, line number i begins at offset
   i * LINE_LENGTH in the memory map. Sample call:

     // text_line points to "data/f2.txt" in the mapped file
     char *text_line = get_file_name (map, 1, 36);
*/
char *
get_file_name (char *map, size_t lineno, size_t filesize)
{
	if(lineno*LINE_LENGTH <= filesize)
		return &map[lineno*LINE_LENGTH];	

  return NULL;
}

/* Re-implementation of the create_cksum_child() in pipe.c. Instead of
   using the calls to pipe/fork/dup2/exec, combine the latter three
   into a call to posix_spawn(). Sample call:

     // sum is "3015617425 6 data/f1.txt\n" [with the newline]
     char *sum = spawn_cksum ("data/f1.txt");
*/
char *
spawn_cksum (char *filename)
{
  // Instead of using fork() and exec(), use the posix_spawn functions.
  // Note that you will need to add a close and dup2 action to the
  // set of file actions to run when spawning.

  // Use these parameters to posix_spawn(). You will need to set up
  // the posix_spawn_file_actions_t by (1) initializing it, (2) adding
  // a close action, and (3) adding a dup2 action so that STDOUT writes
  // to a pipe. After spawning the new process, make sure to call
  // posix_spawn_file_actions_destroy() on the actions to prevent
  // memory leaks.
  const char *path = "/usr/bin/cksum";
  char *const argv[] = { "cksum", filename, NULL };
  int pipefd[2];
  pid_t child = -1;
  assert(pipe(pipefd) == 0);
  
  posix_spawn_file_actions_t file_actions;
  posix_spawn_file_actions_init(&file_actions);
  posix_spawn_file_actions_addclose(&file_actions, pipefd[0]);
  posix_spawn_file_actions_adddup2(&file_actions, pipefd[1], STDOUT_FILENO);
  
  assert(posix_spawn(&child, path, &file_actions, NULL, argv, NULL) == 0);
  posix_spawn_file_actions_destroy(&file_actions);
 
  close(pipefd[1]); 
  
  // Parent code: read the value back from the pipe into a dynamically
  // allocated buffer. Wait for the child to exit, then return the
  // buffer.
  size_t buffer_size = 256;
  char *buffer = malloc(buffer_size);
  
  ssize_t bytes_read = read(pipefd[0], buffer, buffer_size-1);
  buffer[bytes_read] = '\0';
  waitpid(child, NULL, 0);
  close(pipefd[0]);
  
  return buffer;
}











#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "pipe.h"

/* Given a writable string buffer, find the first space and replace it
   with a '\0', thus ending the string at that location. Sample call:

     char buffer[] = "hello world";
     // result points to buffer, which prints as "hello"
     char *result = split_string (buffer);
*/
char *
split_string (char *buffer)
{
  int count = 0;
  while(buffer[count] != ' ')
  {
    count++;
  }
  buffer[count] = '\0';
  return buffer;
}

/* Creates a child process. If this fails, close the pipe and return NULL.
   If the child is created, use it to run the cksum program on the filename
   passed. Send the result back through the pipe. The parent process should
   just return the child's PID. Sample call:

     int fd[2];
     pipe (fd);
     pid_t child_pid = create_cksum_child (fd, "foo.txt");
*/
pid_t
create_cksum_child (int *pipe, const char *filename)
{
  pid_t child = fork();
  if(child < 0)
  {
    close(pipe[0]);
    close(pipe[1]);
    return child;
  }

  if(child == 0)
  {
    close(pipe[0]);
    dup2(pipe[1],STDOUT_FILENO);
    int ret = execlp("cksum","cksum",filename,NULL);
    if(ret == -1)
    {
      printf("execlp unsuccessful\n");
    }
  }

  
  
  return child;
}

/* Uses the cksum program on the input filename. Start by creating a pipe
   and calling create_cksum_child() to create the child process. If that
   fails, return NULL. Otherwise, Implement the parent here so that it
   reads the result of the cksum calculation from the pipe and returns
   the result. Sample call:

     // sum is "3015617425 6 data/f1.txt\n" [with the newline]
     char *sum = get_cksum ("data/f1.txt");
*/
char *
get_cksum (const char *filename)
{
  int fd[2];
  int result = pipe (fd);
  if(result < 0){
    return NULL;
  }
  char* buffer = calloc(1024, sizeof(char));

  pid_t child_pid = create_cksum_child (fd, filename);
  if(child_pid < 0)
  {
    printf("child failed\n");
    return NULL;
  }

  close(fd[1]);
  read(fd[0],buffer,1023);
  return buffer;
}

/* Opens two FIFOs, one for reading (O_RDONLY) and one for writing
   (O_WRONLY). Reads in the name of a file from the reading FIFO, then
   gets the cksum output from get_cksum(). Uses split_string() to get
   only the first token (full output would be something like
   "12345678   25   filename.txt"). Writes the cksum value (12345678)
   into the writing FIFO and returns that string. Make sure to close
   both FIFOs before returning. Sample call:

     // if received "data/f1.txt" from fifo.in,
     // writes "3015617425" to fifo.out and returns "3015617425"
     char *sum = fifo_server ("fifo.in", "fifo.out");
*/
char *
fifo_server (char *fifo_in, char *fifo_out)
{
  
  int read_fifo = open(fifo_in,O_RDONLY);
  if(read_fifo == -1)
  {
    fprintf (stderr, "Failed to open FIFO\n");
    unlink (fifo_in);
    return NULL;
  }
  
  int write_fifo = open(fifo_out,O_WRONLY);
  if(write_fifo == -1)
  {
    fprintf (stderr, "Failed to open FIFO\n");
    unlink (fifo_out);
    return NULL;
  }

  char buffer[256];
  read(read_fifo,buffer,256);
  char *cksum = get_cksum(buffer);
  char* cksum_val = split_string(cksum);
  write(write_fifo,cksum_val,strlen(cksum_val)+1);
  close(write_fifo);
  close(read_fifo);

  return cksum_val;
}

#include <assert.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "client.h"
#include "server.h"

/* Opens two message queues (mqreq and mqresp) to communicate a request
   for information about a file (filename) from the server.  The response
   from the server should be returned using the call-by-reference
   parameter (response). Returns true if successful, false otherwise. If
   any errors occur, be sure to close any opened message queues before
   returning. Note that the server requires any request received to have
   the type field of the ids_req_t struct to be set to REQUEST. The
   response from the server will have the ids_resp_t type field set to
   RESPONSE.
 */
bool
get_record (char *filename, char *mqreq, char *mqresp, ids_resp_t **response)
{
  mqd_t req_msg = mq_open(mqreq,O_WRONLY,0600,NULL);
  if(req_msg == -1)
  {
    perror("req_msg failed");
    return false;
  }
  mq_send(req_msg,filename,strlen(filename)+1,10);

  mqd_t resp_msg = mq_open(mqresp,O_RDONLY);
  if(resp_msg == -1)
  {
    perror("resp_msg failed");
    return false;
  }
  struct mq_attr attr;
  mq_getattr(mqresp,&attr);
  char *buffer = calloc(attr.mq_msgsize,1);
  unsigned int priority = 0;
  mq_recieve(resp_msg,buffer,attr.mq_msgsize,&priority);
  *response = buffer;

  free(buffer);
  mq_close(req_msg);
  mq_close(resp_msg);
  return true;
}

/* Given an ids_resp_t struct from the server for the specified file
   (filename), confirm that the fields are correct. You will need to use
   fstat() and execute the cksum program as in Assignment 4 to get the local
   information. Return true if all fields are correct, and false otherwise.
 */
bool
check_record (char *filename, ids_resp_t *response)
{
  // Use the following print format strings for correct results.
  // printf ("%s:\n", filename);
  // printf ("  permissions: %o\n", mode);
  // printf ("  size: %zu\n", size);
  // printf ("  cksum: %s\n", cksum);
  // If there is a mismatch, include " [MISMATCH X]" before the newline,
  // where X is the value you retrieve locally. For instance, if the size
  // is wrong, replace the printed message with:
  // printf ("  size: %zu [MISMATCH: %zu]\n", server_size, my_size);
  int pipefd[2];
  int result = pipe(pipefd);
  pid_t child = fork();
  if(child < 0)
  {
    return false;
  }

  if(child == 0)
  {
    close(pipefd[0]);
    dup2(pipefd[1],STDOUT_FILENO);
    int ret = execlp("cksum","cksum",filename,NULL);
    if(ret == -1)
    {
      printf("execlp unsuccessful\n");
    }
  }
  close(pipefd[1]);
  int size = 256;
  char buffer[size];
  int cksum = read(pipefd[0],buffer,size);
  
  return true;
}

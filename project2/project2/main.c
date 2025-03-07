#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "client.h"
#include "ids.h"
#include "server.h"
#include "support.h"

static bool get_args (int, char **, char **, int *, bool *, bool *);
static void usage ();

int
main (int argc, char **argv)
{
  char *pidfile = NULL; // server process's PID file
  int index = -1;       // index of first file to query
  bool k_flag = false;
  bool o_flag = false;

  if (!get_args (argc, argv, &pidfile, &index, &o_flag, &k_flag))
    {
      usage ();
      return EXIT_FAILURE;
    }

  if (index >= argc - 2)
    {
      usage ();
      return EXIT_FAILURE;
    }

  if (pidfile == NULL)
    pidfile = "PID";

  char *mqreq = argv[index++];  // name of the request message queue
  char *mqresp = argv[index++]; // name of the response message queue

	int count = 0;
  ids_resp_t *response;
  int buffer_size = argc-index;
  ids_entry_t buffer[buffer_size];
  
  if (access(pidfile, F_OK) == -1) 
  {
    if (!start_server(pidfile, mqreq, mqresp)) 
    	return EXIT_FAILURE;
  }
  
  if (access(pidfile, F_OK) == -1) 
  {
    if (!start_server(pidfile, mqreq, mqresp)) 
    	return EXIT_FAILURE;
  }
  for(int i = index; i < argc; i++)
  {
  	response = NULL;
    ids_entry_t file_entry;
  	if(get_record(argv[i], mqreq, mqresp, &response))
  	{
      strcpy(file_entry.filename, argv[i]);
      file_entry.mode = response->mode;
      file_entry.size = response->size;
      strcpy(file_entry.cksum, response->cksum);
	  	file_entry.valid = check_record(argv[i], response);
      buffer[count] = file_entry;
		}
		else
			printf("ERROR: Failed to get record for this_file_does_not_exist\n");// This works but it might be wrong
		free (response);

  }
  
  if(o_flag)
  {
  	int fd = open(argv[index++], O_WRONLY);
    write(fd,buffer,sizeof(buffer));
    struct stat file_stat;
    fstat(fd,&file_stat);
    ftruncate(fd,sizeof(buffer));
    void* map = mmap(NULL,file_stat.st_size,PROT_READ,MAP_SHARED,fd,0);
  }
  
  if(k_flag)
	  stop_server(pidfile);	 
  
  // Last step of stage 3
  //int fd = open(*pidfile, O_RDONLY);
  //ftruncate(fd, )
	


  return EXIT_SUCCESS;
}

/* Parse the command-line arguments. */
//TODO change function arguments (wittman told me to use getOpt)
static bool
get_args (int argc, char **argv, char **pidfile, int *index, bool *o_flag, bool *k_flag)
{
  int ch = 0;
  while ((ch = getopt (argc, argv, "p:okh")) != -1)
    {
      switch (ch)
        {
        case 'p':
          *pidfile = optarg;
          break;
        case 'o':
        	*o_flag = true;
        	break;
        case 'k':
        	*k_flag = true;
        	break;	  
        default:
          return false;
        }
    }
    
  *index = optind;
  return true;
}

static void
usage (void)
{
	printf ("ids, a message-queue based intrusion-detection system\n\n");
	printf ("usage: ids [options] mqreq mqresp file1 ...\n");
	printf ("file1 is a file on disk to query from the server to get\n");
	printf ("the file info from the last time it was checked.\n\n");
	printf ("mqreq and mqresp are used to identify the message queues\n");
	printf ("to communicate with the server.\n\n");
	printf ("options can be one or more of the following:\n");
	printf ("  -k          Kill the server after performing the queries\n");
	printf ("  -o outfile  Write the query results to the file \"outfile\"\n");
	printf ("  -p pidfile  Use \"pidfile\" to store the server's PID");
	printf (" (default \"PID\")\n");
}

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

static bool get_args (int, char **, char **, int *);
static void usage ();

int
main (int argc, char **argv)
{
  char *pidfile = NULL; // server process's PID file
  int index = -1;       // index of first file to query

  if (!get_args (argc, argv, &pidfile, &index))
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
  ids_resp_t *response;
  
  if (access(pidfile, F_OK) == -1) 
  {
    if (!start_server(pidfile, mqreq, mqresp)) 
    	return EXIT_FAILURE;
  }
  
  for(int i = index; i < argc; i++)
  {
  	response = NULL;
  	if(get_record(argv[i], mqreq, mqresp, &response))
  	{
	  	if(!check_record(argv[i], response))
	  		return EXIT_FAILURE;
		}
  }
	  	 
  free (response);
	


  return EXIT_SUCCESS;
}

/* Parse the command-line arguments. */
static bool
get_args (int argc, char **argv, char **pidfile, int *index)
{
  int ch = 0;
  while ((ch = getopt (argc, argv, "p:h")) != -1)
    {
      switch (ch)
        {
        case 'p':
          *pidfile = optarg;
          break;
        case 'o':
        	break;
        case 'k':
          stop_server(*pidfile);
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

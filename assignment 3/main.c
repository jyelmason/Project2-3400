/*
 * COMP 3400: Signals Assignment
 *
 * Name: 
 */

#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "signals.h"

#define SIGNAL_MAX 20

int cmdline (int, char**, size_t*, int**, bool*);

void
usage (void)
{
  printf ("Usage: sigs [options] [SIG ...]\n");
  printf (" Options are:\n");
  printf ("  -s N    Override signal number N\n");
}

int
main (int argc, char **argv)
{
  bool count_signals = false;
  size_t count = 0;
  int *signals = NULL;

  // Uses the login name of the user to coordinate the timing of the
  // parent and child processes
  char *semname = getlogin ();
  assert (semname != NULL);

  if (cmdline (argc, argv, &count, &signals, &count_signals) < 0)
    {
      usage ();
      return EXIT_FAILURE;
    }

  // optind is the index of the first non-flag argument
  if (optind == (argc - 1))
    {
      // MINIMUM CRITERIA: Only one signal to deal with. Use run_child()
      // to start a child process that sets up a custom signal handler.
      // To get started, ignore the last argument; the child process
      // itself will trigger a segfault. Later, use the command-line
      // signal by converting the command-line option from its string
      // name (e.g., SEGV) into its corresponding integer and send the
      // signal to the child process.

			pid_t child = run_child(count, signals, semname);
			int sig_num = getsig(argv[argc-1]);
			kill(child, sig_num);
 
		
      // Wait until the child exits, then clean up the signals array
      wait (NULL);
      if (signals != NULL)
        free (signals);
      return EXIT_SUCCESS;
    }

  // FULL CRITERIA: Use a long jump to capture a signal multiple times.
  // After starting a child process with run_with_jump(), send it the
  // TSTP signal twice, pausing for a second between each. 

	pid_t newChild = run_with_jump(semname);
	kill(newChild, SIGTSTP);
	kill(newChild, SIGTSTP);
  
  if (signals != NULL)
    free (signals);
  return EXIT_SUCCESS;
}

/* DO NOT MODIFY THIS FUNCTION. Parses the command line arguments to get
   the signal numbers to override, the number of overridden signals, and
   the order of signals to raise in the full implementation. */
int
cmdline (int argc, char **argv, size_t *overrides, int **signals, bool *count)
{
  bool override[SIGNAL_MAX];
  memset (override, 0, sizeof (override));
  int option;
  int sig;
  size_t overridden = 0;

  while ((option = getopt (argc, argv, "s:c")) != -1)
    {
      switch (option)
        {
        case 's':
          sig = atoi (optarg);
          if (sig == 0 || sig > SIGNAL_MAX)
            return -1;
          override[sig] = true;
          overridden++;
          break;
        case 'c':
          *count = true;
          break;
        default:
          return -1;
        }
    }

  if (overridden > 0)
    {
      *signals = calloc (overridden, sizeof (int));
      assert (*signals != NULL);
      size_t count = 0;
      if (override[SIGSEGV])
        (*signals)[count++] = SIGSEGV;
      if (override[SIGHUP])
        (*signals)[count++] = SIGHUP;
      if (override[SIGINT])
        (*signals)[count++] = SIGINT;
      if (override[SIGFPE])
        (*signals)[count++] = SIGFPE;
      if (override[SIGALRM])
        (*signals)[count++] = SIGALRM;
      *overrides = count;
    }

  return 0;
}

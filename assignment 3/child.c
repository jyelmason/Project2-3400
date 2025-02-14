#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "signals.h"

/* Signal handler for signals that are not overridden. Prints a message and
   exits. */
void
sig_unknown (int signum)
{
  write (STDOUT_FILENO, "NOT OVERRIDDEN\n", 15);
  exit (EXIT_SUCCESS);
}

/* Signal handler for overriden signals. Print out the appropriate message from
   using sig2msg() to translate the signal number to a string. Note that you
   cannot use printf(). You must use write(). */
void
sig_handler (int signum)
{
  // TODO: Using write(), print out the message for the signal, then exit.
  size_t length = 0;
  char *msg = sig2msg(signum,&length);
  write(STDOUT_FILENO, msg, length);
  write(STDOUT_FILENO, "\n",1);
  exit (EXIT_SUCCESS);
}

/* Create a child and override the signals listed in the array.
   The parent should return the child PID; child infinitely loops
   after overriding the signal handlers. */
pid_t
run_child (size_t number_of_signals, int signals[], char *semname)
{
  // Do NOT modify these three lines, which are necessary for the timing to
  // work properly.
  sem_unlink (semname);
  sem_t *start = sem_open (semname, O_CREAT, 0644, 0);
  if (start == NULL)
    return EXIT_FAILURE;

  // Call fork() to create a child. The parent's code is marked below.

  pid_t child = fork ();
  if (child < 0)
    return child;

  if (child == 0)
    {
      // The child process should override the signals specified in the array
      // with the sig_handler() function. Other signals that are not specified
      // in the array should be overriden to use sig_unknown() instead.
      //
      // NOTE: We are only dealing with SEGV, HUP, INT, FPE, and ALRM. Only
      // these five signals should be overriden.
      // Do NOT modify these two lines. They will let the parent know the
      // child has started, then enter an infinite loop until the signal
      // arrives. The child should not do anything after this.
      struct sigaction sa;
      memset(&sa, 0 ,sizeof(sa));
      sa.sa_handler = sig_handler;
      if(sigaction (SIGSEGV,&sa, NULL) == -1)
        printf("Failed to overwrite SIGSEGV.\n");
      sem_post (start);
      printf("Entering loop\n");
      while (1);
    }

  // Do NOT modify these three lines. These are the parent's code. It must
  // wait until the child starts before returning.
  sem_wait (start);
  sem_unlink (semname);
  return child;
}
